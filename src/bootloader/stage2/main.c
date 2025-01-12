#include "stdio.h"
#include "x86.h"
#include "memory.h"
#include "memdetect.h"
#include "paging.h"

#include "../../libs/bootparams.h"
#include <stdint.h>

typedef struct {
    uint8_t     DriveNumber;
    uint8_t     Reserved;
    uint8_t     Signature;
    uint32_t    VolumeID;
    uint8_t     Volumelabel[11];
    uint8_t     SystemId[8];
} __attribute__((packed)) EBR;

typedef struct {
    uint8_t     BootJumpInstruction[3];
    uint8_t     OemIdentifier[8];
    uint16_t    BytesPerSector;
    uint8_t     SectorsPerCluster;
    uint16_t    ReservedSectors;
    uint8_t     FatCount;
    uint16_t    DirEntryCount;
    uint16_t    TotalSectors;
    uint8_t     MediaDescriptorType;
    uint16_t    SectorsPerFat;
    uint16_t    SectorsPerTrack;
    uint16_t    Heads;
    uint32_t    HiddenSectors;
    uint32_t    LargeSectorCount;
    EBR         ebr;
} __attribute__((packed)) BPB;


typedef struct {
    uint8_t     DriveAttributes;
    uint8_t     CHSAddressStart[3];
    uint8_t     PartitionType;
    uint8_t     CHSAddressLast[3];
    uint32_t    LBAStart;
    uint32_t    NumberOfSectors;
} __attribute__((packed)) PartitionRecord;

typedef struct {
    uint8_t         BootStrapCode[440];
    uint32_t        DiskID;
    uint16_t        reserved;
    PartitionRecord part[4];
    uint16_t        bootSignature;

} __attribute__((packed)) MBR;

/*  This is the main entry point to the kernel loader (stage2.bin)
    This module will do the following:

    1.  Collect memory information about the system, and store it in a suitable memory location, 
        the pass a pointer the kernel

    2.  Load the fat and directory into memory
        2.1     The fat is located immediadly after the boot loader, and it's size and quantity are located at byte offsets
                Number of FATs      0x10    (1 byte)
                Sectors per FAT     0x16    (2 bytes)
        2.2     This means we will need to ensure that the area we load the fat has (number of fats * sectors per FAT * 512) bytes available
        2.3     The directory is located after the fat and any reserved sectors, eg if the FAT has 252 sectors, and there are 2 FATs, with 1 
                reserved sector, the directory would be located 505 sectors from the beginning of the partitions
        2.4     The size of the directory is the number of root directory entries (offset 0x11) * 32 (this is in bytes, and must be divied by 512 to get sectors)

        2.5     The boot loaded is loaded in the first sector of the active partition, we will need to parse the partition table to get the LBA offset for the active partition.
                The MBR is at location 0x0000600, so we could create a structure and just point it at this memory location.

        

    3.  Search the directory for "kernel.bin"

    4.  Load "kernel.bin" into memory, following any fat16 clusters

    5.  jmp to the location of the loaded kernel to start execution

    We currently know about the following information that is loaded into memory:
    1.  The MBR is loaded at 0x0:0x600              0x00000600
    2.  The Stage1 bootloader is at 0x0:0x7c00      0x00007c00
    3.  The bootdrive has been passed to the stage2 entrty point.

*/

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));
uint32_t kernel_page_table[1024] __attribute__((aligned(4096)));


uint8_t* Kernel = (uint8_t*)0xc0000000;

typedef void (*KernelStart)(BootParams* bootParams);

void __attribute__((cdecl)) start(uint16_t bootDrive)
{
    DAP dap;

    MBR* pMBR = (void*)0x0600;  // location of MBR
    BPB* pBPB = (void*)0x7c00;  // locattion of stage1 bootloader, not overwritten

   clrscr();

// Find the active partition
    int activePartition = 0xff;
    for(int i = 0; i < 4; i++)
    {
        if(pMBR->part[i].DriveAttributes & 0x80) {
            activePartition = i;
            break;
        }
    }

    uint32_t LBAStart = pMBR->part[activePartition].LBAStart;

// setup the read for the FAT
    int FATSize = pBPB->SectorsPerFat * pBPB->FatCount;

    dap.bufferOffset = 0x0000;
    dap.BufferSegment = 0x2000;
    dap.NumberOfBlocks = 1;
    dap.size = 0x10;
    dap.startingBlock = LBAStart + 1;

// calculate how many sets of 128 sectors we need to read
    int loop = FATSize / 128;
    int loopRemain = FATSize % 128;

    for(int i = 0; i < loop; i++)
    {
        int LBAOffset = (LBAStart + 1) + (i * 128);
        int offset = (i * 128);


        dap.bufferOffset = offset;
        dap.startingBlock = LBAOffset;
        dap.reserved = 0;
        dap.NumberOfBlocks = 128;

        if(x86_Disk_Read_LBA(bootDrive, &dap))
            printf("Success\n");
    }  
// now read the last part of the FAT, if there is any left

    if(loopRemain)
    {
        int offset = ((loop) * 128);
        int LBAOffset = (LBAStart + 1) + ((loop) * 128);
        dap.bufferOffset = offset;
        dap.startingBlock = LBAOffset;
        dap.reserved = 0;
        dap.NumberOfBlocks = loopRemain;

        
        if(x86_Disk_Read_LBA(bootDrive, &dap))
        {
            
        }
    }

// Fat is now loaded, next load the directory this is located immediatly after the fat
// The location is partition LBA start + FATsize + reserved
    int directoryOffset = pMBR->part[activePartition].LBAStart + FATSize + pBPB->ReservedSectors;
    int directoryBlocks = (pBPB->DirEntryCount * 32) / 512;

// read the directory into memory location 0x00030000
    dap.BufferSegment = 0x3000;
    dap.bufferOffset = 0x0000;
    dap.NumberOfBlocks = directoryBlocks;
    dap.startingBlock = directoryOffset;

        if(x86_Disk_Read_LBA(bootDrive, &dap))
            printf("Directory Loaded\n");


// Find the enty for KERNEL.BIN

    char kernel[] = "KERNEL  BIN";

    int FileLocation = 0;
    bool FileFound = false;
    while((FileLocation < (pBPB->DirEntryCount * 32)) && !FileFound)
    {
        if(!memcmp(kernel, (void*)(0x30000 + FileLocation), 11))
            FileFound = true;
        else
            FileLocation += 32;
        // Compare File name with 'KERNEL  BIN'
    }

    uint32_t kernelSize;

    if(FileFound) {
        // load the kernel to location 0x60000
        // the first cluster of the file is found at byte 26 of the director entry
        // and the LBA is calculated by (cluster number * SectorPerCluster) + reservered sectors + fat + directrory - 2
        kernelSize = *(uint32_t*)(0x30000 + FileLocation + 28);
        printf("Kernel Size: %d\n", kernelSize);
        uint16_t kernelCluster = *(uint16_t*)(0x30000 + FileLocation + 26);
        uint32_t kernelLBA = (pBPB->SectorsPerCluster * kernelCluster) + (pBPB->FatCount * pBPB->SectorsPerFat) + pBPB->ReservedSectors;
        kernelLBA += pMBR->part[activePartition].LBAStart;
        bool finished = false;

        uint16_t* FAT = (uint16_t*)0x20000;
        dap.startingBlock = kernelLBA;
        dap.NumberOfBlocks = pBPB->SectorsPerCluster;
        dap.BufferSegment = 0x6000;
        dap.bufferOffset = 0x0000;

        int count = 0;

        while(!finished)
        {
            x86_Disk_Read_LBA(bootDrive, &dap);

// find next cluster, the fat is loaded at 0x20000.  
            uint16_t recordCluster = kernelCluster * 2;
            uint16_t recordBlock = recordCluster >> 9;
            uint16_t recordEntry = recordCluster & 0x1ff;

            uint16_t offset = (512 * recordBlock) + recordEntry;
            kernelCluster = FAT[offset / 2];

            if(kernelCluster == 0xffff)
                finished = true;
            else
            {
                uint32_t nextLBA = (pBPB->SectorsPerCluster * kernelCluster) + (pBPB->FatCount * pBPB->SectorsPerFat) + pBPB->ReservedSectors;
                nextLBA += pMBR->part[activePartition].LBAStart;
                printf("NextLBA 0x%x\n", nextLBA);
                dap.startingBlock = nextLBA;
                dap.NumberOfBlocks = pBPB->SectorsPerCluster;
            }   
            printf("0x%x 0x%x 0x%x\n", recordCluster, recordBlock, recordEntry);
            printf("0x%x\n", kernelCluster);

            memcpy((void*)0x10000 + (count * 0x2000), (void*)0x60000, 0x2000);
            count++;
        }
    }

    
    int i;
    for(i = 0; i < 1024; i++)
    {
        page_directory[i] = 0x00000002;
    }

    for(i = 0; i < 1024; i++)
    {
        first_page_table[i] = (i * 0x1000) | 3;
    }

        for(i = 0; i < 1024; i++)
    {
        kernel_page_table[i] = ((i + 1) * 0x100000) | 3;
    }

    page_directory[0] = ((uint32_t)first_page_table | 3);
    page_directory[768] = ((uint32_t)kernel_page_table | 3);

    
    LoadPageDirectory(page_directory);
    enablePaging();

    printf("Paging\n");

    memcpy((void*)0xc0000000, (void*)0x10000, kernelSize);


 //   relocate kernel to correct location
    BootParams params;

    params.kernelSize = kernelSize;
    params.BootDevice = bootDrive;

    MemoryDetect(&params.Memory);

    KernelStart kernelStart = (KernelStart)Kernel;
    kernelStart(&params);



end:
    goto end;
}