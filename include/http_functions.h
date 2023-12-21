#pragma once
#include "wholeinclude.h"
#include "esp_http_client.h"

constexpr int8_t HTTP_FAIL_BIT = BIT3;


namespace http_functions
{
    bool get_request(std::string received_data = "", std::string url = "example.com/data");
}