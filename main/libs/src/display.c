#include "main.h"

SSD1306_t dev;

void display_init(void){
    i2c_master_init(&dev, OLED_SDA, OLED_SCL, (-1));
    ssd1306_init(&dev, 128, 64);
    ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
    ssd1306_display_text(&dev, 0, " STARTING...", 13, false);
    ESP_ERROR_CHECK(i2cdev_init());
}

void write_text(char* buff, uint8_t row)
{
    ssd1306_display_text(&dev, row, buff, strlen(buff), false);
}

void clear_text(void)
{
    ssd1306_clear_screen(&dev, false);
}