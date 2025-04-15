#include "modemrelated.h"
#include "esp_log.h"
#include "sdcard.h"

static const char *TAG = "MODEM";

void a7670_init(void)
{
    gpio_set_direction(12, GPIO_MODE_OUTPUT);
    gpio_set_direction(A7670_RESET, GPIO_MODE_OUTPUT);
    gpio_set_direction(A7670_PWKEY, GPIO_MODE_OUTPUT);
    vTaskDelay(pdMS_TO_TICKS(500));
    gpio_set_level(12, 1);
    a7670_turnoff();
    for (uint8_t i = 0; i < 5; i++)
    {
        ESP_LOGI("A7670","Unpowering modem... %d", i);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    a7670_turnon();
    for (uint8_t i = 0; i < 10; i++)
    {
        ESP_LOGI("A7670","Initializing... %d", i);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void a7670_turnon(void)
{   
    clear_a7670_rst();
    vTaskDelay(pdMS_TO_TICKS(100));
    set_a7670_rst();
    vTaskDelay(pdMS_TO_TICKS(2600));
    clear_a7670_rst();
    //power modem
    clear_a7670_pwkey();
    vTaskDelay(pdMS_TO_TICKS(100));
    set_a7670_pwkey();
    vTaskDelay(pdMS_TO_TICKS(1000));
    clear_a7670_pwkey();
    //----------------
    // set_sim7600_flight();
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void a7670_turnoff(void){
    set_a7670_pwkey();
}


uint8_t check_signal_quality(esp_modem_dce_t *dce)
{
    int rssi, ber;
    esp_err_t err = esp_modem_get_signal_quality(dce, &rssi, &ber);
    for (size_t i = 0; i < MAX_RETRY; i++)
    {
        if(err != ESP_OK)
        {
            ESP_LOGE(TAG, "esp_modem_get_signal_quality failed with %d %s", err, esp_err_to_name(err));
            system_logging(PATH_SYSTEM,"|ERROR| => esp_modem_get_signal_quality failed");
            err = esp_modem_get_signal_quality(dce, &rssi, &ber);
            vTaskDelay(pdMS_TO_TICKS(500));
        }else{
            ESP_LOGI(TAG, "Signal quality: rssi=%d, ber=%d", rssi, ber);
            if ((rssi > 4 && rssi <= 31) || rssi == 99)
            {
                return rssi;
            }
        }
        
    }
    return 0;
}

static void enable_gps(esp_modem_dce_t *dce)
{
    char at_data[16] = {0};
    memset(at_data, 0, sizeof(at_data));
    esp_modem_at(dce,"AT+CGDRT=2,1", at_data, portMAX_DELAY);
    ESP_LOGI(TAG, "setting pin mode %s", at_data);
    vTaskDelay(pdMS_TO_TICKS(500));

    memset(at_data, 0, sizeof(at_data));
    esp_modem_at(dce,"AT+CGSETV=2,1", at_data, portMAX_DELAY);
    ESP_LOGI(TAG, "setting pin mode %s", at_data);
    vTaskDelay(pdMS_TO_TICKS(500));
    
    esp_err_t res = esp_modem_set_gnss_power_mode(dce,1);
    for (size_t i = 0; i < 15; i++)
    {
        if (res == ESP_OK)
        {
            ESP_LOGI(TAG, "THE GNSS POWER IS ON");
            break;
        }else{
            ESP_LOGI(TAG, "CAN'T POWER ON THE GNSS");
        }
        res = esp_modem_set_gnss_power_mode(dce,1);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void disable_gps(esp_modem_dce_t *dce)
{
    char at_data[16] = {0};

    memset(at_data, 0, sizeof(at_data));
    esp_modem_at(dce,"AT+CGDRT=2,0", at_data, portMAX_DELAY);
    ESP_LOGI(TAG, "setting pin mode %s", at_data);
    vTaskDelay(pdMS_TO_TICKS(500));

    memset(at_data, 0, sizeof(at_data));
    esp_modem_at(dce,"AT+CGSETV=2,0", at_data, portMAX_DELAY);
    ESP_LOGI(TAG, "setting pin mode %s", at_data);
    vTaskDelay(pdMS_TO_TICKS(500));

    memset(at_data, 0, sizeof(at_data));
    esp_modem_at(dce,"AT+CGNSSPWR=0", at_data, portMAX_DELAY);
    ESP_LOGI(TAG, "powering gps %s", at_data);
}

char *strtok_fr (char *s, char delim, char **save_ptr)
{
    char *tail;
    char c;

    if (s == NULL) {
        s = *save_ptr;
    }
    tail = s;
    if ((c = *tail) == '\0') {
        s = NULL;
    }
    else {
        do {
            if (c == delim) {
                *tail++ = '\0';
                break;
           }
        }while ((c = *++tail) != '\0');
    }
    *save_ptr = tail;
    return s;
}

char *strtok_f (char *s, char delim)
{
    static char *save_ptr;

    return strtok_fr(s, delim, &save_ptr);
}

gps_t get_gnss_data(esp_modem_dce_t *dce)
{
    gps_t gnss;
    char at_data[128] = {0};
    // char data[] = "+CGNSSINFO: 3,13,,07,00,42.4344826,N,59.5115700,E,210624,065131.01,58.0,0.047,,2.47,1.70,1.80,06";
    enable_gps(dce);
    ESP_LOGI(TAG, "GPS ENABLED: %s", at_data);
    vTaskDelay(pdMS_TO_TICKS(1000));

    memset(at_data, 0, sizeof(at_data));
    esp_modem_at(dce,"AT+CGNSSIPR=115200", at_data, portMAX_DELAY);
    ESP_LOGI(TAG, "Setting baud rate to 115200: %s", at_data);

    memset(at_data, 0, sizeof(at_data));
    esp_modem_at(dce,"AT+CGPSHOT", at_data, portMAX_DELAY);
    ESP_LOGI(TAG, "Hot Start: %s", at_data);
    uint8_t gps_try_count = 0;
    for (size_t i = 0; i < 10; i++)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        memset(at_data, 0, sizeof(at_data));
        esp_modem_at(dce,"AT+CGNSSINFO", at_data, portMAX_DELAY);
        ESP_LOGI(TAG, "length of gps data: %d", strlen(at_data));
        ESP_LOGI(TAG, "CGNSSINFO %s", at_data);
        if (strlen(at_data) > 40)
        {
            ESP_LOGI(TAG, "Waiting gps to stabilize %d", gps_try_count);
            gps_try_count++;
            if (gps_try_count > 2)
            {
                char* mode = strtok_f(at_data,',');
                char* gps_svs = strtok_f(NULL,',');
                char* beidou_svs = strtok_f(NULL,',');
                char* glonass_svs = strtok_f(NULL,',');
                char* galileo_svs = strtok_f(NULL,',');
                char* Raw_Latitude = strtok_f(NULL,',');
                char* N_S = strtok_f(NULL,',');
                char* Raw_Longitude = strtok_f(NULL,',');
                char* E_W = strtok_f(NULL,',');
                char* Date = strtok_f(NULL,',');
                char* utc_time = strtok_f(NULL,',');
                char* alt = strtok_f(NULL,',');
                char* Speed = strtok_f(NULL,',');
                char* course = strtok_f(NULL,',');
                char* pdop = strtok_f(NULL,',');
                char* hdop = strtok_f(NULL,',');
                char* vdop = strtok_f(NULL,',');

                double Latitude = atof(Raw_Latitude);
                double Longitude = atof(Raw_Longitude);
                
                printf("mode : %s\n", mode);
                printf("gps_svs : %s\n", gps_svs);
                printf("beidou_svs : %s\n", beidou_svs);
                printf("glonass_svs : %s\n", glonass_svs);
                printf("galileo_svs : %s\n", galileo_svs);
                printf("Latitude : %f\n", Latitude);
                printf("N_S : %s\n", N_S);
                printf("Longitude : %f\n", Longitude);
                printf("E_W : %s\n", E_W);
                printf("Date : %s\n", Date);
                printf("utc_time : %s\n", utc_time);
                printf("alt : %s\n", alt);
                printf("Speed : %s\n", Speed);
                printf("course : %s\n", course);
                printf("pdop : %s\n", pdop);
                printf("hdop : %s\n", hdop); 
                printf("vdop : %s\n", vdop);

                sprintf(gnss.latitude, Raw_Latitude);
                sprintf(gnss.longitude, Raw_Longitude);
                break; 
            }
        }else{
            sprintf(gnss.latitude, "0");
            sprintf(gnss.longitude, "0"); 
        }
    }
    disable_gps(dce);
    return gnss;
}




gps_t get_NMEA_gnss_data(esp_modem_dce_t *dce)
{
    gps_t gnss = {
        .latitude = "NaN",
        .longitude = "NaN"
    };
    char at_data[16] = {0};
    enable_gps(dce);
    vTaskDelay(pdMS_TO_TICKS(15000));
    esp_modem_set_gnss_power_mode(dce, 1);
    memset(at_data, 0, sizeof(at_data));
    esp_modem_at(dce,"AT+CGNSSIPR=115200", at_data, portMAX_DELAY);
    ESP_LOGI(TAG, "Setting baud rate to 115200: %s", at_data);

    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_modem_at(dce,"AT+CGNSSMODE=3", at_data, portMAX_DELAY); //GPS + BD
    ESP_LOGI(TAG, "setting gps mode %s", at_data);

    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_modem_at(dce,"AT+CGNSSNMEA=1,1,1,1,1,1,0,0", at_data, portMAX_DELAY); 
    ESP_LOGI(TAG, "CGNSSNMEA %s", at_data);

    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_modem_at(dce,"AT+CGPSNMEARATE=1", at_data, portMAX_DELAY);
    ESP_LOGI(TAG, "CGPSNMEARATE %s", at_data);

    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_modem_at(dce,"AT+CGNSSTST=1", at_data, portMAX_DELAY);
    ESP_LOGI(TAG, "CGNSSTST %s", at_data);

    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_modem_at(dce,"AT+CGNSSPORTSWITCH=0,1", at_data, portMAX_DELAY);
    ESP_LOGI(TAG, "CGNSSPORTSWITCH %s", at_data);

    vTaskDelay(pdMS_TO_TICKS(1000));
    memset(at_data, 0, sizeof(at_data));
    esp_modem_at(dce,"AT+CGNSSINFO", at_data, portMAX_DELAY);
    ESP_LOGI(TAG, "CGNSSINFO %s", at_data);
    disable_gps(dce);
    return gnss;
}
