#ifndef _ACPI_H
#define _ACPI_H

#define RSDT_BEGIN 0x000e0000
#define RSDT_END 0x000fffff

#define RSDT_SIG_B ' DSR'
#define RSDT_SIG_E ' RTP'

#define MADT_SIG "APIC"
#define SSDT_SIG "SSDT"

struct acpi_RSDptr {
    unsigned char rsd_signature[8];
    u8_t checksum;
    unsigned char OEM_ID[6];
    u8_t revision;
    u32_t rsdt_addr;
    u32_t length;
    u64_t xsdt_addr;
    u8_t ext_checksum;
    //reserved    
} __attribute__((packed));

struct acpi_RSDheader {
    unsigned char rsd_signature[8];
    u32_t length;
    u8_t revision;
    u8_t checksum;
    unsigned char OEM_ID[6];
    unsigned char OEM_TABLE_ID[8];
    u32_t OEM_revision;    
    u32_t creator_ID;    
    u32_t creator_revision;    
    u32_t entry[3]; // array of n
} __attribute__((packed));


#endif _ACPI_H