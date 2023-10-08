#include <stdint.h>

#define LCD_ADDRESS 0x27
#define LCD_ENABLE 0x04

typedef enum LcdBacklight
{
    LCD_BACKLIGHT = 0x08,
    LCD_NOBACKLIGHT = 0x00,
} LcdBacklight;

typedef enum LcdRs
{
    LCD_RS_LOW = 0,
    LCD_RS_HIGH = 1,
} LcdRs;

void send_byte(uint8_t byte, LcdRs rs);
void send_char(char c);
void send_string(char *s);
void _send_nibble(uint8_t byte, LcdRs rs);
void _pulse_enable(uint8_t data);
