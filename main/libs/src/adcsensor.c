#include "adcsensor.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

const static char *TAG = "ADC";

static int adc_raw[2][10];
static int voltage[2][10];

static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);


bms_t get_bms_data(void)
{
    bms_t bms;
    //-------------ADC1 Init---------------//
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, BATTERY_CHANNEL, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, SOLAR_CHANNEL, &config));

    //-------------ADC1 Calibration Init---------------//
    adc_cali_handle_t adc1_cali_chan0_handle = NULL;
    adc_cali_handle_t adc1_cali_chan1_handle = NULL;
    bool do_calibration1_chan0 = adc_calibration_init(ADC_UNIT_1, BATTERY_CHANNEL, ADC_ATTEN_DB_12, &adc1_cali_chan0_handle);
    bool do_calibration1_chan1 = adc_calibration_init(ADC_UNIT_1, SOLAR_CHANNEL, ADC_ATTEN_DB_12, &adc1_cali_chan1_handle);

    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, BATTERY_CHANNEL, &adc_raw[0][0]));
    ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, BATTERY_CHANNEL, adc_raw[0][0]);
    if (do_calibration1_chan0) {
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_chan0_handle, adc_raw[0][0], &voltage[0][0]));
        ESP_LOGI(TAG, "ADC%d Channel[%d] Battery Voltage: %d mV", ADC_UNIT_1 + 1, BATTERY_CHANNEL, voltage[0][0]);
        // bms.bat_voltage = voltage[0][0] * 7.0;
        bms.bat_voltage = voltage[0][0] * 2.0;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, SOLAR_CHANNEL, &adc_raw[0][1]));
    ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, SOLAR_CHANNEL, adc_raw[0][1]);
    if (do_calibration1_chan1) {
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_chan1_handle, adc_raw[0][1], &voltage[0][1]));
        ESP_LOGI(TAG, "ADC%d Channel[%d] Solar Voltage: %d mV", ADC_UNIT_1 + 1, SOLAR_CHANNEL, voltage[0][1]);
        if (voltage[0][1] > 1000)
        {
            bms.charging = true;
        }else{
            bms.charging = false;
        }
    }
    return bms;
}

/*---------------------------------------------------------------
        ADC Calibration
---------------------------------------------------------------*/
static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }

    *out_handle = handle;
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }

    return calibrated;
}