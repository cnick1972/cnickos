ENTRY(start)
phys = 0x00100000;

SECTIONS
{
    . = phys;

    .entry              : ALIGN (4K) { __entry_start = .;      *(.entry)   }
    .text               : ALIGN (4K) { __text_start = .;       *(.text)    }
    .data               : ALIGN (4K) { __data_start = .;       *(.data)    }
    .rodata             : ALIGN (4K) { __rodata_start = .;     *(.rodata)  }
    .bss                : ALIGN (4K) { __bss_start = .;        *(.bss)     }
    
    .end                : ALIGN (4K) { __end = .;               *(.end)     }
}