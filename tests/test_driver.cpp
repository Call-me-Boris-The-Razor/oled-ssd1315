/**
 * @file test_driver.cpp
 * @brief Unit-тесты для Ssd1315Driver с Mock I2C
 */

#include <cassert>
#include <cstdio>

#define OLED_SSD1315_ENABLE 1
#define OLED_USE_ARDUINO 0
#define OLED_USE_STM32HAL 0
#define OLED_ENABLED 1

#include "../include/oled/domain/Ssd1315Driver.hpp"
#include "../include/oled/OledTypes.hpp"
#include "mocks/MockI2c.hpp"

using namespace oled;
using namespace oled::test;

namespace {

class DriverTest {
public:
    void testInitSuccess() {
        MockI2c mockI2c;
        mockI2c.addRespondingAddress(0x3C);

        Ssd1315Driver driver;
        OledConfig cfg;
        cfg.i2cAddr7 = 0x3C;
        cfg.width = 128;
        cfg.height = 64;
        cfg.vccMode = VccMode::InternalChargePump;

        OledResult result = driver.init(mockI2c, cfg);

        assert(result == OledResult::Ok);
        assert(driver.isReady());
        assert(mockI2c.transactionCount() > 0);

        printf("[PASS] testInitSuccess\n");
    }

    void testInitI2cFail() {
        MockI2c mockI2c;
        mockI2c.setFail(true);

        Ssd1315Driver driver;
        OledConfig cfg;
        cfg.i2cAddr7 = 0x3C;
        cfg.width = 128;
        cfg.height = 64;

        OledResult result = driver.init(mockI2c, cfg);

        assert(result == OledResult::I2cError);
        assert(!driver.isReady());

        printf("[PASS] testInitI2cFail\n");
    }

    void testInitInvalidWidth() {
        MockI2c mockI2c;

        Ssd1315Driver driver;
        OledConfig cfg;
        cfg.i2cAddr7 = 0x3C;
        cfg.width = 0;  // Invalid
        cfg.height = 64;

        OledResult result = driver.init(mockI2c, cfg);

        assert(result == OledResult::InvalidArg);

        printf("[PASS] testInitInvalidWidth\n");
    }

    void testInitInvalidHeight() {
        MockI2c mockI2c;

        Ssd1315Driver driver;
        OledConfig cfg;
        cfg.i2cAddr7 = 0x3C;
        cfg.width = 128;
        cfg.height = 48;  // Invalid - only 32 or 64 supported

        OledResult result = driver.init(mockI2c, cfg);

        assert(result == OledResult::InvalidArg);

        printf("[PASS] testInitInvalidHeight\n");
    }

    void testSetPowerNotInitialized() {
        Ssd1315Driver driver;

        OledResult result = driver.setPower(true);

        assert(result == OledResult::NotInitialized);

        printf("[PASS] testSetPowerNotInitialized\n");
    }

    void testSetContrastNotInitialized() {
        Ssd1315Driver driver;

        OledResult result = driver.setContrast(128);

        assert(result == OledResult::NotInitialized);

        printf("[PASS] testSetContrastNotInitialized\n");
    }

    void testWriteBufferNotInitialized() {
        Ssd1315Driver driver;
        uint8_t buffer[1024] = {0};

        OledResult result = driver.writeBuffer(buffer, sizeof(buffer));

        assert(result == OledResult::NotInitialized);

        printf("[PASS] testWriteBufferNotInitialized\n");
    }

    void testWriteBufferNullptr() {
        MockI2c mockI2c;

        Ssd1315Driver driver;
        OledConfig cfg;
        cfg.i2cAddr7 = 0x3C;
        cfg.width = 128;
        cfg.height = 64;

        driver.init(mockI2c, cfg);

        OledResult result = driver.writeBuffer(nullptr, 1024);

        assert(result == OledResult::InvalidArg);

        printf("[PASS] testWriteBufferNullptr\n");
    }

    void runAll() {
        printf("=== Ssd1315Driver Unit Tests ===\n");
        testInitSuccess();
        testInitI2cFail();
        testInitInvalidWidth();
        testInitInvalidHeight();
        testSetPowerNotInitialized();
        testSetContrastNotInitialized();
        testWriteBufferNotInitialized();
        testWriteBufferNullptr();
        printf("=== All tests passed ===\n");
    }
};

} // anonymous namespace

int main() {
    DriverTest test;
    test.runAll();
    return 0;
}
