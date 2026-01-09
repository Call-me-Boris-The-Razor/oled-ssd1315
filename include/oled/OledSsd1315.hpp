/**
 * @file OledSsd1315.hpp
 * @brief Главный заголовочный файл библиотеки OLED SSD1315
 * 
 * Библиотека для управления OLED дисплеями с контроллером SSD1315 по I2C.
 * 
 * Включение библиотеки:
 *   build_flags = -DOLED_SSD1315_ENABLE=1
 * 
 * Если флаг не задан - библиотека компилируется как заглушки.
 */

#ifndef OLED_SSD1315_HPP
#define OLED_SSD1315_HPP

#include "OledConfig.hpp"
#include "OledTypes.hpp"
#include "internal/Ssd1315Driver.hpp"
#include "internal/Gfx.hpp"
#include <cstdarg>

// Platform-specific includes
#if OLED_ENABLED
    #if OLED_USE_ARDUINO
        #include <Wire.h>
        #include "internal/WireI2cAdapter.hpp"
    #elif OLED_USE_STM32HAL
        #include "internal/Stm32HalI2cAdapter.hpp"
    #endif
#endif

namespace oled {

/**
 * @brief Основной класс для работы с OLED SSD1315
 * 
 * Объединяет транспорт, драйвер и графику в единый API.
 * 
 * Пример использования (Arduino):
 * @code
 * #include <Wire.h>
 * #include <oled/OledSsd1315.hpp>
 * 
 * OledSsd1315 oled(Wire);
 * 
 * void setup() {
 *     Wire.begin();
 *     OledConfig cfg;
 *     cfg.i2cAddr7 = 0x3C;
 *     oled.begin(cfg);
 *     oled.clear();
 *     oled.print("Hello!");
 *     oled.flush();
 * }
 * @endcode
 * 
 * Пример использования (STM32 HAL):
 * @code
 * #include <oled/OledSsd1315.hpp>
 * 
 * I2C_HandleTypeDef hi2c1;
 * OledSsd1315 oled(&hi2c1);
 * 
 * int main() {
 *     HAL_Init();
 *     MX_I2C1_Init();
 *     OledConfig cfg;
 *     cfg.i2cAddr7 = 0x3C;
 *     oled.begin(cfg);
 *     oled.print("Привет!");
 *     oled.flush();
 * }
 * @endcode
 */
class OledSsd1315 {
public:
#if OLED_ENABLED
    /**
     * @brief Конструктор по умолчанию
     */
    OledSsd1315() = default;
    
    #if OLED_USE_ARDUINO
    /**
     * @brief Конструктор с Arduino Wire
     * @param wire Ссылка на TwoWire (обычно Wire)
     */
    explicit OledSsd1315(TwoWire& wire);
    #endif
    
    #if OLED_USE_STM32HAL
    /**
     * @brief Конструктор с STM32 HAL I2C
     * @param hi2c Указатель на I2C_HandleTypeDef
     */
    explicit OledSsd1315(I2C_HandleTypeDef* hi2c);
    #endif
#else
    /**
     * @brief Конструктор по умолчанию (когда библиотека отключена)
     */
    OledSsd1315() = default;
    
    /**
     * @brief Конструктор-заглушка (когда библиотека отключена)
     */
    template<typename T>
    explicit OledSsd1315(T*) {}
    template<typename T>
    explicit OledSsd1315(T&) {}
#endif

    /**
     * @brief Деструктор
     */
    ~OledSsd1315();
    
    // Запрет копирования
    OledSsd1315(const OledSsd1315&) = delete;
    OledSsd1315& operator=(const OledSsd1315&) = delete;
    
    // === Инициализация ===
    
    /**
     * @brief Инициализировать дисплей
     * @param cfg Конфигурация дисплея
     * @return OledResult::Ok при успехе, OledResult::Disabled если библиотека выключена
     */
    OledResult begin(const OledConfig& cfg);
    
    /**
     * @brief Проверить готовность дисплея
     */
    bool isReady() const;
    
    /**
     * @brief Сбросить состояние (требует повторного begin)
     */
    void resetState();
    
    // === Управление дисплеем ===
    
    /**
     * @brief Включить/выключить дисплей
     * @param on true - включить (0xAF), false - выключить/sleep (0xAE)
     */
    OledResult setPower(bool on);
    
    /**
     * @brief Установить контраст
     * @param value Уровень 0-255
     */
    OledResult setContrast(uint8_t value);
    
    /**
     * @brief Инвертировать цвета
     * @param on true - инверсия включена
     */
    OledResult invert(bool on);
    
    // === Буфер ===
    
    /**
     * @brief Очистить буфер (все пиксели выключены)
     */
    void clear();
    
    /**
     * @brief Заполнить буфер
     * @param color true = все пиксели включены
     */
    void fill(bool color);
    
    /**
     * @brief Отправить буфер на дисплей
     */
    OledResult flush();
    
    // === Примитивы ===
    
    /**
     * @brief Установить пиксель
     */
    void pixel(int x, int y, bool color);
    
    /**
     * @brief Нарисовать линию
     */
    void line(int x0, int y0, int x1, int y1, bool color);
    
    /**
     * @brief Нарисовать прямоугольник (контур)
     */
    void rect(int x, int y, int w, int h, bool color);
    
    /**
     * @brief Нарисовать залитый прямоугольник
     */
    void rectFill(int x, int y, int w, int h, bool color);
    
    // === Текст ===
    
    /**
     * @brief Установить позицию курсора
     */
    void setCursor(int x, int y);
    
    /**
     * @brief Установить масштаб текста (1, 2, 3...)
     */
    void setTextSize(uint8_t scale);
    
    /**
     * @brief Установить цвет текста
     */
    void setTextColor(bool color);
    
    /**
     * @brief Вывести строку
     */
    void print(const char* str);
    
    /**
     * @brief Вывести форматированную строку (как printf)
     * @note Максимальная длина результата - 128 символов
     */
    void printf(const char* fmt, ...);
    
private:
#if OLED_ENABLED
    #if OLED_USE_ARDUINO
    TwoWire* wire_ = nullptr;
    WireI2cAdapter adapter_;
    #elif OLED_USE_STM32HAL
    I2C_HandleTypeDef* hi2c_ = nullptr;
    Stm32HalI2cAdapter adapter_;
    #endif
    Ssd1315Driver driver_;
    Gfx gfx_;
    uint8_t buffer_[OLED_MAX_BUFFER_SIZE] = {0};
    bool initialized_ = false;
#endif
};

} // namespace oled

// Экспорт типов в глобальное пространство для удобства
using oled::OledSsd1315;
using oled::OledConfig;
using oled::OledResult;
using oled::VccMode;

#endif // OLED_SSD1315_HPP
