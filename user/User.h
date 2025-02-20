#pragma once

#include <iostream>
#include <string>
#include <ctime> 


std::string CurrentTime();


class User {
    public:
        void SetDepartureCity(const std::string& code);
        void SetArrivalCity(const std::string& code);
        void SetDate(const std::string& date);
        void SetKey(const std::string& key);
        void SwapCities();

        std::string GetDepartureCity() const;
        std::string GetArrivalCity() const;
        std::string GetDate() const;
        std::string GetKey() const;

    private:
        const std::string kDepartureCityCode = "c2";
        const std::string kArrivalCityCode = "c39";
        
        std::string date_ = CurrentTime();
        std::string key_ = "";
        std::string dep_city_code_ = kDepartureCityCode;
        std::string arr_city_code_ = kArrivalCityCode;

};