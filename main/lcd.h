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

/* Commands */
void clear_display();
void return_home();

/* Data */
void send_char(char c);
void send_string(char *s);

// Set RS and send a single byte
void send_byte(uint8_t byte, LcdRs rs);
