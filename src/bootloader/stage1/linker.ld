ENTRY(start)
phys = 0x7c00;

SECTIONS
{
    . = phys;
    .fsjump             : { *(.fsjump)          }
    .fsheaders          : { *(.fsheaders)       }
    .entry              : { __entry_start = .;
                            *(.entry)
                          }
    .text               : { *(.text)            }
    .data               : { *(.data)            }
    .rodata             : { *(.rodata)          }

    .bios_footer 0x7dfe : { SHORT(0xAA55)       }

    .bss                : { *(.bss)             }

}