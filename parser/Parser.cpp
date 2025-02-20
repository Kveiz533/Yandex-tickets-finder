#include "user/User.h"
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include "workAPI/workAPI.h"
#include <sstream>
#include <iterator>


std::vector<std::string> SplitString(const std::string& str) {
    std::istringstream iss(str);
    return {std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>()};
}

void PrintInfo() {
    printf("\x1B[42mКонсольное приложение поддерживает следующие команды:\033[0m\n");
    printf("\x1B[33m1)\033[0m \x1B[32mset arr_city\033[0m название_города (устанавлиевает город прибытия)\n");
    printf("\x1B[33m2)\033[0m \x1B[32mset dep_city\033[0m название_город (устанавлиевает город отправления)\n");
    printf("\x1B[33m3)\033[0m \x1B[32mset date\033[0m дата (в формате YYYY-MM-DD)\n");
    printf("\x1B[33m4)\033[0m \x1B[32mset key\033[0m ключ (от ЯндексAPI)\n");
    printf("\x1B[33m5)\033[0m \x1B[32mswap cities\033[0m (меняет города прибытия и отправления местами)\n");
    printf("\x1B[33m7)\033[0m \x1B[32mupdate result\033[0m (обновляет информацию о выбранном рейсе)\n");
    printf("\x1B[33m8)\033[0m \x1B[32mget result\033[0m (выводит реультат)\n");
    printf("\x1B[33m8)\033[0m \x1B[32mprint info\033[0m (выводит список доступных команд)\n");
    printf("------------------------------------------------------\n");
}

void Parse() {
    std::string cmd;
    bool is_active = true;
    bool key_recieved = false;
    User user;
    WorkWithAPI work_with_api;

    PrintInfo();
    printf("\x1B[33mPS.\033[0m \n");
    printf("дата по умолчанию - текущая;\n");
    printf("город отправления по умолчанию - Санкт-Петербург;\n");
    printf("город прибытия по умолчанию - Ростов-на-Дону;\n");
    printf("\x1B[31mключ для Яндекс API нужно обязательно установить\033[0m\n");
    printf("------------------------------------------------------\n");


    while(is_active) {    
        std::getline(std::cin, cmd);
        std::vector<std::string> line = SplitString(cmd);

        if (line.empty()) {
            continue;
        } else {
            if (line[0] == "exit") {
                work_with_api.DeleteOldFiles();
                std::cout << "Goodbye, see you later)" << std::endl;
                is_active = false;
            } else if (line[0] == "set" && line[1] == "arr_city") {
                if (line.size() == 3 && work_with_api.SetArrivalCityCode(user, line[2])) {
                    std::cout << "Город приезда установлен" << std::endl;
                } else {
                    std::cout << "Город приезда не установлен" << std::endl;
                }
            } else if (line[0] == "set" && line[1] == "dep_city") {
                if (line.size() == 3 && work_with_api.SetDepartureCityCode(user,line[2])) {
                    std::cout << "Город выезда установлен" << std::endl;
                } else {
                    std::cout << "Город выезда не установлен" << std::endl;
                }
            } else if (line[0] == "set" && line[1] == "date") {
                if (line.size() == 3 && work_with_api.SetDate(user, line[2])) {
                    std::cout << "Дата установлена" << std::endl;
                } else {
                    std::cout << "Дата не установлена" << std::endl;
                }
            } else if (line[0] == "set" && line[1] == "key") {
                if (line.size() == 3 && work_with_api.SetKey(user, line[2])) {
                    key_recieved = true;
                    std::cout << "Ключ установлен" << std::endl;
                } else {
                    std::cout << "Ключ не установлен" << std::endl;
                }
            } else if (line[0] == "get" && line[1] == "result") {
                if (key_recieved) {
                    work_with_api.FindRoutes(user);
                } else {
                    std::cout << "Сначала установите ключ" << std::endl;
                }
            } else if (key_recieved && line[0] == "update" && line[1] == "result") {
                if (key_recieved) {
                    work_with_api.UpdateRoutesCache(user);
                } else {
                    std::cout << "Сначала установите ключ" << std::endl;
                }
            } else if (line[0] == "swap" && line[1] == "cities") {
                user.SwapCities();
                std::cout << "Город прибытия и отправления поменялись местами" << std::endl;
            } else if (line[0] == "print" && line[1] == "info") {
                PrintInfo();
            } else {
                std::cerr << "Wrong Command" << std::endl;
            }
        }
    }
}