# API Reference — OLED SSD1315

Полное описание публичного API библиотеки.

---

## Содержание

- [Класс OledSsd1315](#класс-oledssd1315)
- [Структура OledConfig](#структура-oledconfig)
- [Перечисление OledResult](#перечисление-oledresult)
- [Перечисление VccMode](#перечисление-vccmode)

---

## Класс OledSsd1315

Основной класс для работы с OLED дисплеем.

### Конструкторы

```cpp
OledSsd1315();
explicit OledSsd1315(TwoWire& wire);
```

| Параметр | Тип | Описание |
|----------|-----|----------|
| `wire` | `TwoWire&` | Ссылка на объект Wire (I2C) |

**Пример:**

```cpp
#include <Wire.h>
#include <oled/OledSsd1315.hpp>

OledSsd1315 oled(Wire);  // Использует стандартный Wire
```

---

### Инициализация

#### `begin()`

```cpp
OledResult begin(const OledConfig& cfg);
```

Инициализирует дисплей с заданной конфигурацией.

| Параметр | Тип | Описание |
|----------|-----|----------|
| `cfg` | `const OledConfig&` | Конфигурация дисплея |

**Возвращает:** `OledResult`

| Значение | Описание |
|----------|----------|
| `Ok` | Успешная инициализация |
| `InvalidArg` | Неверные параметры конфигурации |
| `I2cError` | Ошибка I2C связи |
| `Disabled` | Библиотека отключена (флаг не задан) |

**Пример:**

```cpp
void setup() {
    Wire.begin();
    
    OledConfig cfg;
    cfg.i2cAddr7 = 0x3C;
    cfg.width = 128;
    cfg.height = 64;
    
    OledResult result = oled.begin(cfg);
    if (result != OledResult::Ok) {
        Serial.println("Ошибка инициализации OLED!");
    }
}
```

---

#### `isReady()`

```cpp
bool isReady() const;
```

Проверяет, инициализирован ли дисплей.

**Возвращает:** `true` если дисплей готов к работе.

---

#### `resetState()`

```cpp
void resetState();
```

Сбрасывает внутреннее состояние. После вызова требуется повторный `begin()`.

---

### Управление дисплеем

#### `setPower()`

```cpp
OledResult setPower(bool on);
```

Включает или выключает дисплей (режим сна).

| Параметр | Тип | Описание |
|----------|-----|----------|
| `on` | `bool` | `true` — включить, `false` — выключить |

**Примечание:** При выключении корректно отключается charge pump (если используется).

---

#### `setContrast()`

```cpp
OledResult setContrast(uint8_t value);
```

Устанавливает контрастность дисплея.

| Параметр | Тип | Диапазон | Описание |
|----------|-----|----------|----------|
| `value` | `uint8_t` | 0–255 | Уровень контраста |

**Типичные значения:**
- `0x00` — минимальный контраст
- `0x7F` — средний (по умолчанию)
- `0xFF` — максимальный

---

#### `invert()`

```cpp
OledResult invert(bool on);
```

Инвертирует цвета на дисплее.

| Параметр | Тип | Описание |
|----------|-----|----------|
| `on` | `bool` | `true` — инверсия включена |

---

### Работа с буфером

#### `clear()`

```cpp
void clear();
```

Очищает буфер (все пиксели выключены). **Не отправляет данные на дисплей** — требуется `flush()`.

---

#### `fill()`

```cpp
void fill(bool color);
```

Заполняет весь буфер.

| Параметр | Тип | Описание |
|----------|-----|----------|
| `color` | `bool` | `true` — все пиксели включены, `false` — выключены |

---

#### `flush()`

```cpp
OledResult flush();
```

Отправляет содержимое буфера на дисплей.

**Важно:** Все операции рисования происходят в памяти. Чтобы увидеть изменения на дисплее, необходимо вызвать `flush()`.

**Пример:**

```cpp
oled.clear();
oled.print("Hello!");
oled.flush();  // Теперь текст виден на дисплее
```

---

### Графические примитивы

#### `pixel()`

```cpp
void pixel(int x, int y, bool color);
```

Устанавливает пиксель.

| Параметр | Тип | Описание |
|----------|-----|----------|
| `x` | `int` | Координата X (0 = левый край) |
| `y` | `int` | Координата Y (0 = верхний край) |
| `color` | `bool` | `true` — включён, `false` — выключен |

---

#### `line()`

```cpp
void line(int x0, int y0, int x1, int y1, bool color);
```

Рисует линию (алгоритм Брезенхэма).

| Параметр | Тип | Описание |
|----------|-----|----------|
| `x0`, `y0` | `int` | Начальная точка |
| `x1`, `y1` | `int` | Конечная точка |
| `color` | `bool` | Цвет линии |

---

#### `rect()`

```cpp
void rect(int x, int y, int w, int h, bool color);
```

Рисует контур прямоугольника.

| Параметр | Тип | Описание |
|----------|-----|----------|
| `x`, `y` | `int` | Левый верхний угол |
| `w`, `h` | `int` | Ширина и высота |
| `color` | `bool` | Цвет контура |

---

#### `rectFill()`

```cpp
void rectFill(int x, int y, int w, int h, bool color);
```

Рисует залитый прямоугольник.

---

### Текст

#### `setCursor()`

```cpp
void setCursor(int x, int y);
```

Устанавливает позицию курсора для вывода текста.

---

#### `setTextSize()`

```cpp
void setTextSize(uint8_t scale);
```

Устанавливает масштаб текста.

| Значение | Размер символа |
|----------|----------------|
| `1` | 5×7 пикселей (по умолчанию) |
| `2` | 10×14 пикселей |
| `3` | 15×21 пиксель |

---

#### `setTextColor()`

```cpp
void setTextColor(bool color);
```

Устанавливает цвет текста.

| Значение | Описание |
|----------|----------|
| `true` | Белый текст на чёрном фоне (по умолчанию) |
| `false` | Чёрный текст (для инвертированных областей) |

---

#### `print()`

```cpp
void print(const char* str);
```

Выводит строку с текущей позиции курсора.

**Поддерживает:**
- ASCII символы (32–126)
- Кириллицу (А–Я, а–я, Ё, ё)
- UTF-8 кодировку
- Автоматический перенос строки

**Пример:**

```cpp
oled.setCursor(0, 0);
oled.print("Привет мир!");
oled.print("\nHello World!");
```

---

#### `printf()`

```cpp
void printf(const char* fmt, ...);
```

Форматированный вывод (как стандартный `printf`).

**Ограничение:** Максимальная длина результата — 128 символов.

**Пример:**

```cpp
int temp = 25;
float voltage = 3.3f;
oled.printf("Temp: %d C\n", temp);
oled.printf("Vcc: %.1f V", voltage);
```

---

## Структура OledConfig

Конфигурация дисплея.

```cpp
struct OledConfig {
    uint8_t  i2cAddr7 = 0x3C;
    uint16_t width    = 128;
    uint16_t height   = 64;
    uint32_t i2cFreq  = 400000;
    VccMode  vccMode  = VccMode::InternalChargePump;
    bool     flip180  = false;
    ResetGpioCallback resetCallback = nullptr;
};
```

| Поле | Тип | По умолчанию | Описание |
|------|-----|--------------|----------|
| `i2cAddr7` | `uint8_t` | `0x3C` | 7-битный I2C адрес |
| `width` | `uint16_t` | `128` | Ширина в пикселях |
| `height` | `uint16_t` | `64` | Высота в пикселях (32 или 64) |
| `i2cFreq` | `uint32_t` | `400000` | Частота I2C (Гц) |
| `vccMode` | `VccMode` | `InternalChargePump` | Режим питания |
| `flip180` | `bool` | `false` | Поворот на 180° |
| `resetCallback` | `ResetGpioCallback` | `nullptr` | Callback для аппаратного reset (platform-agnostic) |

---

## Перечисление OledResult

Результаты операций.

```cpp
enum class OledResult {
    Ok,             // Успешно
    Disabled,       // Библиотека отключена
    I2cError,       // Ошибка I2C
    NotInitialized, // Не инициализировано
    InvalidArg,     // Неверный аргумент
    Unsupported     // Не поддерживается
};
```

---

## Перечисление VccMode

Режим питания дисплея.

```cpp
enum class VccMode {
    InternalChargePump, // Внутренний DC-DC преобразователь
    ExternalVcc         // Внешнее питание VCC
};
```

| Режим | Описание |
|-------|----------|
| `InternalChargePump` | Используется встроенный charge pump. Подходит для большинства модулей. |
| `ExternalVcc` | Внешний источник питания OLED панели. Для специальных конфигураций. |

---

## См. также

- [Руководство по быстрому старту](QUICKSTART.md)
- [Архитектура библиотеки](ARCHITECTURE.md)
- [Примеры использования](EXAMPLES.md)
