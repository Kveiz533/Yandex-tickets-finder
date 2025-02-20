add_library(parser Parser.cpp)

target_include_directories(parser PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/json/include)