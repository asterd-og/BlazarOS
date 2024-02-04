#include <dev/pci.h>
#include <arch/acpi/mcfg.h>
#include <arch/acpi/acpi.h>

/* Thanks to https://github.com/KM198912 */
const char* pci_get_name(uint16_t vendor_id, uint16_t device_id)
{
    // intel devices
    if (vendor_id == PCIVENDOR_INTEL)
    {
        switch (device_id)
        {
            case 0x1237: { return "Intel 440FX Chipset"; }
            case 0x7000: { return "Intel 82371SB PIIX3 ISA"; }
            case 0x7010: { return "Intel 82371SB PIIX3 IDE"; }
            case 0x7020: { return "Intel 82371SB PIIX3 USB"; }
            case 0x7111: { return "Intel 82371AB/EB/MB PIIX4 IDE"; }
            case 0x7113: { return "Intel 82371AB/EB/MB PIIX4 ACPI"; }
            case 0x100E: { return "82540EM Gigabit Ethernet Controller"; }
            case 0x0041: { return "Intel Core PCI Express x16 Root Port"; }
            case 0x0042: { return "Intel Core IGPU Controller"; }
            case 0x0044: { return "Intel Core DRAM Controller"; }
            case 0x0600: { return "Intel RAID Controller"; }
            case 0x061F: { return "Intel 80303 I/O Processor"; }
            case 0x2415: { return "Intel 82801AA AC'97 Audio"; }
            case 0x2668: { return "Intel 82801(ICH6 Family) HD Audio"; }
            case 0x2770: { return "Intel 82945G Memory Controller Hub"; }
            case 0x2771: { return "Intel 82945G PCI Express Root Port"; }
            case 0x2772: { return "Intel 82945G Integrated Graphics"; }
            case 0x2776: { return "Intel 82945G Integrated Graphics"; }
            case 0x27D0: { return "Intel NM10/ICH7 PCI Express Port 1"; }
            case 0x27D2: { return "Intel NM10/ICH7 PCI Express Port 2"; }
            case 0x27C8: { return "Intel NM10/ICH7 USB UHCI Controller 1"; }
            case 0x27C9: { return "Intel NM10/ICH7 USB UHCI Controller 2"; }
            case 0x27CA: { return "Intel NM10/ICH7 USB UHCI Controller 3"; }
            case 0x27CB: { return "Intel NM10/ICH7 USB UHCI Controller 4"; }
            case 0x27CC: { return "Intel NM10/ICH7 USB2 EHCI Controller"; }
            case 0x244E: { return "Intel 82801 PCI Bridge"; }
            case 0x27DE: { return "Intel 82801G/ICH7 AC'97 Audio"; }
            case 0x27B8: { return "Intel 82801GB/ICH7 LPC Interface Bridge"; }
            case 0x27C0: { return "Intel NM10/ICH7 SATA Controller(IDE MODE)"; }
            case 0x27DA: { return "Intel NM10/ICH7 SMBus Controller"; }
            case 0x0F00: { return "Intel Atom Z36xxx/Z37xxx SoC Trans-Reg"; }
            case 0x0F31: { return "Intel Atom Z36xxx/Z37xxx Series Graphics"; }
            case 0x0F23: { return "Intel Atom E3800 Series SATA AHCI"; }
            case 0x0F18: { return "Intel Atom Z36xxx/Z37xxx Trusted Exec."; }
            case 0x0F04: { return "Intel Atom Z36xxx/Z37xxx HD Audio"; }
            case 0x0F48: { return "Intel Atom E3800 Series PCI Express Port 1"; }
            case 0x0F4A: { return "Intel Atom E3800 Series PCI Express Port 2"; }
            case 0x0F34: { return "Intel Atom Z36xxx/Z37xxx USB EHCI"; }
            case 0x0F1C: { return "Intel Atom Z36xxx/Z37xxx Series PCU"; }
            case 0x0F12: { return "Intel Atom E3800 Series SMBus Controller"; }
            case 0x7190: { return "Intel 440BX/ZX/DX Host Bridge"; }
            case 0x7110: { return "Intel 82371AB/EB/MB PIIX4 ISA"; }
            default:     { return "Unrecognized Intel Device"; }
        }
    }

    // newer intel devices
    if (vendor_id == PCIVENDOR_INTEL_ALT)
    {
        switch (device_id)
        {
            case 0x100E: { return "Intel 82540EM Ethernet Controller"; }
            default: { return "Unrecognized Intel ALT Device"; }
        }
    }

    // vmware
    if (vendor_id == PCIVENDOR_VMWARE)
    {
        switch (device_id)
        {
            case 0x0405: { return "VMWare SVGAII Adapter"; }
            case 0x0710: { return "VMWare SVGA Adapter"; }
            case 0x0770: { return "VMWare USB2 EHCI Controller"; }
            case 0x0720: { return "VMWare VMXNET Ethernet Controller"; }
            case 0x0740: { return "VMWare VM Communication Interface"; }
            case 0x0774: { return "VMWare USB1.1 UHCI Controller"; }
            case 0x0778: { return "VMWare USB3 xHCI 0.96 Controller"; }
            case 0x0779: { return "VMWare USB3 xHCI 1.00 Controller"; }
            case 0x0790: { return "VMWare PCI Bridge"; }
            case 0x07A0: { return "VMWare PCI Express Root Port"; }
            case 0x07B0: { return "VMWare VMXNET3 Ethernet Controller"; }
            case 0x07C0: { return "VMWare PVSCSI SCSI Controller"; }
            case 0x07E0: { return "VMWare SATA AHCI Controller"; }
            case 0x0801: { return "VMWare VM Interface"; }
            case 0x0820: { return "VMWare Paravirtual RDMA Controller"; }
            case 0x0800: { return "VMWare Hypervisor ROM Interface"; }
            case 0x1977: { return "VMWare HD Audio Controller"; }
            default: { return "Unrecognized VMWare Device"; }
        }
    }

    // vmware
    if (vendor_id == PCIVENDOR_VMWARE_ALT)
    {
        switch (device_id)
        {
            default: { return "Unrecognized VMWare Device"; }
        }
    }

    // innotek
    if (vendor_id == PCIVENDOR_INNOTEK)
    {
        switch (device_id)
        {
            case 0xBEEF: { return "VirtualBox Graphics Adapter"; }
            case 0xCAFE: { return "VirtualBox Guest Service"; }
            default: { return "Unrecognized InnoTek Device"; }
        }
    }

    // amd
    if (vendor_id == PCIVENDOR_AMD)
    {
        switch (device_id)
        {
            case 0x2000: { return "AMD 79C970 Ethernet Controller"; }
            default: { return "Unrecognized AMD Device"; }
        }
    }

    // ensoniq
    if (vendor_id == PCIVENDOR_ENSONIQ)
    {
        switch (device_id)
        {
            case 0x5000: { return "Ensoniq ES1370 [AudioPCI]"; }
            case 0x1371: { return "Ensoniq ES1371/ES1373/CT2518"; }
            default: { return "Unrecognized Ensoniq Device"; }
        }
    }

    // broadcom
    if (vendor_id == PCIVENDOR_BROADCOM)
    {
        switch (device_id)
        {
            case 0x1677: { return "NetXtreme BCM5751 Ethernet PCI-E"; }
            default: { return "Unrecognized Realtek Device"; }
        }
    }

    // realtek
    if (vendor_id == PCIVENDOR_REALTEK)
    {
        switch (device_id)
        {
            case 0x5289: { return "Realtek RTL8411 PCI Express Card Reader"; }
            case 0x8168: { return "Realtek RTL8111/8168/8411 Ethernet PCI-E"; }
            case 0x8139: { return "Realtek RTL8100/8101L/8139 Ethernet PCI-E"; }
            default: { return "Unrecognized Realtek Device"; }
        }
    }

    // atheros
    if (vendor_id == PCIVENDOR_ATHEROS)
    {
        switch (device_id)
        {
            case 0x0036: { return "Atheros AR9485 WiFi Adapter"; }
            default: { return "Unrecognized Atheros Device"; }
        }
    }

    // qemu graphics controller
    if (vendor_id == 0x1234 && device_id == 0x1111) { return "QEMU VGA Controller"; }

    // unknown
    return "Unrecognized Device";
}

u16 pci_read_word(u8 bus, u8 slot, u8 func, u8 offset) {
    u32 addr;
    u32 lbus = (u32)bus;
    u32 lslot = (u32)slot;
    u32 lfunc = (u32)func;
    
    addr = (u32)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) |
                 ((u32)0x80000000));
    
    outl(0xCF8, addr);

    return (u16)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
}

u32 pci_read_dword(u8 bus, u8 slot, u8 func, u8 offset) {
    return ((u32)pci_read_word(bus, slot, func, offset + 2) << 16) | pci_read_word(bus, slot, func, offset);
}

void pci_init() {
    u16 vendor, device;
    u32 bars[6];

    acpi_mcfg* mcfg = acpi_find_table("MCFG");

    mcfg_entry* entry = &mcfg->table[0];
    for (u64 bus = entry->first_bus; bus < entry->last_bus; bus++)
        for (u8 slot = 0; slot < PCI_MAX_SLOT; slot++)
            for (u8 func = 0; func < PCI_MAX_FUNC; func++) {
                vendor = pci_read_word(bus, slot, func, 0);
                if (vendor == 0xFFFF) continue;
                device = pci_read_word(bus, slot, func, 2);
                for (int i = 0; i < 6; i++) {
                    bars[i] = pci_read_dword(bus, slot, func, PCI_BAR0 + (sizeof(u32) * i));
                }
                printf("\n%s\n", pci_get_name(vendor, device));
            }
}