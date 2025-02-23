#include "User.h"

const char* kTimeFormat = "%Y-%m-%d"; 
const int kFormatSize = 11;


std::string CurrentTime() {
    time_t now;
    time(&now);
    std::string result(kFormatSize, '0');
    strftime(&result[0], result.size(), kTimeFormat, gmtime(&now));
    return result.substr(0, 10);
}

void User::SetDepartureCity(const std::string& code) {
    dep_city_code_ = code;
}

void User::SetArrivalCity(const std::string& code) {
    arr_city_code_ = code;
}

void User::SetDate(const std::string& date) { 
    date_ = date;
}

void User::SetKey(const std::string& key) {
    key_= key;
}

void User::SwapCities() {
    std::swap(arr_city_code_, dep_city_code_);
}

std::string User::GetDepartureCity() const {
    return dep_city_code_;
}

std::string User::GetArrivalCity() const {
    return arr_city_code_;
}

std::string User::GetDate() const {
    return date_;
}

std::string User::GetKey() const {
    return key_;
}