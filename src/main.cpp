
extern "C"
{
    void app_main(void);
}

#include "wholeinclude.h"

#include "nvs_wrapper.h"
#include "WiFi_Functions.h"
#include "helper_functions.h"
#include "http_functions.h"
#include "secret.h"
#include "tft.h"
#include "SystemWatcher.h"
#include "weather.h"
#include <string.h>

// #include "lvgl_helpers.h"

// #define keythatneverexists "ineverdoexist"

SemaphoreHandle_t semaphore_handle;

using namespace helper_functions;

void task_connect_wifi(void *param)
{
    while (true)
    {
        semaphore_handle = xSemaphoreCreateBinary();
        if (WiFi_Functions::connect_to_nearest_ap())
            xSemaphoreGive(semaphore_handle);
        else
        {
            continue;
        }

        break;
    }
    vTaskDelete(NULL);
}

void task_init_all(void *param)
{
    while (true)
    {
        semaphore_handle = xSemaphoreCreateBinary();
        nvs_wrapper::init();
        WiFi_Functions::init();
        xSemaphoreGive(semaphore_handle);
        vTaskDelete(NULL);
    }
}

/*
    Setup:
        INIT ALL
        Check for very first Startup then we gotta take in user info about their location
        Start SystemWatcher

    Loop:
        Update Weather Object with new Data from Server
        Update Display
        (Handle User Input?)
*/
void app_main(void)
{
    delay(5000);

    xTaskCreate(task_init_all, "init_all", 1024 * 8, NULL, 2, NULL);
    if (xSemaphoreTake(semaphore_handle, portMAX_DELAY))
        xSemaphoreGive(semaphore_handle);

    delay(2000);

    Weather current_weather("1.1.2024","Monday");

    
    
}




















// xTaskCreate( tft_wrapper::print_pixel_mask, " tft_wrapper::init()", 1024 * 8, NULL, 2, NULL);
    // tft_wrapper::init();

    // if(!WiFi_Functions::AP_Credentials::loadAllCredentialsFromNVS())
    //     std::cout << "\n\nCouldnt load even 1 element in loadAllCredentialsFromNVS.\n";

    // std::cout << "\n\n\n";
    // delay(3200);

    // // WiFi_Functions::AP_Credentials::saveRouterConfiguration(wifi_ssid, wifi_password);
    // // WiFi_Functions::AP_Credentials::saveRouterConfiguration(wifi_ssid2, wifi_password2);
    // // WiFi_Functions::AP_Credentials::saveRouterConfiguration(wifi_ssid3, wifi_password3);

    // xTaskCreate(task_connect_wifi, "connect_wifi", 1024 * 8, NULL, 2, NULL);
    // if (xSemaphoreTake(semaphore_handle, portMAX_DELAY))
    //     xSemaphoreGive(semaphore_handle);

    // std::cout << "\n\ngot to get request\n\n";