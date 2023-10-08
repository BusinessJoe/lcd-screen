#include "driver/i2c.h"
#include "rom/ets_sys.h"
#include "lcd.h"

#define I2C_SDA_GPIO 21
#define I2C_SCL_GPIO 22
#define I2C_FREQ_HZ 1000000
#define I2C_PORT I2C_NUM_0

void scan_addresses(void)
{
    uint8_t device_count = 0;
    uint8_t found_address;

    printf("beginning scan...\n");
    for (uint8_t i = 1; i < 127; i++)
    {
        int ret;
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        if (ret == ESP_OK)
        {
            printf("Found device at: 0x%2x\n", i);
            device_count++;
            found_address = i;
        }
    }
    printf("scan complete, found %d devices with most recent being %x\n", device_count, found_address);
    assert(device_count == 1);
    assert(found_address == LCD_ADDRESS);
}

void init_i2c(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SCL_GPIO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ_HZ,
        .clk_flags = 0,
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0));
}

void app_main(void)
{
    init_i2c();

    scan_addresses();

    // Return home
    send_byte(0x2, LCD_RS_LOW);
    ets_delay_us(3000);

    // Clear display
    send_byte(0x1, LCD_RS_LOW);
    ets_delay_us(3000);

    send_string("Hello world");
}