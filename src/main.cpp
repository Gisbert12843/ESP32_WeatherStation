extern "C"
{
    void app_main(void);
}
#include "wholeinclude.h"
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
#include "nvs_wrapper.h"
#include "WiFi_Functions.h"
#include "helper_functions.h"

#include "display_driver.h"
#include "ui.h"

void app_main()
{
    helper_functions::delay(4000);
    std::cout << uxTaskGetStackHighWaterMark2(NULL) << "\n";
    std::cout << esp_get_free_heap_size() << "\n";

    nvs_wrapper::init();
    std::cout << uxTaskGetStackHighWaterMark2(NULL) << "\n";
    std::cout << esp_get_free_heap_size() << "\n";
    helper_functions::delay(1000);

    WiFi_Functions::init();
    std::cout << uxTaskGetStackHighWaterMark2(NULL) << "\n";
    std::cout << esp_get_free_heap_size() << "\n";
    helper_functions::delay(2000);



    initialize_spi();
    std::cout << uxTaskGetStackHighWaterMark2(NULL) << "\n";

    std::cout << esp_get_free_heap_size() << "\n";

    helper_functions::delay(1000);

    initialize_display();
    helper_functions::delay(2000);
    std::cout << uxTaskGetStackHighWaterMark2(NULL) << "\n";

    std::cout << esp_get_free_heap_size() << "\n";

    initialize_lvgl();
    std::cout << uxTaskGetStackHighWaterMark2(NULL) << "\n";

    std::cout << esp_get_free_heap_size() << "\n";

    helper_functions::delay(3000);

    ui::main_menu();
    std::cout << uxTaskGetStackHighWaterMark2(NULL) << "\n";

    std::cout << esp_get_free_heap_size() << "\n";
    helper_functions::delay(4000);

    xTaskCreate(ui::tick_task, "tick_task", 10 * 1024, NULL, 8, NULL);


    helper_functions::delay(10000);

    WiFi_Functions::AP_Credentials *my_ap = new WiFi_Functions::AP_Credentials("Astra WG Rakete 2.4 GHz", "!IchBinEinRiesigerWichser!!?123987");
    WiFi_Functions::connect_to_ap(my_ap);
    std::cout << uxTaskGetStackHighWaterMark2(NULL) << "\n";
    std::cout << esp_get_free_heap_size() << "\n";
    helper_functions::delay(2000);


}
