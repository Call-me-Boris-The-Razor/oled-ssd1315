/**
 * @file OledSsd1315.cpp
 * @brief Реализация главного класса OledSsd1315
 */

#include "../include/oled/OledSsd1315.hpp"

#if OLED_ENABLED

#include <cstdio>
#include <cstdarg>
#include <cstring>

namespace oled {

#if OLED_USE_ARDUINO
OledSsd1315::OledSsd1315(TwoWire& wire) : wire_(&wire) {
    // Объекты инициализируются в begin()
}
#endif

#if OLED_USE_STM32HAL
OledSsd1315::OledSsd1315(I2C_HandleTypeDef* hi2c) : hi2c_(hi2c) {
    // Объекты инициализируются в begin()
}
#endif

OledSsd1315::~OledSsd1315() {
    // Статические поля - деструктор тривиален
}

OledResult OledSsd1315::begin(const OledConfig& cfg) {
    // Сброс состояния
    resetState();
    
    // Проверка размера буфера
    size_t bufSize = static_cast<size_t>(cfg.width) * cfg.height / 8;
    if (bufSize > OLED_MAX_BUFFER_SIZE) {
        return OledResult::InvalidArg;
    }
    
    // Инициализируем адаптер I2C (platform-specific)
    #if OLED_USE_ARDUINO
    if (!wire_) {
        return OledResult::InvalidArg;
    }
    adapter_.init(*wire_);
    #elif OLED_USE_STM32HAL
    if (!hi2c_) {
        return OledResult::InvalidArg;
    }
    adapter_.init(hi2c_);
    #endif
    
    // Инициализируем драйвер
    OledResult res = driver_.init(adapter_, cfg);
    if (res != OledResult::Ok) {
        return res;
    }
    
    // Инициализируем графический контекст
    gfx_.init(buffer_, cfg.width, cfg.height);
    
    // Очищаем буфер
    gfx_.clear();
    
    initialized_ = true;
    return OledResult::Ok;
}

bool OledSsd1315::isReady() const {
    return initialized_ && driver_.isReady();
}

void OledSsd1315::resetState() {
    initialized_ = false;
    // Статические объекты не требуют освобождения
}

OledResult OledSsd1315::setPower(bool on) {
    if (!isReady()) {
        return OledResult::NotInitialized;
    }
    return driver_.setPower(on);
}

OledResult OledSsd1315::setContrast(uint8_t value) {
    if (!isReady()) {
        return OledResult::NotInitialized;
    }
    return driver_.setContrast(value);
}

OledResult OledSsd1315::invert(bool on) {
    if (!isReady()) {
        return OledResult::NotInitialized;
    }
    return driver_.setInvert(on);
}

void OledSsd1315::clear() {
    if (gfx_.isInitialized()) {
        gfx_.clear();
    }
}

void OledSsd1315::fill(bool color) {
    if (gfx_.isInitialized()) {
        gfx_.fill(color);
    }
}

OledResult OledSsd1315::flush() {
    if (!isReady()) {
        return OledResult::NotInitialized;
    }
    return driver_.writeBuffer(gfx_.buffer(), gfx_.bufferSize());
}

void OledSsd1315::pixel(int x, int y, bool color) {
    if (gfx_.isInitialized()) {
        gfx_.pixel(x, y, color);
    }
}

void OledSsd1315::line(int x0, int y0, int x1, int y1, bool color) {
    if (gfx_.isInitialized()) {
        gfx_.line(x0, y0, x1, y1, color);
    }
}

void OledSsd1315::rect(int x, int y, int w, int h, bool color) {
    if (gfx_.isInitialized()) {
        gfx_.rect(x, y, w, h, color);
    }
}

void OledSsd1315::rectFill(int x, int y, int w, int h, bool color) {
    if (gfx_.isInitialized()) {
        gfx_.rectFill(x, y, w, h, color);
    }
}

void OledSsd1315::setCursor(int x, int y) {
    if (gfx_.isInitialized()) {
        gfx_.setCursor(x, y);
    }
}

void OledSsd1315::setTextSize(uint8_t scale) {
    if (gfx_.isInitialized()) {
        gfx_.setTextSize(scale);
    }
}

void OledSsd1315::setTextColor(bool color) {
    if (gfx_.isInitialized()) {
        gfx_.setTextColor(color);
    }
}

void OledSsd1315::print(const char* str) {
    if (gfx_.isInitialized()) {
        gfx_.print(str);
    }
}

void OledSsd1315::printf(const char* fmt, ...) {
    if (!gfx_.isInitialized()) return;
    
    char buf[OLED_PRINTF_BUFFER_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    
    gfx_.print(buf);
}

// === Диагностика (Фаза 1) ===

OledResult OledSsd1315::getLastResult() const {
    return lastResult_;
}

const char* OledSsd1315::getLastError() const {
    return lastErrorMsg_;
}

uint8_t OledSsd1315::scanAddress(uint8_t startAddr, uint8_t endAddr) {
    if (startAddr > endAddr) {
        return 0;
    }
    
    // Минимальная команда для проверки: пустой буфер
    uint8_t dummy = 0x00;
    
    for (uint8_t addr = startAddr; addr <= endAddr; addr++) {
        bool found = adapter_.write(addr, &dummy, 0);
        if (found) {
            return addr;
        }
    }
    
    return 0;  // Не найден
}

// === STM32 HAL специфичные методы (Фаза 2) ===

#if OLED_USE_STM32HAL

OledResult OledSsd1315::flushDMA() {
    if (!isReady()) {
        lastResult_ = OledResult::NotInitialized;
        lastErrorMsg_ = "Display not initialized";
        return lastResult_;
    }
    
    if (dmaInProgress_) {
        lastResult_ = OledResult::Busy;
        lastErrorMsg_ = "DMA transfer in progress";
        return lastResult_;
    }
    
    // Получаем конфигурацию для адреса
    const OledConfig& cfg = driver_.config();
    uint16_t addr8 = static_cast<uint16_t>(cfg.i2cAddr7) << 1;
    
    // Подготовка буфера с командой данных
    // Для DMA нужен отдельный буфер с префиксом 0x40
    static uint8_t dmaBuffer[OLED_MAX_BUFFER_SIZE + 1];
    dmaBuffer[0] = 0x40;  // Data command
    memcpy(dmaBuffer + 1, gfx_.buffer(), gfx_.bufferSize());
    
    dmaInProgress_ = true;
    
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit_DMA(
        hi2c_,
        addr8,
        dmaBuffer,
        static_cast<uint16_t>(gfx_.bufferSize() + 1)
    );
    
    if (status != HAL_OK) {
        dmaInProgress_ = false;
        lastResult_ = OledResult::I2cError;
        lastErrorMsg_ = "DMA transfer start failed";
        return lastResult_;
    }
    
    lastResult_ = OledResult::Ok;
    lastErrorMsg_ = nullptr;
    return lastResult_;
}

bool OledSsd1315::isDMAComplete() const {
    return !dmaInProgress_;
}

bool OledSsd1315::i2cBusRecovery(void* gpioPort, uint16_t sclPin, uint16_t sdaPin) {
    if (!gpioPort) {
        return false;
    }
    
    GPIO_TypeDef* port = static_cast<GPIO_TypeDef*>(gpioPort);
    
    // Сохраняем текущий режим пинов
    GPIO_InitTypeDef gpio = {0};
    gpio.Mode = GPIO_MODE_OUTPUT_OD;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    
    // Настраиваем SCL как выход
    gpio.Pin = sclPin;
    HAL_GPIO_Init(port, &gpio);
    
    // Проверяем SDA - если LOW, генерируем clock pulses
    bool recovered = false;
    
    for (int i = 0; i < 9; i++) {
        // Генерируем clock pulse
        HAL_GPIO_WritePin(port, sclPin, GPIO_PIN_RESET);
        for (volatile int d = 0; d < 100; d++) {}  // Небольшая задержка
        
        HAL_GPIO_WritePin(port, sclPin, GPIO_PIN_SET);
        for (volatile int d = 0; d < 100; d++) {}
        
        // Проверяем SDA
        if (HAL_GPIO_ReadPin(port, sdaPin) == GPIO_PIN_SET) {
            recovered = true;
            break;
        }
    }
    
    // Генерируем STOP condition
    gpio.Pin = sdaPin;
    HAL_GPIO_Init(port, &gpio);
    
    HAL_GPIO_WritePin(port, sdaPin, GPIO_PIN_RESET);
    for (volatile int d = 0; d < 100; d++) {}
    HAL_GPIO_WritePin(port, sclPin, GPIO_PIN_SET);
    for (volatile int d = 0; d < 100; d++) {}
    HAL_GPIO_WritePin(port, sdaPin, GPIO_PIN_SET);
    
    return recovered;
}

#endif // OLED_USE_STM32HAL

} // namespace oled

#else // OLED_ENABLED == 0

// === Заглушки когда библиотека отключена ===

namespace oled {

OledSsd1315::~OledSsd1315() {}

OledResult OledSsd1315::begin(const OledConfig&) {
    return OledResult::Disabled;
}

bool OledSsd1315::isReady() const {
    return false;
}

void OledSsd1315::resetState() {}

OledResult OledSsd1315::setPower(bool) {
    return OledResult::Disabled;
}

OledResult OledSsd1315::setContrast(uint8_t) {
    return OledResult::Disabled;
}

OledResult OledSsd1315::invert(bool) {
    return OledResult::Disabled;
}

void OledSsd1315::clear() {}

void OledSsd1315::fill(bool) {}

OledResult OledSsd1315::flush() {
    return OledResult::Disabled;
}

void OledSsd1315::pixel(int, int, bool) {}

void OledSsd1315::line(int, int, int, int, bool) {}

void OledSsd1315::rect(int, int, int, int, bool) {}

void OledSsd1315::rectFill(int, int, int, int, bool) {}

void OledSsd1315::setCursor(int, int) {}

void OledSsd1315::setTextSize(uint8_t) {}

void OledSsd1315::setTextColor(bool) {}

void OledSsd1315::print(const char*) {}

void OledSsd1315::printf(const char*, ...) {}

OledResult OledSsd1315::getLastResult() const {
    return OledResult::Disabled;
}

const char* OledSsd1315::getLastError() const {
    return "Library disabled";
}

uint8_t OledSsd1315::scanAddress(uint8_t, uint8_t) {
    return 0;
}

} // namespace oled

#endif // OLED_ENABLED
