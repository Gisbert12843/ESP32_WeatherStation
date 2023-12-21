#pragma once
#include "wholeinclude.h"
#include <array>

#define X_Resolution 480
#define Y_Resolution 320

namespace tft_wrapper
{
    // std::array<std::byte, X_Resolution> pixel_mask_x;
    std::vector<std::vector<char>> pixel_mask(Y_Resolution, std::vector<char>(X_Resolution,'1'));

    inline void init(void* param)
    {
        
    }

    void print_pixel_mask(void*param)
    {
        std::string temp = "";
        std::cout << pixel_mask[0][0];
        std::cout << "\n\n";

        for (int i = 0; i < pixel_mask.size(); i++)
        {
            for (auto it : pixel_mask[i])
            {
                temp += it;
            }
            std::cout << temp << "\n";
            temp = "";
            vTaskDelay(2);
        }
        // std::cout << pixel_mask.size() << "\n";
        // std::cout << pixel_mask[0].size() << "\n";
        vTaskDelete(NULL);
    }

}