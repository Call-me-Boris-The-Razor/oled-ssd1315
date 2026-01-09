# Руководство по STM32 HAL

Инструкция по использованию библиотеки OLED SSD1315 с STM32 и framework `stm32cube`.

---

## Содержание

- [Требования](#требования)
- [Установка](#установка)
- [Конфигурация platformio.ini](#конфигурация-platformioini)
- [Инициализация I2C](#инициализация-i2c)
- [Использование библиотеки](#использование-библиотеки)
- [Аппаратный Reset](#аппаратный-reset)
- [Troubleshooting](#troubleshooting)
- [Примеры](#примеры)

---

## Требования

| Компонент | Версия |
|-----------|--------|
| PlatformIO | 6.0+ |
| Platform | ststm32 |
| Framework | stm32cube |
| STM32 HAL | Любая версия |

### Поддерживаемые семейства STM32

- STM32F0, STM32F1, STM32F3, STM32F4
- STM32G0, STM32G4
- STM32H7
- STM32L4

---

## Установка

### 1. Скопируйте библиотеку

```text
your_project/
├── lib/
│   └── oled_ssd1315/    ← Скопируйте сюда
├── src/
│   └── main.cpp
└── platformio.ini
```

### 2. Добавьте зависимость (альтернатива)

```ini
lib_deps = 
    oled_ssd1315
```

---

## Конфигурация platformio.ini

```ini
[env:your_board]
platform = ststm32
board = blackpill_f411ce   ; Или ваша плата
framework = stm32cube

build_flags = 
    -DOLED_SSD1315_ENABLE=1
    -DOLED_PLATFORM_STM32HAL=1
    -DSTM32F4                  ; Укажите семейство: STM32F1, STM32F4, STM32H7...

lib_deps = 
    oled_ssd1315
```

### Флаги компиляции

| Флаг | Описание |
|------|----------|
| `OLED_SSD1315_ENABLE=1` | Включить библиотеку |
| `OLED_PLATFORM_STM32HAL=1` | Использовать STM32 HAL I2C |
| `STM32xx` | Семейство MCU (для правильных HAL includes) |

---

## Инициализация I2C

### Вариант 1: Ручная настройка

```cpp
#include "stm32f4xx_hal.h"

I2C_HandleTypeDef hi2c1;

void MX_I2C1_Init(void) {
    // Включить тактирование GPIO
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();
    
    // Настройка пинов (PB6=SCL, PB7=SDA для F4)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    // Настройка I2C
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 400000;  // 400 kHz
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    
    HAL_I2C_Init(&hi2c1);
}
```

### Вариант 2: STM32CubeMX

1. Откройте CubeMX
2. Включите I2C1 (или другой)
3. Настройте:
   - Speed: Fast Mode (400 kHz)
   - Address: 7-bit
4. Сгенерируйте код
5. Используйте `hi2c1` в вашем приложении

---

## Использование библиотеки

### Минимальный пример

```cpp
#include "main.h"
#include <oled/OledSsd1315.hpp>

extern I2C_HandleTypeDef hi2c1;

OledSsd1315 oled(&hi2c1);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_I2C1_Init();
    
    // Конфигурация OLED
    OledConfig cfg;
    cfg.i2cAddr7 = 0x3C;
    cfg.width = 128;
    cfg.height = 64;
    cfg.vccMode = VccMode::InternalChargePump;
    
    // Инициализация
    if (oled.begin(cfg) != OledResult::Ok) {
        Error_Handler();
    }
    
    // Вывод текста
    oled.clear();
    oled.setCursor(0, 0);
    oled.print("Привет, STM32!");
    oled.flush();
    
    while (1) {
        // ...
    }
}
```

### Графика

```cpp
// Пиксель
oled.pixel(64, 32, true);

// Линия
oled.line(0, 0, 127, 63, true);

// Прямоугольник
oled.rect(10, 10, 50, 30, true);      // Контур
oled.rectFill(70, 10, 50, 30, true);  // Заливка

// Текст
oled.setCursor(0, 50);
oled.setTextSize(2);
oled.printf("T: %d°C", temperature);

// Отправить на дисплей
oled.flush();
```

---

## Аппаратный Reset

Если у дисплея есть пин RST, используйте callback:

```cpp
// Определите callback
void oledResetCallback(bool high) {
    HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, 
                      high ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// Передайте в конфигурацию
OledConfig cfg;
cfg.resetCallback = &oledResetCallback;
cfg.i2cAddr7 = 0x3C;
// ...

oled.begin(cfg);
```

Без callback библиотека просто ждёт 20мс для стабилизации питания.

---

## Troubleshooting

### Дисплей не отвечает

1. **Проверьте адрес I2C**
   - Большинство дисплеев: `0x3C`
   - Альтернативный: `0x3D`
   - Используйте I2C сканер для проверки

2. **Проверьте подключение**
   ```text
   OLED        STM32
   VCC   -->   3.3V
   GND   -->   GND
   SCL   -->   I2C SCL (с подтяжкой 4.7k к VCC)
   SDA   -->   I2C SDA (с подтяжкой 4.7k к VCC)
   ```

3. **Проверьте тактирование**
   - I2C CLK должен быть включён
   - GPIO CLK должен быть включён

### HAL_I2C_Master_Transmit возвращает HAL_ERROR

```cpp
// Проверьте статус
HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&hi2c1, addr, data, len, 100);
if (status == HAL_BUSY) {
    // I2C занят - возможно, зависла шина
    // Решение: reset I2C или toggle SCL
}
if (status == HAL_TIMEOUT) {
    // Устройство не отвечает
    // Проверьте адрес и подключение
}
```

### Нет текста на дисплее

1. Убедитесь, что вызван `oled.flush()` после рисования
2. Проверьте, что дисплей инициализирован: `oled.isReady()`
3. Попробуйте инвертировать: `oled.invert(true)`

---

## Примеры

### Обновление данных в цикле

```cpp
uint32_t lastUpdate = 0;

while (1) {
    if (HAL_GetTick() - lastUpdate >= 1000) {
        lastUpdate = HAL_GetTick();
        
        // Очистить область
        oled.rectFill(0, 40, 128, 24, false);
        
        // Обновить данные
        oled.setCursor(0, 40);
        oled.printf("Tick: %lu", HAL_GetTick() / 1000);
        
        oled.flush();
    }
}
```

### Несколько дисплеев

```cpp
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;

OledSsd1315 oled1(&hi2c1);
OledSsd1315 oled2(&hi2c2);

void setup_displays() {
    OledConfig cfg1, cfg2;
    cfg1.i2cAddr7 = 0x3C;
    cfg2.i2cAddr7 = 0x3C;
    
    oled1.begin(cfg1);
    oled2.begin(cfg2);
}
```

### Экран загрузки

```cpp
void showBootScreen() {
    oled.clear();
    
    // Логотип (рамка)
    oled.rect(24, 8, 80, 32, true);
    
    // Название
    oled.setCursor(32, 16);
    oled.setTextSize(2);
    oled.print("BOOT");
    
    // Прогресс-бар
    for (int i = 0; i <= 100; i += 10) {
        oled.rectFill(14, 48, i, 8, true);
        oled.flush();
        HAL_Delay(100);
    }
}
```

---

## Пины I2C для популярных плат

| Плата | I2C | SCL | SDA |
|-------|-----|-----|-----|
| BlackPill F411 | I2C1 | PB6 | PB7 |
| BluePill F103 | I2C1 | PB6 | PB7 |
| Nucleo F446RE | I2C1 | PB8 | PB9 |
| DevEBox H743 | I2C1 | PB8 | PB9 |

---

## Ссылки

- [Документация API](API.md)
- [Архитектура библиотеки](ARCHITECTURE.md)
- [Команды SSD1315](SSD1315_COMMANDS.md)
- [STM32CubeMX](https://www.st.com/stm32cubemx)
