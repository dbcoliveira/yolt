#include "utils.h"

char *itoa(u32_t val, u8_t base)
{
	static char buf[32] = {0};
	u8_t i=30;

	if (val == 0) return "0";

	for(; val && i ; i--, val /= base)
		buf[i] ="0123456789abcdef"[val % base];

	return &buf[i+1];
}

void strcat(char* destination, const char* source)
{
	char* ptr = destination + strlen(destination);

	while (*source != '\0')
		*ptr++ = *source++;

	*ptr = '\0';
	//return destination;
}

int strlen(const char *str)
{
	const char *s;

	for (s = str; *s; ++s) ;

	return (s - str);
}

char *strncpy(char *dst, const char *src, int n)
{
   int i;
   char *temp = dst;  
   for (i = 0; i < n; i++)
      *dst++ = *src++;
   return temp;
}

void *memset(void *b, char c, u16_t len)
{
    u16_t i = 0;
    unsigned char *p = b;
    while (len >0) 
    {
            *p = c;
            p++;
            len--;
    }
    return b;
}

int strncmp(const char *s1, const char *s2, u8_t n)
{
	if (n == 0)
		return 0;
	do {
		if (*s1 != *s2++)
			return *(unsigned char *)s1 - *(unsigned char *)--s2;
		if (*s1++ == 0)
			break;
	} while (--n != 0);
	
	return 0;
}
/*
int memcmp(void *s1, void *s2, u8_t n)
{
	if (n == 0)
		return 0;
	do {
		if (s1 != s2++)
			return *s1 - --*s2;
		if (s1++ == 0)
			break;
	} while (--n != 0);
	
	return 0;
}
*/

// ACPI reset
// cpu reset pin via 8042 keyboard controller.
void system_reboot_kb64(void)
{
		_outb(0x64, 0xfe);
}

void _system_halt(void)
{
	    for(;;);
        //__asm__ volatile ("cli\nhlt\n");
}

void _outb(u16_t port, u8_t v)
{
	__asm__ volatile (" outb %0, %1\n\t"
					: : "a" (v), "Nd"(port) );
}

u64_t _rdtsc(void)
{
	u64_t res;
	__asm__ volatile ("rdtsc"
						: "=A" (res));
	return res;
}

u64_t _ffz(u64_t n) {    
    u64_t i = 0;
    __asm__ volatile(" not %1\n\t"
                     " bsf %1, %0\n\t"
                     " jne 1f\n\t"
                     " movq $32, %0\n\t"                     
                     " 1:\n\t"
                     : "=r" (i)
                     : "r" (n) );
    return i;
}