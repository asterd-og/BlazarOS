#pragma once

#include <types.h>

#include <arch/io.h>

#include <lib/stdio/printf.h>

#define PCI_BAR0 0x10
#define PCI_BAR1 0x14
#define PCI_BAR2 0x18
#define PCI_BAR3 0x1C
#define PCI_BAR4 0x20
#define PCI_BAR5 0x24

#define PCI_MAX_BUS 256
#define PCI_MAX_SLOT 32
#define PCI_MAX_FUNC 8

enum {
    PCIVENDOR_INTEL                = 0x8086,
    PCIVENDOR_INTEL_ALT            = 0x8087,
    PCIVENDOR_VMWARE               = 0x15AD,
    PCIVENDOR_VMWARE_ALT           = 0x0E0F,
    PCIVENDOR_DELL                 = 0x1028,
    PCIVENDOR_DELL_ALT             = 0x413C,
    PCIVENDOR_ATI                  = 0x1002,
    PCIVENDOR_AMD                  = 0x1022,
    PCIVENDOR_VIA                  = 0x2109,
    PCIVENDOR_BENQ                 = 0x04A5,
    PCIVENDOR_ACER                 = 0x5986,
    PCIVENDOR_HP                   = 0x03F0,
    PCIVENDOR_HP_ALT               = 0x103C,
    PCIVENDOR_HP_ENT               = 0x1590,
    PCIVENDOR_ASUS                 = 0x1043,
    PCIVENDOR_ASUS_ALT             = 0x0B05,
    PCIVENDOR_INNOTEK              = 0x80EE,
    PCIVENDOR_REALTEK              = 0x10EC,
    PCIVENDOR_REAKLTEK_ALT         = 0x0BDA,
    PCIVENDOR_ENSONIQ              = 0x1274,
    PCIVENDOR_BROADCOM             = 0x14E4,
    PCIVENDOR_ATHEROS              = 0x168C,
};

void pci_init();

u16 pci_read_word(u8 bus, u8 slot, u8 func, u8 offset);
u16 pci_get_vendor(u8 buts, u8 slot, u8 func);