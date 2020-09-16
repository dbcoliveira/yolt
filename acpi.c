#include "utils.h"
#include "acpi.h"

u32_t* acpi_RSDP_table() {
    u32_t *ptr = NULL;
    for (ptr = (u32_t *) RSDT_BEGIN; ptr < (u32_t*) RSDT_END; ptr++){
        if ((*ptr == RSDT_SIG_B) && (*(ptr+1) == RSDT_SIG_E)) {
            return ptr;
        }            
    }
}
