#pragma once
#include "wholeinclude.h"

inline std::string location;

class Weather
{

    std::string date, current_weekday;
    std::vector<std::tuple<short, double>> rain_risk_per_hour;
    std::vector<std::tuple<short, double>> temperature_per_hour;
    std::vector<std::tuple<short, double>> sunshine_per_hour;

public:
    Weather(std::string pDate, std::string pCurrent_weekday) : date{pDate}, current_weekday{pCurrent_weekday} {
    };
};