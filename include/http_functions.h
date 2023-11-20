#pragma once
#include "wholeinclude.h"
#include "esp_http_client.h"

namespace http_functions
{

    bool get_request(std::string received_data = "", std::string url = "example.com/data");

}