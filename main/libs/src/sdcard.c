#include "sdcard.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include <stdlib.h>

static const char *TAG = "SDCARD";
static const char *BASE_PATH = "/store";


static void read_file(char *path)
{
    ESP_LOGI(TAG, "reading file %s", path);
    FILE *file = fopen(path, "r");
    char buffer[100];
    fgets(buffer, 99, file);
    fclose(file);
    ESP_LOGI(TAG, "file contains: %s", buffer);
}

static void write_file(char *path, char *content)
{
    ESP_LOGI(TAG, "Writing \"%s\" to %s", content, path);
    FILE *file = fopen(path, "a");
    fputs("\r\n", file);
    fputs(content, file);
    fclose(file);
}

void data_logging(char *path, char *header,char *content)
{
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};

    spi_bus_config_t spi_bus_config = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_CLK,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1};

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    ESP_ERROR_CHECK(spi_bus_initialize(host.slot, &spi_bus_config, SDSPI_DEFAULT_DMA));

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_CS;
    slot_config.host_id = host.slot;

    sdmmc_card_t *card;
    if (esp_vfs_fat_sdspi_mount(BASE_PATH, &host, &slot_config, &mount_config, &card) == ESP_OK)
    {
        ESP_LOGI(TAG, "Card is mounted");
        sdmmc_card_print_info(stdout, card);
        //////////////////
        write_file(path, header);
        write_file(path, content);

        esp_vfs_fat_sdcard_unmount(BASE_PATH, card);       
    }else{
        ESP_LOGI(TAG, "Failed to mount Card, maybe card doen't exist");
    }
    spi_bus_free(host.slot);
}

void system_logging(char *path, char *content)
{
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};

    spi_bus_config_t spi_bus_config = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_CLK,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1};

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    ESP_ERROR_CHECK(spi_bus_initialize(host.slot, &spi_bus_config, SDSPI_DEFAULT_DMA));

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_CS;
    slot_config.host_id = host.slot;

    sdmmc_card_t *card;
    if (esp_vfs_fat_sdspi_mount(BASE_PATH, &host, &slot_config, &mount_config, &card) == ESP_OK)
    {
        ESP_LOGI(TAG, "Card is mounted");
        sdmmc_card_print_info(stdout, card);
        //////////////////
        write_file(path, content);

        esp_vfs_fat_sdcard_unmount(BASE_PATH, card);       
    }else{
        ESP_LOGI(TAG, "Failed to mount Card, maybe card doen't exist");
    }
    spi_bus_free(host.slot);
}