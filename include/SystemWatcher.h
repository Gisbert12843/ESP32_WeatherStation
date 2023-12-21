#pragma once
#include "wholeinclude.h"
#include "WiFi_Functions.h"

// #include "nvs_wrapper.h"
// #include "WiFi_Functions.h"
// #include "helper_functions.h"
// #include "http_functions.h"
// #include "secret.h"
#include <chrono>

#include "helper_functions.h"

inline constexpr int8_t SYSTEM_FAIL_BIT = BIT0;


namespace SystemWatcher
{
    /** SystemWatcher gotta check for:
     *   HTTP Receiving Event
     *   WIFI Connection Issues
     *   Display Updates are working
     *   Tasks are running
     */

    inline std::chrono::high_resolution_clock::time_point last_http_update;

    bool startSystemWatcher();

    bool http_welfare_check(int compared_miliseconds = 600000);
    bool wifi_welfare_check();
    bool tft_welfare_check();
}