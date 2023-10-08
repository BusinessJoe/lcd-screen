#include "lcd.h"
#include "driver/i2c.h"
#include "rom/ets_sys.h"

/* Private functions */
void _send_nibble(uint8_t byte, LcdRs rs);
void _pulse_enable(uint8_t data);

void _pulse_enable(uint8_t data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LCD_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, data | LCD_ENABLE, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    ets_delay_us(1);

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LCD_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, data & ~LCD_ENABLE, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    ets_delay_us(50);
}

void _send_nibble(uint8_t nibble, LcdRs rs)
{
    uint8_t data = nibble | rs | LCD_BACKLIGHT;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LCD_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    _pulse_enable(data);
}

// Sends a single byte to the LCD screen
void send_byte(uint8_t byte, LcdRs rs)
{
    _send_nibble(byte & 0xf0, rs);
    _send_nibble((byte << 4) & 0xf0, rs);
}

void clear_display()
{
    send_byte(0x1, LCD_RS_LOW);
    ets_delay_us(3000);
}

void return_home()
{
    send_byte(0x2, LCD_RS_LOW);
    ets_delay_us(3000);
}

void send_char(char c)
{
    send_byte(c, LCD_RS_HIGH);
    ets_delay_us(100);
}

void send_string(char *s)
{
    for (; *s != '\0'; s++)
    {
        send_char(*s);
    }
}