#include "utils.h"

#define color_videobuff 0xb8000
#define CRTC_reg_addr 0x3d4
#define CRTC_reg_data 0x3d5

#define nr_columns 80 * 2 // 1st byte=color 2nd byte=character
#define nr_rows 25

#define default_cl_out 0x06

volatile unsigned char *video_buff = (volatile unsigned char*) color_videobuff;

u16_t v_x =0, v_y = 0;

void update_vbuff() {
    /* 
        Regarding the cursor color in standard vga:
        According to http://osdever.net/FreeVGA/vga/textcur.htm
        standard vga cursor color is (only) set by the color of the foreground 
        color  of the character that the cursor is superimposing.
    */

    // update cursor and video_buff

   u16_t position = v_x + v_y * (nr_columns / 2);

   _outb(CRTC_reg_addr, 0x0f);
   _outb(CRTC_reg_data, (u8_t) (position & 0xff));

   _outb(CRTC_reg_addr, 0x0e);
   _outb(CRTC_reg_data, (u8_t) ((position >> 8)& 0xff));

   video_buff = color_videobuff  + (v_x * 2) + (v_y * nr_columns);   
}

void kputc(const char c) {        
    switch (c) {            
        case '\n': // Moves position to start of the next line v_y++
            v_x=0;
            v_y++;  
            break;
        case '\r': // Moves position to start of the current line v_y
            v_x=0;
            break;
        case '\b': // Moves position one column before the current position, if not start of line.
            if (v_x != 0) 
            v_x--;
            break;
        case '\a': // Moves position one column before the current position, if not start of line.
                /*beep*/
            break;
        default:            
            if (v_y >= nr_rows)
                scrolldwn();
            if (v_x == nr_columns / 2) {
                v_y++; v_x = 1;
            } else 
                v_x++;
    
            *video_buff++=c; 
            *video_buff++=default_cl_out;                 
            break;    
    }
    update_vbuff();  
}


void scrolldwn() {
    volatile unsigned char *t = (volatile unsigned char*) color_videobuff;
    volatile unsigned char *s =  t + nr_columns;

    for (u16_t i = 0 ; i < ((v_x*2) + (v_y * nr_columns)) - nr_columns; i++, *t++ = *s++);        

    v_y--;

    *t = color_videobuff  + (v_x*2) + (v_y * nr_columns);
    for (u16_t i = 0 ; i < nr_columns + (nr_columns-v_x); i++, *t++ = 0);
    
    update_vbuff();  
}

void clearconsole() {
    volatile unsigned char *t = (volatile unsigned char*) color_videobuff;    
    
    for (u16_t i = 0 ; i < (nr_columns * nr_rows); i++, *t++ = 0x0, *t++ = default_cl_out); 

    v_y = v_x = 0;

    update_vbuff();  
}

//void consoleCursoroff