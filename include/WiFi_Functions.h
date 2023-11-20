#pragma once

#include "wholeinclude.h"
#include "nvs_wrapper.h"



namespace WiFi_Functions
{
    class AP_Credentials;

    inline EventGroupHandle_t s_wifi_event_group;

    // State of WiFi Init, changed when calling init() and de_init()
    inline bool init_state = false;

    // WPSEventHandlers
    inline esp_event_handler_instance_t instance_any_id_wps;

    // WiFiEventHandlers
    inline esp_event_handler_instance_t instance_any_id;
    inline esp_event_handler_instance_t instance_got_ip;

    inline esp_netif_t *sta_netif;

    inline esp_wps_config_t wps_config = WPS_CONFIG_INIT_DEFAULT(WPS_TYPE_PBC);
    inline wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    // Bits for following the status of the WiFI Connection to the current AP
    // Used by e.g. wifi_fail_bit_listener
    inline constexpr int8_t WIFI_CONNECTED_BIT = BIT0;
    inline constexpr int8_t WIFI_FAIL_BIT = BIT1;

    inline AP_Credentials *currentAPConnection = nullptr;

    class AP_Credentials
    {
    private:
        std::string wifi_ssid = "";
        std::string wifi_password = "";

        // Contains all stored AP Credentials at RunTime.
        inline static std::vector<AP_Credentials *> vec_AP_Credentials;

        void setWifi_ssid(std::string new_ssid)
        {
            wifi_ssid = new_ssid;
        }
        void setWifi_password(std::string new_password)

        {
            wifi_password = new_password;
        }
        int getVectorIndexOfConnection()
        {
            for (size_t index = 0; index < vec_AP_Credentials.size(); ++index)
            {
                if (vec_AP_Credentials[index]->wifi_ssid == wifi_ssid)
                    return index;
            }
            printf("RouterConnection was not found in vec_AP_Credentials.\n");
            return -1;
        }

    public:
        AP_Credentials(){};
        AP_Credentials(AP_Credentials &cred) : wifi_ssid(cred.getWifi_ssid()), wifi_password(cred.getWifi_password()){};
        AP_Credentials(std::string ssid, std::string password) : wifi_ssid(ssid), wifi_password(password){};

        virtual ~AP_Credentials()
        {
            vec_AP_Credentials.erase(vec_AP_Credentials.begin() + getVectorIndexOfConnection());
        }

        std::string getWifi_ssid()
        {
            return wifi_ssid;
        }
        std::string getWifi_password()
        {
            return wifi_password;
        }

        static const std::vector<AP_Credentials *> getVecAPCredentials()
        {
            return vec_AP_Credentials;
        }

        static AP_Credentials *getCredentialFromNVS(std::string wifi_ssid);
        static esp_err_t setCredentialToNVS(std::string wifi_ssid, AP_Credentials *cred);

        static bool loadAllCredentialsFromNVS(const std::string p_nvs_namespace_name = nvs_wrapper::nvs_namespace_name);

        static AP_Credentials *saveRouterConfiguration(std::string wifi_ssid, std::string wifi_password, bool save_to_nvs = true);
        static AP_Credentials *findSavedRouterConnection(const std::string wifi_ssid);

        static void listAllConnections()
        {
            int i = 0;
            for (auto it : vec_AP_Credentials)
            {
                printf("%i: SSID: %s Password: %s\n", ++i, it->getWifi_ssid().c_str(), it->getWifi_password().c_str());
            }
        }

        void printConnectionData()
        {
            std::cout << "SSID: \"" << this->getWifi_ssid().c_str() << "\" Password: \"" << this->getWifi_password().c_str() << "\"\n";
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void init();

    void de_init();

    void startWPS();

    bool connect_to_ap(AP_Credentials *connection);

    std::vector<std::string> find_available_networks();

    // returns 0 when not found, 1 if found and 2 on error
    int check_network_availability(std::string ssid);

    bool connect_to_nearest_ap();

    // Waits for the WiFi_FailBit to be set.
    // Then uses an infinity loop to scan if the previous network is available again.
    // If Errors persist during the scanning -> device restart
    // If previous network was not found in 5 Minutes try another stored network if it is available.
    // If that also fails return to the infinity loop else close the function
    void wifi_fail_bit_listener();

    /* void fix_wifi()
    {

    } */
};



namespace WiFi_Task
{
    inline void Task_Start_Fail_Bit_Listener(void *params)
    {
        WiFi_Functions::wifi_fail_bit_listener();
        std::cout << "\n\n deleting Task_Start_Fail_Bit_Listener\n\n";
        vTaskDelete(NULL);
    }
}