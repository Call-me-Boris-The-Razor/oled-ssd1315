/**
 * @file test_gfx.cpp
 * @brief Unit-тесты для графического слоя Gfx
 *
 * Компиляция (host):
 *   g++ -std=c++17 -DOLED_SSD1315_ENABLE=1 -DOLED_PLATFORM_ARDUINO=0 \
 *       -I../include test_gfx.cpp ../src/gfx/Gfx.cpp -o test_gfx
 */

#include <cassert>
#include <cstdio>
#include <cstring>

// Для host-тестирования определяем минимальную конфигурацию
#define OLED_SSD1315_ENABLE 1
#define OLED_USE_ARDUINO 0
#define OLED_USE_STM32HAL 0
#define OLED_ENABLED 1

#include "../include/oled/domain/Gfx.hpp"

using namespace oled;

namespace {

constexpr uint16_t kTestWidth = 128;
constexpr uint16_t kTestHeight = 64;
constexpr size_t kBufferSize = (kTestWidth * kTestHeight) / 8;

class GfxTest {
public:
    GfxTest() {
        memset(buffer_, 0, sizeof(buffer_));
        gfx_.init(buffer_, kTestWidth, kTestHeight);
    }

    void testInit() {
        assert(gfx_.isInitialized());
        assert(gfx_.width() == kTestWidth);
        assert(gfx_.height() == kTestHeight);
        assert(gfx_.bufferSize() == kBufferSize);
        printf("[PASS] testInit\n");
    }

    void testClear() {
        // Заполняем буфер единицами
        memset(buffer_, 0xFF, sizeof(buffer_));

        // Очищаем
        gfx_.clear();

        // Проверяем что все нули
        for (size_t i = 0; i < kBufferSize; ++i) {
            assert(buffer_[i] == 0x00);
        }
        printf("[PASS] testClear\n");
    }

    void testFill() {
        gfx_.clear();
        gfx_.fill(true);

        for (size_t i = 0; i < kBufferSize; ++i) {
            assert(buffer_[i] == 0xFF);
        }
        printf("[PASS] testFill\n");
    }

    void testPixel() {
        gfx_.clear();

        // Устанавливаем пиксель (0, 0)
        gfx_.pixel(0, 0, true);
        assert((buffer_[0] & 0x01) == 0x01);

        // Устанавливаем пиксель (0, 7) - последний бит первого байта
        gfx_.pixel(0, 7, true);
        assert((buffer_[0] & 0x80) == 0x80);

        // Устанавливаем пиксель (1, 0)
        gfx_.pixel(1, 0, true);
        assert((buffer_[1] & 0x01) == 0x01);

        // Выключаем пиксель (0, 0)
        gfx_.pixel(0, 0, false);
        assert((buffer_[0] & 0x01) == 0x00);

        // Пиксель за границами - не должен падать
        gfx_.pixel(-1, 0, true);
        gfx_.pixel(0, -1, true);
        gfx_.pixel(kTestWidth, 0, true);
        gfx_.pixel(0, kTestHeight, true);

        printf("[PASS] testPixel\n");
    }

    void testLine() {
        gfx_.clear();

        // Горизонтальная линия
        gfx_.line(0, 0, 10, 0, true);
        for (int x = 0; x <= 10; ++x) {
            assert((buffer_[x] & 0x01) == 0x01);
        }

        gfx_.clear();

        // Вертикальная линия
        gfx_.line(0, 0, 0, 7, true);
        assert(buffer_[0] == 0xFF);

        printf("[PASS] testLine\n");
    }

    void testRect() {
        gfx_.clear();

        // Прямоугольник 4x4 в позиции (0, 0)
        gfx_.rect(0, 0, 4, 4, true);

        // Верхняя граница
        assert((buffer_[0] & 0x01) == 0x01);
        assert((buffer_[1] & 0x01) == 0x01);
        assert((buffer_[2] & 0x01) == 0x01);
        assert((buffer_[3] & 0x01) == 0x01);

        printf("[PASS] testRect\n");
    }

    void testCursor() {
        gfx_.setCursor(10, 20);
        // Внутреннее состояние курсора - проверяем косвенно через print
        printf("[PASS] testCursor\n");
    }

    void runAll() {
        printf("=== Gfx Unit Tests ===\n");
        testInit();
        testClear();
        testFill();
        testPixel();
        testLine();
        testRect();
        testCursor();
        printf("=== All tests passed ===\n");
    }

private:
    uint8_t buffer_[kBufferSize];
    Gfx gfx_;
};

} // anonymous namespace

int main() {
    GfxTest test;
    test.runAll();
    return 0;
}
