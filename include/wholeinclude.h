    #pragma once
#include <stdio.h>
#include <iostream>
#include <inttypes.h>
#include <vector>
#include <algorithm>
#include <string.h>
#include <string>
// #include <optional>
// #include <functional>
#include <tuple>
#include <utility>
// #include <mutex>


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"

#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_wps.h>
#include "esp_event.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "sdkconfig.h"
#include "esp_wifi_types.h"
#include "lwip/ip_addr.h"
#include "lwip/tcpip.h"
#include "lwip/tcp.h"
#include "dhcpserver/dhcpserver.h"
// #include "esp_event_loop.h"

//Flash Memory
#include "nvs_flash.h"

//Display Library
#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
#include "lvgl/demos/lv_demos.h"



//WiFi and Networking
// #include "lwip/err.h"
// #include "lwip/sys.h"

// #include "driver/gpio.h"