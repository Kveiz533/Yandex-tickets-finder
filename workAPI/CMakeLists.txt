add_library(workAPI workAPI.cpp)

target_link_libraries(workAPI PRIVATE cpr::cpr nlohmann_json::nlohmann_json)
target_include_directories(workAPI PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/json/include)