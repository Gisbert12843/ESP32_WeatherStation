#include "WiFi_Functions.h"

#define TEST_SSID "TEST_SSID"
#define TEST_PASSWORD "TEST_PASSWORD"

WiFi_Functions::AP_Credentials *WiFi_Functions::AP_Credentials::saveRouterConfiguration(std::string wifi_ssid, std::string wifi_password, bool save_to_nvs)
{
    for (auto connection : vec_AP_Credentials)
    {
        if (connection->getWifi_ssid() != "" && connection->getWifi_ssid() == wifi_ssid)
        {
            if (0 == strcmp(connection->getWifi_password().c_str(), wifi_password.c_str()))
            {
                std::cout << "Old password: \"" << connection->getWifi_password() << "\" is equal to new password: \"" << wifi_password << "\"\n";
                printf("Credentials are equal to the already stored ones, no opertation on saved credentials was performed.");
                return connection;
            }
            std::cout << "Old password: \"" << connection->getWifi_password() << "\" is not equal to new password: \"" << wifi_password << "\"\n";

            connection->setWifi_password(wifi_password);
            WiFi_Functions::AP_Credentials::setCredentialToNVS(connection->getWifi_ssid(), connection);
            printf("A Configuration was found that machtches this SSID. Credentials were updated.\n");
            return connection; // Found a match
        }
    }

    printf("No Configuration was found that machtches this SSID. Saving new Configuration...\n");

    WiFi_Functions::AP_Credentials *newConnection = new WiFi_Functions::AP_Credentials(wifi_ssid, wifi_password);
    vec_AP_Credentials.push_back(newConnection);
    WiFi_Functions::AP_Credentials::setCredentialToNVS(wifi_ssid, newConnection);

    // nvs_wrapper::setValueToKey(wifi_ssid, *newConnection);
    printf("The following Configuration has been saved:\n\tSSID:     \"%s\"\n\tPassword: \"%s\"\n", wifi_ssid.c_str(), wifi_password.c_str());

    return newConnection; // No matching element found
}

WiFi_Functions::AP_Credentials *WiFi_Functions::AP_Credentials::getCredentialFromNVS(std::string wifi_ssid)
{

    // // Get the size of the blob data for wifi_ssid
    // size_t blob_size = 0;
    // nvs_wrapper::getSizeFromKey(wifi_ssid, blob_size);

    // // Allocate a std::vector<char> with the appropriate size
    // std::vector<char> deserializedData(blob_size);

    // // Retrieve the blob data from NVS
    esp_err_t err = ESP_OK;

    std::string deserializedData = "";
    err = nvs_wrapper::getValueFromKey(wifi_ssid, deserializedData);

    std::cout << "Deserialized Data seems to be: " << std::string(deserializedData.data()) << "\n";

    std::string ssid = "";
    std::string password = "";

    if (deserializedData.size() > 0 && err == ESP_OK)
    {
        bool mode = 0;
        for (auto it : deserializedData)
        {
            switch (mode)
            {
            case 0:
            {
                if (it == '~')
                {
                    mode = 1;
                    break;
                }
                else
                {
                    ssid += it;
                }
            }
            break;
            case 1:
                password += it;
                break;
            default:
                break;
            }
        }
        std::cout << "Deserialised: SSID: \"" << ssid << "\" and password: \"" << password << "\"\n";
    }
    else
    {
        std::cout << "Error when reading from NVS: " << esp_err_to_name(err) << "\n";
        std::cout << "Vector content is for some reason:\n";
        for (auto it : deserializedData)
            std::cout << it;
        std::cout << "\n";
        return nullptr;
    }

    WiFi_Functions::AP_Credentials *returnptr = new WiFi_Functions::AP_Credentials(ssid, password);
    return returnptr;
}

esp_err_t WiFi_Functions::AP_Credentials::setCredentialToNVS(std::string wifi_ssid, WiFi_Functions::AP_Credentials *cred)
{
    std::string wifi_password = cred->getWifi_password();

    std::string serializedData;
    serializedData.insert(serializedData.end(), wifi_ssid.begin(), wifi_ssid.end());
    serializedData.push_back('~'); // Separator
    serializedData.insert(serializedData.end(), wifi_password.begin(), wifi_password.end());

    // Add a null terminator to the end
    serializedData.push_back('\0');

    std::cout << "\nSaving following string data to NVS: \"";
    for (auto it : serializedData)
        std::cout << it;
    std::cout << "\"\n\n";

    return nvs_wrapper::setValueToKey(wifi_ssid, serializedData);
}

static void wps_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{

    switch (event_id)
    {
    }
}

void wifi_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    constexpr char *TAG = "wifi station";
    static int8_t s_retry_num = 0;

    constexpr int8_t EXAMPLE_ESP_MAXIMUM_RETRY = 30;

    switch (event_id)
    {

    case (WIFI_EVENT_STA_START):
    {
        printf("WIFI_EVENT_STA_START\n");
        // ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_connect());
    }
    break;
    case WIFI_EVENT_STA_DISCONNECTED:
    {
        printf("WIFI_EVENT_STA_DISCONNECTED: Connection to AP failed\n");

        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_connect());
            s_retry_num++;
            printf("retrying to connect to AP\n");
            vTaskDelay(1500 / portTICK_PERIOD_MS);
        }
        else
        {
            // ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_scan_get_ap_records());
            ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_disconnect());
            s_retry_num = 0;
            xEventGroupSetBits(WiFi_Functions::s_wifi_event_group, WiFi_Functions::WIFI_FAIL_BIT);
        }
    }
    break;
    case IP_EVENT_STA_GOT_IP:
    {
        std::cout << "IP_EVENT_STA_GOT_IP\n";
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

        char local_ip[32];
        sprintf(local_ip, IPSTR, IP2STR(&event->ip_info.ip));
        char gateway_ip[32];
        sprintf(gateway_ip, IPSTR, IP2STR(&event->ip_info.gw));

        printf("Connected as : \"%s\" to \"%s\"\n", local_ip, gateway_ip);
        s_retry_num = 0;
        xEventGroupSetBits(WiFi_Functions::s_wifi_event_group, WiFi_Functions::WIFI_CONNECTED_BIT);
    }
    break;

    default:
        break;
    }
}

void WiFi_Functions::de_init()
{
    if (init_state)
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_instance_unregister(WIFI_EVENT,
                                                                            ESP_EVENT_ANY_ID,
                                                                            &WiFi_Functions::instance_any_id));
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_instance_unregister(IP_EVENT,
                                                                            IP_EVENT_STA_GOT_IP,
                                                                            &WiFi_Functions::instance_got_ip));

        esp_wifi_disconnect();
        esp_wifi_stop();

        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_deinit());

        // esp_netif_destroy_default_wifi(WiFi_Functions::sta_netif);

        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_loop_delete_default());
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_deinit());
        init_state = false;
    }
    else
        printf("Duplicate call of WiFi De-Init ignored.\n");
}

void WiFi_Functions::init()
{
    if (!init_state)
    {
        init_state = true;


        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_init());
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_loop_create_default());

        // creates the default WiFi station (STA) interface. This interface is used for connecting the ESP32 to a WiFi network as a client (station)
        // This line is needed!!
        sta_netif = esp_netif_create_default_wifi_sta();
        assert(sta_netif);

        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_init(&cfg));
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_set_storage(WIFI_STORAGE_RAM));
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_start());
        esp_netif_set_hostname(sta_netif, "Kais_Wetterstations");
    }
    else
    {
        printf("Duplicate call of WiFi Init ignored.\n");
    }
}

/**
 * Finds and returns the AP_Credentials* associated with the given ssid in the vec_AP_Credentials vector.
 *
 * @param wifi_ssid The WiFi SSID to search for in the vec_AP_Credentials vector.
 *
 * @return A pointer to the AP_Credentials object that matches the WiFi SSID. Returns nullptr if no match was found.
 *
 * @throws None.
 */
WiFi_Functions::AP_Credentials *WiFi_Functions::AP_Credentials::findSavedRouterConnection(std::string wifi_ssid)
{
    printf("Searching for \"%s\" in %i saved RouterConnections.\n", wifi_ssid.c_str(), vec_AP_Credentials.size());

    for (auto it : vec_AP_Credentials)
    {
        std::string ssid_in_vector = it->getWifi_ssid();

        // Check if ssid_in_vector is not "" and compare it to wifi_ssid
        if (ssid_in_vector != "" && strcmp(ssid_in_vector.c_str(), wifi_ssid.c_str()) == 0)
        {
            printf("Found\tSSID: \"%s\"\n\tPassword:\"%s\"\nin saved RouterConnections.\n", ssid_in_vector.c_str(), it->getWifi_password().c_str());
            return it; // Return the matching AP_Credentials object
        }
    }

    printf("No matching AP_Credentials were found.\n");
    return nullptr; // Return nullptr if no match was found
}

bool WiFi_Functions::AP_Credentials::loadAllCredentialsFromNVS(std::string p_nvs_namespace_name)
{

    esp_err_t err = nvs_open(nvs_wrapper::nvs_namespace_name, NVS_READWRITE, &nvs_wrapper::nvs_namespace_handle);
    if (err != ESP_OK)
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return false;
    }

    // nvs_stats_t nvs_stats;
    // nvs_get_stats(nvs_wrapper::nvs_namespace_name, &nvs_stats);
    // entry_count = nvs_stats.used_entries;

    nvs_iterator_t it = NULL;
    err = nvs_entry_find(NVS_DEFAULT_PART_NAME, nvs_wrapper::nvs_namespace_name, NVS_TYPE_STR, &it);
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);

    if (err != ESP_OK)
        return false;

    do
    {
        nvs_entry_info_t info;
        err = nvs_entry_info(it, &info);
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        if (err != ESP_OK)
            return false;

        std::string received_string = "";
        err = nvs_wrapper::getValueFromKey(std::string(info.key), received_string, true);
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);

        if (err == ESP_OK)
        {
            std::cout << "Read NVS Value: \"" << received_string << "\"\n";

            std::string ssid = "";
            std::string password = "";

            if (received_string.size() > 0)
            {
                bool mode = 0;
                for (auto it : received_string)
                {
                    switch (mode)
                    {
                    case 0:
                    {
                        if (it == '~')
                        {
                            mode = 1;
                            break;
                        }
                        else
                        {
                            ssid += it;
                        }
                    }
                    break;
                    case 1:
                        password += it;
                        break;
                    default:
                        break;
                    }
                }

                //edited code from saveRouterConfig() to remove additive NVS Saving when initially storing credentials at each startup
                for (auto connection : vec_AP_Credentials)
                {
                    if (connection->getWifi_ssid() != "" && connection->getWifi_ssid() == ssid)
                    {
                        if (0 == strcmp(connection->getWifi_password().c_str(), password.c_str()))
                        {
                            std::cout << "Old password: \"" << connection->getWifi_password() << "\" is equal to new password: \"" << password << "\"\n";
                            printf("Credentials are equal to the already stored ones, no opertation on saved credentials was performed.");
                            return connection;
                        }
                        std::cout << "Old password: \"" << connection->getWifi_password() << "\" is not equal to new password: \"" << password << "\"\n";

                        connection->setWifi_password(password);
                        WiFi_Functions::AP_Credentials::setCredentialToNVS(connection->getWifi_ssid(), connection);
                        printf("A Configuration was found that machtches this SSID. Credentials were updated.\n");
                        return connection; // Found a match
                    }
                }

                WiFi_Functions::AP_Credentials *newConnection = new WiFi_Functions::AP_Credentials(ssid, password);
                vec_AP_Credentials.push_back(newConnection);

                // nvs_wrapper::setValueToKey(wifi_ssid, *newConnection);
                printf("The following Configuration has been saved:\n\tSSID:     \"%s\"\n\tPassword: \"%s\"\n", ssid.c_str(), password.c_str());
            }
            else
            {
                std::cout << "\n\nError when reading from NVS, though getValueFromKey() suceeded: Reason: \"String.size <= 0\" Error: \"" << esp_err_to_name(err) << "\"\n\n";
                // err = nvs_entry_next(&it);
                // ESP_ERROR_CHECK_WITHOUT_ABORT(err);
            }
        }
        else
        {
            std::cout << "\n\nError when reading from NVS, though nvs_entry_next() suceeded: Reason: \"getValueFromKey != ESP_OK\" Error: \"" << esp_err_to_name(err) << "\"\n\n";
            // err = nvs_entry_next(&it);
            // ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        }
    } while (nvs_entry_next(&it) == ESP_OK);

    nvs_release_iterator(it);
    return true;
}

bool WiFi_Functions::connect_to_ap(AP_Credentials *connection)
{

    if (connection)
        printf("Started connection_to_ap on SSID: \"%s\"\nPassword: \"%s\"\n", connection->getWifi_ssid().c_str(), connection->getWifi_password().c_str());
    else
        return false;

    // Inits the ESP32 Event Group
    WiFi_Functions::s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_instance_register(WIFI_EVENT,
                                                                      ESP_EVENT_ANY_ID,
                                                                      &wifi_event_handler,
                                                                      NULL,
                                                                      &instance_any_id));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_instance_register(IP_EVENT,
                                                                      IP_EVENT_STA_GOT_IP,
                                                                      &wifi_event_handler,
                                                                      NULL,
                                                                      &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = {TEST_SSID},
            .password = {TEST_PASSWORD},
        },
    };

    // copying new ssid/password to wifi_config; wifi_config_t expecting an array really sucks ngl
    strncpy((char *)wifi_config.sta.ssid, (char *)connection->getWifi_ssid().c_str(), 32);
    strncpy((char *)wifi_config.sta.password, (char *)connection->getWifi_password().c_str(), 64);

    printf("Configured:\n\tSSID: \"%s\"\n\tPassword: \"%s\"\n", (char *)wifi_config.sta.ssid, (char *)wifi_config.sta.password);

    // ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_connect());

    printf("WiFi Connection started!\n");

    EventBits_t bits = xEventGroupWaitBits(WiFi_Functions::s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, 15000 / portTICK_PERIOD_MS);

    std::cout << "\ngot out of WAITBITS\n";

    if (bits & WIFI_CONNECTED_BIT)
    {
        printf("Connected to AP using the credentials:\n\t%s\n\t%s\n", connection->getWifi_ssid().c_str(), connection->getWifi_password().c_str());
        WiFi_Functions::currentAPConnection = connection;

        // wifi_fail_bit_listener();
        xTaskCreate(WiFi_Task::Task_Start_Fail_Bit_Listener, "Task_Start_Fail_Bit_Listener", 8 * 1024, NULL, 2, NULL);
        return true;
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        printf("Failed to connect to AP using the credentials:\n\t%s\n\t%s\n", connection->getWifi_ssid().c_str(), connection->getWifi_password().c_str());
    }
    else
    {
        printf("UNEXPECTED ERROR while establishing initial AP Connection.");
    }

    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &instance_got_ip));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &instance_any_id));

    vEventGroupDelete(WiFi_Functions::s_wifi_event_group);

    printf("EvenGroup got deleted!!!\n");

    return false;
}

void WiFi_Functions::startWPS()
{
    constexpr char *TAG = "WPS_LOGIC:";

    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wps_event_handler, NULL, &WiFi_Functions::instance_any_id_wps));

    ESP_LOGI(TAG, "start wps...");
    esp_wps_config_t wps_config = WPS_CONFIG_INIT_DEFAULT(WPS_TYPE_PBC);

    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_wps_enable(&WiFi_Functions::wps_config));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_wps_start(0));
}

std::vector<std::string> WiFi_Functions::find_available_networks()
{
    constexpr char *TAG = "wifi_scan";
    esp_err_t err;
    std::vector<std::string> found_networks_ssids{};

    // err = esp_wifi_set_mode(WIFI_MODE_STA);
    // ESP_ERROR_CHECK_WITHOUT_ABORT(err);
    // if (err != ESP_OK)
    // {
    //     ESP_LOGE(TAG, "Error setting Wi-Fi mode: %s", esp_err_to_name(err));
    //     return found_networks_ssids;
    // }

    // ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_stop());

    // if (err != ESP_OK)
    // {
    //     ESP_LOGE(TAG, "Error starting Wi-Fi: %s", esp_err_to_name(err));
    //     return found_networks_ssids;
    // }

    wifi_scan_config_t scan_config = {
        .ssid = 0,
        .bssid = 0,
        .channel = 0,
        .show_hidden = true,
        .scan_time = 1000};

    err = esp_wifi_scan_start(&scan_config, true);
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_scan_stop());

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error starting Wi-Fi scan: %s", esp_err_to_name(err));
        std::vector<wifi_ap_record_t> wifi_records(30);
        uint16_t max_records = 30;
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_scan_get_ap_records(&max_records, wifi_records.data()));
        /*         ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_stop()); */
        return found_networks_ssids;
    }

    // wifi_ap_record_t wifi_records[20];
    std::vector<wifi_ap_record_t> wifi_records(30);

    uint16_t max_records = 20;
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_scan_get_ap_records(&max_records, wifi_records.data()));

    for (int i = 0; i < max_records; i++)
    {
        for (auto it : WiFi_Functions::AP_Credentials::getVecAPCredentials())
        {
            if (it->getWifi_ssid() == (char *)wifi_records[i].ssid)
            {
                printf("Found present SSID: %s\n", (char *)(wifi_records[i].ssid));
                found_networks_ssids.push_back((char *)(wifi_records[i].ssid));
            }
        }
    }
    // ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_stop());
    std::cout << "\n\n\nExited find available!\n\n\n";
    return found_networks_ssids;
}

bool WiFi_Functions::connect_to_nearest_ap()
{
    constexpr char *TAG = "wifi_scan";
    esp_err_t err;
    std::tuple<std::string, int8_t> best_ap = std::make_tuple("default", -128);

    /*     esp_err_t err = esp_wifi_set_mode(WIFI_MODE_STA);
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Error setting Wi-Fi mode: %s", esp_err_to_name(err));
            return false;
        } */

    /* err = esp_wifi_start();
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error starting Wi-Fi: %s", esp_err_to_name(err));
        return false;
    } */

    wifi_scan_config_t scan_config = {
        .ssid = 0,
        .bssid = 0,
        .channel = 0,
        .show_hidden = true};

    err = esp_wifi_scan_start(&scan_config, true);
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);
    esp_wifi_scan_stop();

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error starting Wi-Fi scan: %s", esp_err_to_name(err));
        // esp_wifi_stop();
        return false;
    }

    std::vector<wifi_ap_record_t> wifi_records(20);

    uint16_t max_records = 20;
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_scan_get_ap_records(&max_records, wifi_records.data()));

    for (int i = 0; i < max_records; i++)
    {
        for (auto it : WiFi_Functions::AP_Credentials::getVecAPCredentials())
        {
            if (it->getWifi_ssid() == (char *)wifi_records[i].ssid)
            {
                printf("Found present SSID: %s with RSSI %i\n", (char *)(wifi_records[i].ssid), (wifi_records[i].rssi));
                if (get<1>(best_ap) < wifi_records[i].rssi)
                {
                    std::cout << "Better Connection with " << (char *)(wifi_records[i].ssid) << " instead of " << get<0>(best_ap) << " was found.\n";
                    get<0>(best_ap) = (char *)wifi_records[i].ssid;
                    get<1>(best_ap) = wifi_records[i].rssi;
                }
                else
                    std::cout << std::to_string(get<1>(best_ap)) << " was bigger than " << std::to_string(wifi_records[i].rssi) << "\n";
            }
        }
    }
    // esp_wifi_stop();

    std::cout << "Best Connection was: " << get<0>(best_ap) << ". Connecting...\n";

    return WiFi_Functions::connect_to_ap(WiFi_Functions::AP_Credentials::findSavedRouterConnection(get<0>(best_ap)));
}

int WiFi_Functions::check_network_availability(std::string ssid)
{
    constexpr char *TAG = "wifi_scan";

    esp_err_t err = ESP_OK;

    // ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_disconnect());

    wifi_scan_config_t scan_config = {
        .ssid = 0,
        .bssid = 0,
        .channel = 0,
        .show_hidden = true,
        .scan_time = 1000};
    std::cout << "available: "
              << "1\n";

    err = esp_wifi_scan_start(&scan_config, true);
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);

    std::cout << "available: "
              << "2\n";

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error starting Wi-Fi scan: %s", esp_err_to_name(err));
        // esp_wifi_stop();
        return 2;
    }
    std::cout << "available: "
              << "3\n";

    uint16_t ap_num = 0;
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_scan_get_ap_num(&ap_num));

    std::vector<wifi_ap_record_t> wifi_records(ap_num);

    std::cout << "available: "
              << "4\n";

    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_scan_get_ap_records(&ap_num, wifi_records.data()));

    std::cout << "available: "
              << "5\n";
    for (int i = 0; i < ap_num; i++)
    {
        std::cout << (char *)wifi_records[i].ssid << " was found.\n\n";

        if (ssid == (char *)wifi_records[i].ssid)
        {
            std::cout << "\n\n"
                      << ssid << " was found!!\n\n";
            return 1;
        }
    }

    std::cout << "\n\n"
              << ssid << " was not found.\n\n";

    return 0;
}

void WiFi_Functions::wifi_fail_bit_listener()
{
    printf("\nWifi_fail_bit_listener started.\n");

    xEventGroupWaitBits(WiFi_Functions::s_wifi_event_group, WIFI_FAIL_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

    while (1)
    {
        short tries = 0;
        short error_count = 0;

        printf("\n\nWIFI_FAIL_BIT was set. Reestablishing WIFI Connection.\n\n");

        // 0 not available, 1 available, 2 Error
        short availability = 2;
        while (tries < 10)
        {
            tries++;
            std::cout << "\n\n\ntry number:<< " << tries << "\n\n\n";
            std::cout << "1\n";
            availability = WiFi_Functions::check_network_availability(WiFi_Functions::currentAPConnection->getWifi_ssid());

            if (availability == 2)
            {
                if (error_count >= 20)
                {
                    std::cout << "Unexpected Error keeps appearing. Solution: Restarting Device in 5 seconds...";
                    vTaskDelay(5000 / portTICK_PERIOD_MS);
                    esp_restart();
                }
                std::cout << "Unexpected Error while testing for availability of " << WiFi_Functions::currentAPConnection->getWifi_ssid() << "\nNext try in 5 secs.\n";

                vTaskDelay(5000 / portTICK_PERIOD_MS);
                error_count++;
                tries--;
                continue;
            }
            else if (availability == 0)
            {
                std::cout << "4\n";

                std::cout << WiFi_Functions::currentAPConnection->getWifi_ssid() << " is currently not available.\nNext try in 30 secs.\n";

                vTaskDelay(30000 / portTICK_PERIOD_MS);
                continue;
            }
            else
            {
                if (esp_wifi_connect() != ESP_OK)
                {
                    std::cout << WiFi_Functions::currentAPConnection->getWifi_ssid() << " did not accept connection request.\nNext try in 30 secs.\n";
                    vTaskDelay(30000 / portTICK_PERIOD_MS);
                }
                else
                {
                    xEventGroupClearBits(WiFi_Functions::s_wifi_event_group, WiFi_Functions::WIFI_FAIL_BIT);
                    xTaskCreate(WiFi_Task::Task_Start_Fail_Bit_Listener, "Task_Start_Fail_Bit_Listener", 4 * 1024, NULL, 4, NULL);
                    return;
                }
            }
        }
        std::cout << "Previous AP not available after 10 attempts.\nSearching for a different AP.";

        std::vector<std::string> available_networks = WiFi_Functions::find_available_networks();
        for (auto it : available_networks)
        {
            if (WiFi_Functions::connect_to_ap(WiFi_Functions::AP_Credentials::findSavedRouterConnection(it)))
            {
                std::cout << "Alternative Connection to " << WiFi_Functions::AP_Credentials::findSavedRouterConnection(it)->getWifi_ssid() << " was successfull.\n";
                return;
            }
        }
    }
}
