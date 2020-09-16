#include "utils.h"

typedef __builtin_va_list __gnuc_va_list;

#define va_arg(l,t) __builtin_va_arg(l,t)
#define va_start(a,f) __builtin_va_start(a,f)
#define va_end(a) __builtin_va_end(a)
#define va_list __builtin_va_list

doprint(const char *fmt, va_list *args) {
    register char c;
    int base = 0;
    int num_pad = 0;
    
    while ((c = *fmt) != 0) {            
        if (c !='%'){
            kputc(c);
            *fmt++;            
            continue;
        }
        *fmt++;
        cycle:
        c = *fmt;
        switch (c){
            case '0' ...'9':
                if (num_pad == 0)
                    num_pad = c - '0';
                else 
                    num_pad = (num_pad *10) + (c - '0');
                *fmt++;
                goto cycle;
                break;
            case '%':
                    kputc(c);
                    *fmt++;
                    break;
            case 's': {
                unsigned char *s = va_arg(*args,  char *);                
                if (s == (char*) 0)
                    s ="";
                while(*s != 0) 
                    kputc((unsigned char)*s++);
                *fmt++;
                break;
            }
            case 'd':
            case 'i': base = 10;
                      goto num;
            case 'x': base = 16;
            num: {     
                static char *s = NULL, *sn = NULL;
                char *arg = va_arg(*args, u64_t);
                
                if (num_pad > 0) {                                                           
                    sn = itoa(arg, base);
                    int e = strlen(sn);
                    if (num_pad > e){
                        memset(s, '0', num_pad);
                        for (; e >= 0; e--, num_pad-- )
                            s[num_pad] = sn[e];
                    }else
                        s = sn;                    
                } else 
                    s = itoa(arg, base);
                
                num_pad = 0;                                        
                while(*s != 0)
                    kputc((unsigned char)*s++);
                *fmt++;                
                s = sn = NULL;
                break;
            }                        
        }
    }
}

void kprintf(const char *fmt, ...) {
    
    va_list args;
    
    va_start(args,fmt);
    doprint(fmt, args);
    va_end(args);
}