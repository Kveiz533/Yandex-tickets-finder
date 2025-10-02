#include "workAPI.h"


std::string WorkWithAPI::TransportTypeTranslation(const std::string& type) {
    if (type == "train") {
        return "поезд";
    } else if (type == "bus") {
        return "автобус";
    } else if (type == "plane") {
        return "самолет";
    } else if (type == "suburban") {
        return "электричка";
    }
    return type;
}

bool WorkWithAPI::SetArrivalCityCode(User& user, const std::string& name) {
    std::string res = FindCityCode(user, name);

    if (res != "" && user.GetDepartureCity() != res) {
        user.SetArrivalCity(res);
        return true;
    }
    return false;
}

bool WorkWithAPI::SetDepartureCityCode(User& user, const std::string& name) {
    std::string res = FindCityCode(user, name);

    if (res != "" && res != user.GetArrivalCity()) {
        user.SetDepartureCity(res);
        return true;
    }
    return false;
}

bool WorkWithAPI::SetKey(User& user, const std::string& key) {
    try {
        cpr::Response response = cpr::Get(cpr::Url{"https://api.rasp.yandex.net/v3.0/nearest_settlement/"},
         cpr::Parameters{{"apikey", key},
        {"format", parameters_.kFormat},
        {"lang", parameters_.kLang}, 
        {"lat", parameters_.kLat},
        {"lng", parameters_.kLng},
        {"distance", parameters_.kDistance}});

        if (response.status_code == kCorrectRequestCode_) {
            user.SetKey(key);
            return true;
        } 
    } catch (const nlohmann::detail::parse_error& error) {
        std::cerr << "Произошла ошибка парсинга, попробуйте еще раз" << std::endl;
    }
    return false;

}

bool WorkWithAPI::IsDigit(std::string& line) {
    for (auto elem: line) {
        if (!std::isdigit(elem)) {
            return false;
        }
    }
    return true;
 }

bool WorkWithAPI::SetDate(User& user, const std::string& date) {
    if (date.length() == 10) {
        std::string year = date.substr(0, 4);
        std::string month = date.substr(5, 2);
        std::string day = date.substr(8, 2);

        if (date[4] == '-' && date[7] == '-' && IsDigit(year) && IsDigit(month) && IsDigit(day)) {
            int m = std::stoi(month);
            int d = std::stoi(day);
            if (m < 13 && d < 32) {
                user.SetDate(date);
                return true;
            }
        }
    }
    return false;
}

std::string WorkWithAPI::FindCityCode(const User& user, const std::string& city_name) {
    std::string result = "";

    if (!city_codes_loaded_) {
        std::string file_name = kPath_ + "city_codes.json";
        std::ifstream file(file_name);
        if (file.is_open()) {
            file >> current_session_city_code_;
            city_codes_loaded_ = true;
        } else {
            UpdateCityCode(user);
            FindCityCode(user, city_name); 
        }    
    }

    if (!(current_session_city_code_[city_name].is_null())) {
        result = current_session_city_code_[city_name];
    }

    return result;
}

void WorkWithAPI::UpdateCityCode(const User& user) {
    try {
        cpr::Response response = cpr::Get(cpr::Url{"https://api.rasp.yandex.net/v3.0/stations_list/"},
        cpr::Parameters{{"apikey", user.GetKey()},
        {"lang", parameters_.kLang},
        {"format", parameters_.kFormat}});

        nlohmann::json json_file = nlohmann::json::parse(response.text);
        nlohmann::json data;

        for (const auto& country : json_file["countries"]) {
            for (const auto& region : country["regions"]) {
                for (const auto& settlement : region["settlements"]) {
                    if (!settlement["codes"].empty()) {
                        std::string city_name = settlement["title"];
                        std::string code = settlement["codes"]["yandex_code"];
                        data[city_name] = code;
                    }
                }
            }
        }
    
        std::ofstream file(kPath_ + "city_codes.json");
        if (file.is_open()) {
            file << data.dump(4); 
            file.close();
            std::cout << "Информация успешно обновлена" << std::endl;
        } else {
            std::cerr << "Информацию не удалось обновить, попробуйте еще раз" << std::endl;
        }

    } catch (const nlohmann::detail::parse_error& error) {
        std::cerr << "Произошла ошибка парсинга, подождите, попытка повторяется" << std::endl;
    }
}

void WorkWithAPI::UpdateRoutesCache(const User& user) {
    try {
        cpr::Response response = cpr::Get(cpr::Url{"https://api.rasp.yandex.net/v3.0/search/"}, 
        cpr::Parameters{{"apikey", user.GetKey()},
        {"from", user.GetDepartureCity()},
        {"to", user.GetArrivalCity()},
        {"transfers", parameters_.kTransfers},
        {"lang", parameters_.kLang},
        {"page", parameters_.kPage},
        {"date", user.GetDate()}});

        nlohmann::json json_file = nlohmann::json::parse(response.text);
        std::string name = user.GetDate() + user.GetDepartureCity() + user.GetArrivalCity();
        nlohmann::json data; 

        ParseRoutes(data, json_file);

        current_session_json_[name].push_back(data);  
        std::string output_file_name =  kPath_ + name + ".json";

        std::ofstream file(output_file_name);
        if (file.is_open()) {
            file << data.dump(4); 
            file.close();
            std::cout << "Запрос успешно обработан" << std::endl;
        } else {
            std::cerr << "Запрос не удалось обработать, попробуйте еще раз" << std::endl;
        }
    } catch (const nlohmann::detail::parse_error& error) {
        std::cerr << "Произошла ошибка парсинgга, попробуйте еще раз" << std::endl;
    }
}

void WorkWithAPI::ParseRoutes(nlohmann::json& data, const nlohmann::json& json_file) {
    for (const auto& segment : json_file["segments"]) {
        if (!segment["has_transfers"]) {
            data.push_back({{"has_transfer", false},
            {"from", segment["from"]["title"]},
            {"to",  segment["to"]["title"]},
            {"transport_type", segment["from"]["transport_type"]},
            {"departure_time", segment["departure"]},
            {"arrival_time", segment["arrival"]}});
        } else {
            if (segment["transfers"].size() == 1) {
                data.push_back({{"has_transfer", true},
                {"from", segment["details"][0]["from"]["title"]},
                {"to_transfer",  segment["details"][0]["to"]["title"]},
                {"transport_type_before_trans", segment["transport_types"][0]},
                {"departure_time", segment["details"][0]["departure"]},
                {"arrival_transfer_time", segment["details"][0]["arrival"]},
                
                {"from_transfer", segment["details"][2]["from"]["title"]},
                {"to",  segment["details"][2]["to"]["title"]},
                {"transport_type_after_trans", segment["transport_types"][1]},
                {"departure_transfer_time", segment["details"][2]["departure"]},
                {"arrival_time", segment["details"][2]["arrival"]}});
            }
        }  
    }
}

void WorkWithAPI::FindRoutes(const User& user) {
    std::string name = user.GetDate() + user.GetDepartureCity() + user.GetArrivalCity();

    if (!current_session_json_[name].empty()) {

        for (const auto& segment : current_session_json_[name]) {
            if (segment.is_null()) {
                std::cout << "Увы, кажется, что такого маршрута нет(" << std::endl;
                return;
            }   

            for (const auto& elem: segment) {
                PrintJsonElem(elem);
            }         
        }

    } else {
        std::string file_name = kPath_ + name + ".json";
        std::ifstream file(file_name);

        if (file.is_open()) {
            LoadRoutesToCurrentSession(user);
        } else {
            UpdateRoutesCache(user);
        }
        FindRoutes(user);
    }
}

void WorkWithAPI::LoadRoutesToCurrentSession(const User& user) {
    std::string name = user.GetDate() + user.GetDepartureCity() + user.GetArrivalCity();
    std::string file_name = kPath_ + name + ".json";

    std::ifstream file(file_name);
    nlohmann::json json_file;

    if (file.is_open()) {
        file >> json_file;
        current_session_json_[name].push_back(json_file);
    } else {
        std::cerr << "Произошла ошибка, попробуйте еще раз" << std::endl;
    }

}

void WorkWithAPI::DeleteOldFiles() {
    std::string current_time = CurrentTime();

    for (const auto& object : std::filesystem::directory_iterator(kPath_)) {
        if (std::filesystem::is_regular_file(object.status())) {
            std::string file_name =  object.path().filename().string();
            if (file_name != "city_codes" && current_time > file_name) {
                std::filesystem::remove(kPath_ + file_name);    
            }
        }
    }
}

void WorkWithAPI::PrintJsonElem(const nlohmann::basic_json<>& elem) {
    std::string dep_date = elem["departure_time"].dump().substr(1, 10);
    std::string dep_time = elem["departure_time"].dump().substr(12, 8);

    std::string arr_date = elem["arrival_time"].dump().substr(1, 10);
    std::string arr_time = elem["arrival_time"].dump().substr(12, 8);

    std::cout << "-----------------------------------------" << std::endl;
    if (!elem["has_transfer"]) {
        
        printf("Из \x1B[34m%s\033[0m в \x1B[32m%s\033[0m\n", elem["from"].dump().c_str(), elem["to"].dump().c_str());
        std::cout << "Вид транспорта: " << TransportTypeTranslation(elem["transport_type"]) << std::endl;
        printf("Время отправления:  \x1B[44m%s %s\033[0m\n", dep_date.c_str(), dep_time.c_str());
        printf("Время отправления:  \x1B[42m%s %s\033[0m\n", arr_date.c_str(), arr_time.c_str());
        std::cout << "-----------------------------------------" << std::endl;
    } else {
        std::string arr_trans_date = elem["arrival_transfer_time"].dump().substr(1, 10);
        std::string arr_trans_time = elem["arrival_transfer_time"].dump().substr(12, 8);

        std::string dep_trans_date = elem["departure_transfer_time"].dump().substr(1, 10);
        std::string dep_trans_time = elem["departure_transfer_time"].dump().substr(12, 8);

        printf("Из \x1B[34m%s\033[0m в \x1B[32m%s\033[0m\n", elem["from"].dump().c_str(), elem["to_transfer"].dump().c_str());
        std::cout << "Вид транспорта: " << TransportTypeTranslation(elem["transport_type_before_trans"]) << std::endl;
        printf("Время отправления:  \x1B[44m%s %s\033[0m\n", dep_date.c_str(), dep_time.c_str());
        printf("Время прибытия:  \x1B[42m%s %s\033[0m\n", arr_trans_date.c_str(), arr_trans_time.c_str());

        printf("Из \x1B[32m%s\033[0m в \x1B[33m%s\033[0m\n", elem["from_transfer"].dump().c_str(), elem["to"].dump().c_str());
        std::cout << "Вид транспорта: " << TransportTypeTranslation(elem["transport_type_after_trans"]) << std::endl;
        printf("Время отправления:  \x1B[42m%s %s\033[0m\n", dep_trans_date.c_str(), dep_trans_time.c_str());
        printf("Время прибытия:  \x1B[43m%s %s\033[0m\n", arr_date.c_str(), arr_time.c_str());
        std::cout << "-----------------------------------------" << std::endl;
    }

}
