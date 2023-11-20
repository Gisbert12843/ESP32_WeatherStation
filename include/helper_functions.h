#pragma once

#include "wholeinclude.h"

namespace helper_functions
{
    void delay(int delay_in_ms)
    {
        std::cout << "delaying for " << double(delay_in_ms/1000) << " seconds.\n ";
        vTaskDelay(delay_in_ms / portTICK_PERIOD_MS);
    }
}

