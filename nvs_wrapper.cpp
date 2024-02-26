#include <nvs_wrapper.h>

// TODO remove automatic NVS flushing!
esp_err_t nvs_wrapper::init()
{
    static bool ran = false;
    esp_err_t ret = nvs_flash_init();
    // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    // {
    //     ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_flash_erase());
    //     ret = nvs_flash_init();
    //     printf("ERASED NVS!!");
    // }    
    ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
    return ret;
}

// This will delete all currently saved NVS Data
esp_err_t nvs_wrapper::resetNVS()
{
    esp_err_t error_code;
    error_code = nvs_flash_erase();
    if (error_code != ESP_OK)
        return error_code;
    error_code = nvs_flash_init();
    if (error_code != ESP_OK)
        return error_code;

    size_t entry_count = 0;

    nvs_handle_t x;
    error_code = nvs_get_used_entry_count(x, &entry_count);

    std::cout << "\n\n!!!Resetted NVS Storage to " << entry_count << " entries\n\n";
    return error_code;
}

bool nvs_wrapper::checkKeyExistence(std::string key_name)
{
    nvs_handle_t nvs_namespace_handle;
    esp_err_t err = nvs_open(nvs_namespace_name, NVS_READWRITE, &nvs_namespace_handle);
    if (err != ESP_OK)
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return false;
    }

    int mode = 0;
    do
    {
        switch (mode)
        {
        case 0:
        {
            size_t required_size = 0;
            err = nvs_get_blob(nvs_namespace_handle, Crypto_Functions::hash_to_length(key_name).c_str(), NULL, &required_size);
        }
        break;
        case 1:
        {
            int32_t *temp = new int32_t;
            err = nvs_get_i32(nvs_namespace_handle, Crypto_Functions::hash_to_length(key_name).c_str(), temp);
            delete (temp);
        }
        break;
        case 2:
        {
            size_t required_size = 0;
            err = nvs_get_str(nvs_namespace_handle, Crypto_Functions::hash_to_length(key_name).c_str(), NULL, &required_size);
        }
        break;

        default:
            break;
        }
        mode++;

    } while (err != ESP_OK && mode < 3);

    if (err != ESP_OK)
    {
        nvs_close(nvs_namespace_handle);
        return false;
    }
    else
    {
        nvs_close(nvs_namespace_handle);
        return true;
    }
}

esp_err_t nvs_wrapper::getValueFromKey(std::string key_name, int32_t &retrieved_value, bool use_prehashed_key)
{
    nvs_handle_t nvs_namespace_handle;

    esp_err_t err = nvs_open(nvs_namespace_name, NVS_READWRITE, &nvs_namespace_handle);
    if (err != ESP_OK)
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }
    if (use_prehashed_key)
        err = nvs_get_i32(nvs_namespace_handle, key_name.c_str(), &retrieved_value);
    else
        err = nvs_get_i32(nvs_namespace_handle, Crypto_Functions::hash_to_length(key_name).c_str(), &retrieved_value);

    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        nvs_close(nvs_namespace_handle);
        return err;
    }
}

// opens namespace_handle
// nvs_get_str
//

esp_err_t nvs_wrapper::getValueFromKey(std::string key_name, std::string &retrieved_value, bool use_prehashed_key)
{
    nvs_handle_t nvs_namespace_handle;
    esp_err_t err = nvs_open(nvs_namespace_name, NVS_READWRITE, &nvs_namespace_handle);
    if (err != ESP_OK)
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }
    size_t required_size;

    if (use_prehashed_key)
        err = nvs_get_str(nvs_namespace_handle, (key_name).c_str(), NULL, &required_size);
    else
        err = nvs_get_str(nvs_namespace_handle, Crypto_Functions::hash_to_length(key_name).c_str(), NULL, &required_size);

    if (err != ESP_OK)
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        nvs_close(nvs_namespace_handle);
        return err;
    }

    retrieved_value.resize(required_size + 1);

    // retrieved_value = (char *)malloc(required_size);
    // char buffer[required_size + 1];

    if (use_prehashed_key)
        err = nvs_get_str(nvs_namespace_handle, (key_name).c_str(), retrieved_value.data(), &required_size);
    else
        err = nvs_get_str(nvs_namespace_handle, Crypto_Functions::hash_to_length(key_name).c_str(), retrieved_value.data(), &required_size);

    ESP_ERROR_CHECK_WITHOUT_ABORT(err);
    nvs_close(nvs_namespace_handle);
    return err;
}

esp_err_t nvs_wrapper::getValueFromKey(std::string key_name, void *retrieved_value, size_t &data_length, bool use_prehashed_key)
{
    nvs_handle_t nvs_namespace_handle;
    esp_err_t err = nvs_open(nvs_namespace_name, NVS_READWRITE, &nvs_namespace_handle);
    if (err != ESP_OK)
    {
        printf("Couldn't open namespace\n");
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }

    size_t required_size;

    if (use_prehashed_key)
        err = nvs_get_blob(nvs_namespace_handle, (key_name).c_str(), NULL, &required_size);
    else
        err = nvs_get_blob(nvs_namespace_handle, Crypto_Functions::hash_to_length(key_name).c_str(), NULL, &required_size);

    if (err != ESP_OK)
    {
        printf("failed get blob size\n");

        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        nvs_close(nvs_namespace_handle);
        return err;
    }

    if (data_length > 0 && data_length != required_size)
    {
        std::cout << "Error: Provided buffer is not ready for data." << std::endl;
        nvs_close(nvs_namespace_handle);
        return err;
    }
    else if (data_length == 0)
    {
        retrieved_value = malloc(required_size);
        data_length = required_size;

        if (use_prehashed_key)
            err = nvs_get_blob(nvs_namespace_handle, (key_name).c_str(), retrieved_value, &required_size);
        else
            err = nvs_get_blob(nvs_namespace_handle, Crypto_Functions::hash_to_length(key_name).c_str(), retrieved_value, &required_size);

        if (err != ESP_OK)
        {
            printf("failed to get blob data\n");
            ESP_ERROR_CHECK_WITHOUT_ABORT(err);
            // free(retrieved_value);
            nvs_close(nvs_namespace_handle);
            return err;
        }
        else
        {
            nvs_close(nvs_namespace_handle);
            return err;
        }
    }
    else
    {
        err = nvs_get_blob(nvs_namespace_handle, Crypto_Functions::hash_to_length(key_name).c_str(), retrieved_value, &data_length);

        if (err != ESP_OK)
        {
            std::cout << "Error retrieving blob data: " << esp_err_to_name(err) << std::endl;
            nvs_close(nvs_namespace_handle);
            return err;
        }
        else
        {
            std::cout << "Data got received from NVS\n";
        }

        // Close the NVS namespace
        nvs_close(nvs_namespace_handle);
    }
    return err;
}

esp_err_t nvs_wrapper::getSizeFromKey(std::string key_name, size_t &data_length, bool use_prehashed_key)
{
    nvs_handle_t nvs_namespace_handle;
    esp_err_t err = nvs_open(nvs_namespace_name, NVS_READWRITE, &nvs_namespace_handle);
    if (err != ESP_OK)
    {
        printf("Couldn't open namespace\n");
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }
    if (use_prehashed_key)
        err = nvs_get_blob(nvs_namespace_handle, (key_name).c_str(), NULL, &data_length);
    else
        err = nvs_get_blob(nvs_namespace_handle, Crypto_Functions::hash_to_length(key_name).c_str(), NULL, &data_length);

    if (err != ESP_OK)
    {
        printf("failed get blob size\n");

        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        nvs_close(nvs_namespace_handle);
        return err;
    }
    else
    {
        std::cout << "Blob Size: " << data_length << "\n";
    }
    return err;
}

esp_err_t nvs_wrapper::setValueToKey(std::string key_name, int32_t value)
{
    nvs_handle_t nvs_namespace_handle;
    esp_err_t err = nvs_open(nvs_namespace_name, NVS_READWRITE, &nvs_namespace_handle);
    if (err != ESP_OK)
    {
        printf("Couldn't open namespace\n");
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }
    err = nvs_set_i32(nvs_namespace_handle, Crypto_Functions::hash_to_length(key_name).c_str(), value);
    if (err != ESP_OK)
    {
        printf("Couldn't set data\n");
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        nvs_close(nvs_namespace_handle);
        return err;
    }
    else
        printf("Set int32_t value: \"%i\" of Key: %s\n", int(value), key_name.c_str());

    err = nvs_commit(nvs_namespace_handle);
    {
        printf("Couldn't commit data\n");

        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        nvs_close(nvs_namespace_handle);
        return err;
    }
}

esp_err_t nvs_wrapper::setValueToKey(std::string key_name, std::string value)
{
    nvs_handle_t nvs_namespace_handle;
    esp_err_t err = nvs_open(nvs_namespace_name, NVS_READWRITE, &nvs_namespace_handle);
    if (err != ESP_OK)
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }

    err = nvs_set_str(nvs_namespace_handle, Crypto_Functions::hash_to_length(key_name).c_str(), value.c_str());
    if (err != ESP_OK)
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        nvs_close(nvs_namespace_handle);
        return err;
    }
    else
        printf("Set string \"%s\" of Key: %s\n", value.c_str(), key_name.c_str());

    err = nvs_commit(nvs_namespace_handle);
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        nvs_close(nvs_namespace_handle);
        return err;
    }
}

// return 0 aka ESP_OK when successfull, otherwise the corresponding esp_err_t
esp_err_t nvs_wrapper::setValueToKey(std::string key_name, void *value, size_t blob_size)
{
    nvs_handle_t nvs_namespace_handle;
    esp_err_t err = nvs_open(nvs_namespace_name, NVS_READWRITE, &nvs_namespace_handle);
    if (err != ESP_OK)
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }

    err = nvs_set_blob(nvs_namespace_handle, Crypto_Functions::hash_to_length(key_name).c_str(), value, blob_size);

    if (err != ESP_OK)
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        nvs_close(nvs_namespace_handle);
        return err;
    }
    else
        printf("Set Blob of Key: %s\n", key_name.c_str());

    err = nvs_commit(nvs_namespace_handle);
    if (err != ESP_OK)
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        nvs_close(nvs_namespace_handle);
        return err;
    }
    nvs_close(nvs_namespace_handle);
    return err;
}
