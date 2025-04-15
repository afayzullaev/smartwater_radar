#include "httprelated.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "sdcard.h"
#include "lowpowerfunctions.h"

static const char *TAG = "HTTPRELATED";

esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    main_app_t mobj;
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            printf("%.*s\n", evt->data_len, (char *)evt->data);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            // sleep_device();
            esp_restart();
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");

            break;
    }
    return ESP_OK;
}

static double roundf_val(float input, int decimal)
{
  int _pow = 1;
  for (int i = 0; i < decimal; i++)
  {
    _pow = _pow * 10;
  }
  // int _pow = (int)pow(10, decimal);
  double value = (int)(input * _pow + 0.5f);

  return (double)(value / _pow);
}

static char* returnasJSON(main_app_t mobj)
{
    char *jsonData = NULL;
    cJSON *chargingJSON = NULL;
    cJSON *bat_voltageJSON = NULL;
    cJSON *levelJSON = NULL;
    cJSON *velocityJSON = NULL;
    cJSON *latJSON = NULL;
    cJSON *longJSON = NULL;
    cJSON *imeiJSON = NULL;
    cJSON *signalJSON = NULL;
    cJSON *temperatureJSON = NULL;

    cJSON *https_data = cJSON_CreateObject();

    signalJSON = cJSON_CreateNumber(mobj.signal_quality);
    levelJSON = cJSON_CreateNumber(roundf_val(mobj.sensor.measured_distance, 2));
    velocityJSON = cJSON_CreateNumber(mobj.sensor.velocity);
    // latJSON = cJSON_CreateString(mobj.gnss.latitude);
    // longJSON = cJSON_CreateString(mobj.gnss.longitude);
    latJSON = cJSON_CreateString("0");
    longJSON = cJSON_CreateString("0");
    bat_voltageJSON = cJSON_CreateNumber(mobj.bms.bat_voltage);
    chargingJSON = cJSON_CreateBool(mobj.bms.charging);
    imeiJSON = cJSON_CreateString(mobj.imei);
    temperatureJSON = cJSON_CreateString("0");
    
    cJSON_AddItemToObject(https_data, "imei", imeiJSON);
    cJSON_AddItemToObject(https_data, "signal_quality", signalJSON);
    cJSON_AddItemToObject(https_data, "temperature", temperatureJSON);
    cJSON_AddItemToObject(https_data, "latitude", latJSON);
    cJSON_AddItemToObject(https_data, "longitude", longJSON);
    cJSON_AddItemToObject(https_data, "v_battery", bat_voltageJSON);
    cJSON_AddItemToObject(https_data, "charging", chargingJSON);
    cJSON_AddItemToObject(https_data, "measured_distance", levelJSON);
    cJSON_AddItemToObject(https_data, "velocity", velocityJSON);
    // cJSON_AddItemToObject(https_data, "volume", velocityJSON);

    jsonData = cJSON_Print(https_data);
    printf("%s\n", jsonData);
    cJSON_Delete(https_data);
    return jsonData;
}

uint8_t http_retry = 0;

void https_post(main_app_t mobj)
{
    ESP_LOGI(TAG, "http post function");
    // POST request body
    const char *post_data = returnasJSON(mobj);

    esp_http_client_config_t config = {
        .url = SERVER_URL,
        .event_handler = http_event_handler,
        .auth_type = HTTP_AUTH_TYPE_NONE,
        .method = HTTP_METHOD_POST,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .keep_alive_enable = true,
        .timeout_ms = pdMS_TO_TICKS(10000),
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    //Set HTTP method and POST data
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_http_client_set_header(client, "Content-Type", "application/json");
    ESP_LOGI(TAG, "parameters is set");
    // Perform the HTTPS POST request
    char logger[64];
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTPS POST Status = %d, content_length = %lli",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
                 if(esp_http_client_get_status_code(client) > 250 && http_retry < 5)
                 {
                    http_retry++;
                    https_post(mobj);
                 }
        sprintf(logger, "HTTPS POST Status = %d, content_length = %lli",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
        system_logging(PATH_SYSTEM,logger);
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
        memset(logger, 0, sizeof(logger));
        sprintf(logger, "|ERROR| => HTTP POST request failed: %s", esp_err_to_name(err));
        system_logging(PATH_SYSTEM,logger);
    }
    // Clean up
    esp_http_client_cleanup(client);


    // esp_http_client_config_t esp_http_client_config = {
    //     .url = "http://api.quotable.io/random",
    //     .method = HTTP_METHOD_GET,
    //     .event_handler = http_event_handler};
    // esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
    // esp_http_client_set_header(client, "Content-Type", "application/json");
    // esp_err_t err = esp_http_client_perform(client);
    // if (err == ESP_OK)
    // {
    //     ESP_LOGI(TAG, "HTTPS POST Status = %d, content_length = %lli",
    //              esp_http_client_get_status_code(client),
    //              esp_http_client_get_content_length(client));
    // }
    // else
    // {
    //     ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    // }
    // esp_http_client_cleanup(client);
}