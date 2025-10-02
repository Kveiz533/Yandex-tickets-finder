add_library(user User.cpp)

target_include_directories(user PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/json/include)
