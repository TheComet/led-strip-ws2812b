/*
 * File:   main.c
 * Author: thecomet
 *
 * Created on 27 February 2016, 10:17
 */


#include <xc.h>

#define _XTAL_FREQ 32000000
#define Nop6() { \
        _asm \
            nop nop nop nop nop nop \
        _endasm }
        
#pragma config FOSC=INTOSC,LVP=OFF,MCLRE=OFF,WDTE=OFF
void cfg_clk(void)
{
    OSCCONbits.SCS = 0;
    OSCCONbits.IRCF = 0b1110;
    OSCCONbits.SPLLEN = 1;
    while(!OSCSTATbits.PLLR);
}

void cfg_ports(void)
{
    TRISCbits.TRISC2 = 0;
}

void cfg_timer(void)
{
    
}

static unsigned char grb_data[] = {
    0xFF, 0x00, 0x00,
    0xFF, 0x80, 0x00,
    0xFF, 0xFF, 0x00,
    0x80, 0xFF, 0x00,
    0x00, 0xFF, 0x00,
    0x00, 0xFF, 0xFF
};

void rotate_grb_data(void)
{
    unsigned char* grb_ptr = grb_data;
    unsigned char* grb_end_ptr = grb_data + sizeof(grb_data);
    unsigned char first_byte = *grb_ptr;
    while(++grb_ptr != grb_end_ptr)
        *(grb_ptr - 1) = *grb_ptr;
    *(grb_end_ptr - 1) = first_byte;
}

#define SEND_PIN PORTCbits.RC2
void send_data(void)
{
    unsigned char byte, i;
    unsigned char* grb_ptr = grb_data;
    unsigned char* grb_end_ptr = grb_data + sizeof(grb_data);
    
    while(grb_ptr != grb_end_ptr)
    {
        byte = *grb_ptr++;
        i = 8;
        while(i--)
        {
            if(byte & 0x80)
            {
                SEND_PIN = 1;
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                SEND_PIN = 0;
            }
            else
            {
                SEND_PIN = 1;
                asm("nop");
                asm("nop");
                SEND_PIN = 0;
            }
            byte <<= 1;
        }
    }
}

int main(void) {
    cfg_clk();
    cfg_ports();
    cfg_timer();
    
    while(1) {
        send_data();
        rotate_grb_data();
        rotate_grb_data();
        rotate_grb_data();
        __delay_ms(100);
    }
}
