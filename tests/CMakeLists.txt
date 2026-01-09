# CMakeLists.txt для unit-тестов OLED SSD1315
#
# Сборка:
#   mkdir build && cd build
#   cmake .. -DCMAKE_BUILD_TYPE=Debug
#   cmake --build .
#   ctest --output-on-failure

cmake_minimum_required(VERSION 3.14)
project(oled_ssd1315_tests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Предупреждения как ошибки (согласно требованиям раздела 7.1)
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)

# Санитайзеры для Debug сборки (согласно требованиям раздела 7.3)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif()

# Определения для host-тестирования
add_compile_definitions(
    OLED_SSD1315_ENABLE=1
    OLED_USE_ARDUINO=0
    OLED_USE_STM32HAL=0
    OLED_ENABLED=1
)

# Include пути
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

# Исходники библиотеки (только domain, без platform-specific)
set(LIB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/gfx/Gfx.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/driver/Ssd1315Driver.cpp
)

# Тест Gfx
add_executable(test_gfx
    test_gfx.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/gfx/Gfx.cpp
)

# Тест Driver
add_executable(test_driver
    test_driver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/driver/Ssd1315Driver.cpp
)

# Регистрация тестов
enable_testing()
add_test(NAME GfxTests COMMAND test_gfx)
add_test(NAME DriverTests COMMAND test_driver)

# Цель для запуска всех тестов
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_gfx test_driver
)
