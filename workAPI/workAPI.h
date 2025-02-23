#pragma once 

#include <cpr/cpr.h>
#include <iostream>
#include "cpr/curl_container.h"
#include "nlohmann/json_fwd.hpp"
#include "user/User.h"
#include <nlohmann/json.hpp>
#include "nlohmann/detail/exceptions.hpp"
#include <cpr/parameters.h>
#include <exception>


class WorkWithAPI {
    public:
        bool SetDepartureCityCode(User& user, const std::string& name);
        bool SetArrivalCityCode(User& user, const std::string& name);
        bool SetKey(User& user, const std::string& key);
        bool SetDate(User& user, const std::string& date);

        std::string TransportTypeTranslation(const std::string& type);

        std::string FindCityCode(const User& user, const std::string& city_name);
        void UpdateCityCode(const User& user);

        void UpdateRoutesCache(const User& user);
        void FindRoutes(const User& user);
        void LoadRoutesToCurrentSession(const User& user);

        void ParseRoutes(nlohmann::json& data, const nlohmann::json& json_file);

        void DeleteOldFiles();
        void PrintJsonElem(const nlohmann::basic_json<>& elem);

    private:
        struct Parametrs {
            const std::string kFormat = "json";
            const std::string kLang = "ru_RU";
            const std::string kLat = "50.0";
            const std::string kLng = "40.0";
            const std::string kDistance = "50";
            const std::string kTransfers = "true";
            const std::string kPage = "1";
        };

        bool city_codes_loaded_ = false;
        Parametrs parameters_;

        const std::string kPath_ = "../../RoutesData/";
        const int kCorrectRequestCode_ = 200;

        nlohmann::json current_session_json_;
        nlohmann::json current_session_city_code_;
};