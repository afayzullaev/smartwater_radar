#include "main.h"
#include "modemrelated.h"
#include "internet.h"
#include "httprelated.h"
#include "sdcard.h"
#include "lowpowerfunctions.h"
#include "esp_netif.h"
#include "esp_netif_ppp.h"
#include "lowpowerfunctions.h"
#include "esp_event.h"

static const char *TAG = "INTERNET";

esp_modem_dce_t *dce = NULL;
esp_netif_t *esp_netif = NULL;

QueueHandle_t internetqueue = NULL;

static void on_ppp_changed(void *arg, esp_event_base_t event_base,
                           int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "PPP state changed event %" PRIu32, event_id);
    if (event_id == NETIF_PPP_ERRORUSER) {
        /* User interrupted event from esp-netif */
        esp_netif_t **p_netif = event_data;
        ESP_LOGI(TAG, "User interrupted event from netif:%p", *p_netif);
    }
}

static void on_ip_event(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "IP event! %" PRIu32, event_id);
    uint32_t ipstatus;
    if (event_id == IP_EVENT_PPP_GOT_IP) {
        esp_netif_dns_info_t dns_info;
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        esp_netif_t *netif = event->esp_netif;
        ESP_LOGI(TAG, "Modem Connect to PPP Server");
        ESP_LOGI(TAG, "~~~~~~~~~~~~~~");
        ESP_LOGI(TAG, "IP          : " IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI(TAG, "Netmask     : " IPSTR, IP2STR(&event->ip_info.netmask));
        ESP_LOGI(TAG, "Gateway     : " IPSTR, IP2STR(&event->ip_info.gw));
        esp_netif_get_dns_info(netif, 0, &dns_info);
        ESP_LOGI(TAG, "Name Server1: " IPSTR, IP2STR(&dns_info.ip.u_addr.ip4));
        esp_netif_get_dns_info(netif, 1, &dns_info);
        ESP_LOGI(TAG, "Name Server2: " IPSTR, IP2STR(&dns_info.ip.u_addr.ip4));
        ESP_LOGI(TAG, "~~~~~~~~~~~~~~");
        ESP_LOGI(TAG, "GOT ip event!!!");
        ipstatus = GOT_IP_BIT;
        xQueueSend(internetqueue, &ipstatus, pdMS_TO_TICKS(10000));
    } else if (event_id == IP_EVENT_PPP_LOST_IP) {
        ESP_LOGI(TAG, "Modem Disconnect from PPP Server");
        ipstatus = LOST_IP_BIT;
        xQueueSend(internetqueue, &ipstatus, pdMS_TO_TICKS(10000));
    } else if (event_id == IP_EVENT_GOT_IP6) {
        ESP_LOGI(TAG, "GOT IPv6 event!");
        ip_event_got_ip6_t *event = (ip_event_got_ip6_t *)event_data;
        ESP_LOGI(TAG, "Got IPv6 address " IPV6STR, IPV62STR(event->ip6_info.ip));
    }
}




void send_data_to_server(main_app_t mobj){
    internetqueue = xQueueCreate(1,sizeof(uint32_t));
    if( dce != NULL){
        esp_modem_destroy(dce);
    }
    if( esp_netif != NULL){
        esp_netif_destroy(esp_netif);
    }
    esp_event_loop_delete_default();
    vTaskDelay(pdMS_TO_TICKS(1000)); 

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &on_ip_event, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(NETIF_PPP_STATUS, ESP_EVENT_ANY_ID, &on_ppp_changed, NULL));

    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG(APN);
    esp_netif_config_t netif_ppp_config = ESP_NETIF_DEFAULT_PPP();
    esp_netif = esp_netif_new(&netif_ppp_config);
    assert(esp_netif);

    esp_modem_dte_config_t dte_config = ESP_MODEM_DTE_DEFAULT_CONFIG();

    dte_config.uart_config.tx_io_num = GSM_TX;
    dte_config.uart_config.rx_io_num = GSM_RX;
    dte_config.uart_config.rts_io_num = (-1);
    dte_config.uart_config.cts_io_num = (-1);
    dte_config.uart_config.flow_control = ESP_MODEM_FLOW_CONTROL_NONE;
    dte_config.uart_config.rx_buffer_size = 4096;
    dte_config.uart_config.tx_buffer_size = 1024;
    dte_config.uart_config.event_queue_size = 60;
    dte_config.task_stack_size = 8192;
    dte_config.task_priority = 5;
    dte_config.dte_buffer_size = 1024;

    ESP_LOGI(TAG, "Initializing esp_modem for the A7670 module...");
    dce = esp_modem_new_dev(ESP_MODEM_DCE_SIM7600, &dte_config, &dce_config, esp_netif);
    assert(dce);
    mobj.signal_quality = check_signal_quality(dce);

    vTaskDelay(pdMS_TO_TICKS(1000));
    char at_data[16] = {0};
    memset(at_data, 0, sizeof(at_data));

    if (esp_modem_get_imei(dce,at_data) == ESP_OK)
    {
        ESP_LOGI(TAG, "GOT IMEI: %s", at_data);
        sprintf(mobj.imei,at_data);
    }else{
        ESP_LOGE(TAG, "CAN'T GOT IMEI");
    }
    // mobj.gnss = get_gnss_data(dce);
    // char data_log[256];
    // sprintf(data_log,"%16s|%12.02f|%12.02f|%13.02f|%12.02f|%12.02f|%12d|", mobj.imei,mobj.sensor.tds, mobj.sensor.pressure, mobj.sensor.temperature, mobj.sensor.level, mobj.bms.bat_voltage, mobj.signal_quality);
    // data_logging(PATH_DATA, "            IMEI|     TDS ppm|Pressure kPa|Temperature C|    Level cm|     Vbat mV|        RSSI|",data_log);
    // mobj.gnss = get_gnss_data(dce);
    if(esp_modem_set_mode(dce, ESP_MODEM_MODE_DATA) != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_modem_set_mode(ESP_MODEM_MODE_DATA) failed");
        system_logging(PATH_SYSTEM,"|ERROR| => esp_modem_set_mode(ESP_MODEM_MODE_DATA) failed");
        sleep_device();
    }else
    {
        ESP_LOGI(TAG, "DATA MODE is set");
    }
    uint32_t ip_status;
    ESP_LOGI(TAG, "Waiting for IP address");
    if (xQueueReceive(internetqueue, &ip_status, pdMS_TO_TICKS(30000)))
    {
        switch (ip_status)
        {
        case GOT_IP_BIT:
            ESP_LOGI(TAG, "GOT_IP_BIT");
            https_post(mobj);
            break;
        
        case LOST_IP_BIT:
            ESP_LOGE(TAG, "LOST_IP_BIT");
            system_logging(PATH_SYSTEM,"|ERROR| => LOST_IP_BIT");
            sleep_device();
            break;

        default:
            break;
        }
    }
    
}