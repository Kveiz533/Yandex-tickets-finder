add_executable(lab6 main.cpp)

target_link_libraries(lab6 PRIVATE cpr::cpr parser workAPI  nlohmann_json::nlohmann_json user)