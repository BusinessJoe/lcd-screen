#include "esp_wifi.h"
#include "driver/i2c.h"
#include "rom/ets_sys.h"
#include "lcd.h"
#include "wifi.h"
#include "http.h"

#define I2C_SDA_GPIO 21
#define I2C_SCL_GPIO 22
#define I2C_FREQ_HZ 1000000
#define I2C_PORT I2C_NUM_0

// The screen is 4 rows by 20 columns; 80 characters
static uint8_t text[80];

void scan_addresses()
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

void init_i2c()
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

void clear_text_memory()
{
    for (int i = 0; i < 80; i++)
    {
        text[i] = ' ';
    }
}

void draw_text()
{
    return_home();
    clear_display();

    // The lines are drawn in order 1, 3, 2, 4.
    for (int i = 0; i < 20; i++)
    {
        send_char(text[i]);
    }
    for (int i = 0; i < 20; i++)
    {
        send_char(text[i + 40]);
    }
    for (int i = 0; i < 20; i++)
    {
        send_char(text[i + 20]);
    }
    for (int i = 0; i < 20; i++)
    {
        send_char(text[i + 60]);
    }
}

void on_message(char *message)
{
    memcpy(text, message, 80);
    draw_text();
}

void app_main()
{
    init_i2c();

    scan_addresses();

    // TODO: what does this section do?
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    start_webserver(on_message);

    // Disable cursor
    send_byte(0b00001100, LCD_RS_LOW);

    clear_text_memory();

    const char init_message[] = "Initialized!";
    for (int i = 0; init_message[i] != '\0'; i++)
    {
        text[i] = init_message[i];
    }
    draw_text();

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    clear_display();
}