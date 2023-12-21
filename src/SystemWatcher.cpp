#include "SystemWatcher.h"

void task_startSystemWatcher(void *param)
{
    while (1)
    {
        try
        {
            if (!SystemWatcher::http_welfare_check())
            {
                std::cout << "HTTP WELFARE CHECK FAIL\n";
                if (!SystemWatcher::wifi_welfare_check())
                {
                    WiFi_Functions::de_init();
                    WiFi_Functions::init();
                }
                else 
                {
                    std::cout << "No Internet Connection currently or World Servers are down lol.\nGotta wait I guess.\n";
                    
                }
            }
            if (!SystemWatcher::http_welfare_check())
            {
                std::cout << "HTTP WELFARE CHECK FAIL\n";
                WiFi_Functions::de_init();
                WiFi_Functions::init();
            }
            if (!SystemWatcher::wifi_welfare_check())
            {
                std::cout << "HTTP WELFARE CHECK FAIL\n";
                WiFi_Functions::de_init();
                WiFi_Functions::init();
            }
            helper_functions::delay(120000);
        }
        catch (std::exception e)
        {
            std::cout << "EXCEPTION IN task_startSystemWatcher: " << e.what() << "\n";
            helper_functions::delay(300);
            esp_restart();
        }
    }
    vTaskSuspend(NULL);
}

bool SystemWatcher::http_welfare_check(int compared_miliseconds)
{
    auto start = SystemWatcher::last_http_update;
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    if (duration.count() > compared_miliseconds)
        return false;
    return true;
}

bool SystemWatcher::startSystemWatcher()
{
    xTaskCreatePinnedToCore(task_startSystemWatcher, "task_startSystemWatcher", 8 * 1024, NULL, 0, NULL, 0);
}
