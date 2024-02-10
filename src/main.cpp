
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

// #define LV_TICK_PERIOD_MS 1
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
void app_main()
{
    lv_init();

    /* Initialize your hardware display and input drivers here */
    /* This includes setting up SPI, GPIO, or any other hardware interfaces */
    ili9486_init();
    static lv_disp_draw_buf_t disp_buf;
    static lv_color_t buf[480 * 10];  // Adjust the size according to your display
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, 480 * 10); // Initialize the display buffer

    lv_disp_drv_t disp_drv;           // Create a display driver descriptor
    lv_disp_drv_init(&disp_drv);      // Initialize the descriptor
    disp_drv.draw_buf = &disp_buf;    // Assign the display buffer
    disp_drv.flush_cb = ili9486_flush; // Set your display's flush callback function
    disp_drv.hor_res = 480;           // Set the display's horizontal resolution
    disp_drv.ver_res = 320;           // Set the display's vertical resolution
    lv_disp_drv_register(&disp_drv);  // Register the driver
}





















// SemaphoreHandle_t semaphore_handle;

// using namespace helper_functions;

// void task_connect_wifi(void *param)
// {
//     while (true)
//     {
//         semaphore_handle = xSemaphoreCreateBinary();
//         if (WiFi_Functions::connect_to_nearest_ap())
//             xSemaphoreGive(semaphore_handle);
//         else
//         {
//             continue;
//         }

//         break;
//     }
//     vTaskDelete(NULL);
// }

// void task_init_all(void *param)
// {
//     while (true)
//     {
//         semaphore_handle = xSemaphoreCreateBinary();
//         nvs_wrapper::init();
//         WiFi_Functions::init();
//         xSemaphoreGive(semaphore_handle);
//         vTaskDelete(NULL);
//     }
// }