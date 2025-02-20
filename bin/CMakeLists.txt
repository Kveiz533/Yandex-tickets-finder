add_executable(lab6 main.cpp)

target_link_libraries(lab6 PRIVATE cpr::cpr)
target_link_libraries(lab6 PRIVATE parser)
target_link_libraries(lab6 PRIVATE workAPI)
target_link_libraries(lab6 PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(lab6 PRIVATE user)