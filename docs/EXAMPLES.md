# Примеры использования — OLED SSD1315

Практические примеры для различных задач.

---

## Содержание

- [Базовый пример](#базовый-пример)
- [Кириллица и UTF-8](#кириллица-и-utf-8)
- [Графические примитивы](#графические-примитивы)
- [Анимация](#анимация)
- [Отображение датчиков](#отображение-датчиков)
- [Меню](#простое-меню)
- [Экран загрузки](#экран-загрузки)

---

## Базовый пример

```cpp
#include <Wire.h>
#include <oled/OledSsd1315.hpp>

OledSsd1315 oled(Wire);

void setup() {
    Wire.begin();
    
    OledConfig cfg;
    cfg.i2cAddr7 = 0x3C;
    
    oled.begin(cfg);
    oled.clear();
    oled.print("Hello World!");
    oled.flush();
}

void loop() {}
```

---

## Кириллица и UTF-8

Библиотека полностью поддерживает русский язык.

```cpp
void setup() {
    Wire.begin();
    
    OledConfig cfg;
    oled.begin(cfg);
    
    oled.clear();
    
    // Русский текст
    oled.setCursor(0, 0);
    oled.print("Привет мир!");
    
    // Смешанный текст
    oled.setCursor(0, 10);
    oled.print("Hello Мир!");
    
    // Кириллица в printf
    oled.setCursor(0, 20);
    oled.printf("Темп: %d°C", 25);
    
    // Буквы Ё и ё
    oled.setCursor(0, 30);
    oled.print("Ёлка, ёжик");
    
    oled.flush();
}
```

**Поддерживаемые символы:**
- ASCII: 32–126 (латиница, цифры, знаки)
- Кириллица: А–Я, а–я, Ё, ё

---

## Графические примитивы

### Точки и линии

```cpp
void drawPrimitives() {
    oled.clear();
    
    // Точки
    for (int i = 0; i < 128; i += 4) {
        oled.pixel(i, 0, true);
    }
    
    // Горизонтальная линия
    oled.line(0, 10, 127, 10, true);
    
    // Вертикальная линия
    oled.line(64, 10, 64, 63, true);
    
    // Диагональ
    oled.line(0, 20, 127, 63, true);
    
    oled.flush();
}
```

### Прямоугольники

```cpp
void drawRects() {
    oled.clear();
    
    // Контуры
    oled.rect(0, 0, 40, 30, true);
    oled.rect(5, 5, 30, 20, true);
    
    // Залитые
    oled.rectFill(50, 0, 20, 20, true);
    oled.rectFill(80, 0, 40, 30, true);
    
    // Инвертированный текст внутри
    oled.setCursor(82, 10);
    oled.setTextColor(false);  // Чёрный текст
    oled.print("OK");
    oled.setTextColor(true);   // Вернуть белый
    
    oled.flush();
}
```

### Рамка экрана

```cpp
void drawBorder() {
    oled.clear();
    
    // Рамка по периметру
    oled.rect(0, 0, 128, 64, true);
    
    // Заголовок
    oled.rectFill(0, 0, 128, 12, true);
    oled.setCursor(2, 2);
    oled.setTextColor(false);
    oled.print("MENU");
    oled.setTextColor(true);
    
    // Контент
    oled.setCursor(4, 16);
    oled.print("Item 1");
    oled.setCursor(4, 26);
    oled.print("Item 2");
    
    oled.flush();
}
```

---

## Анимация

### Бегущая строка

```cpp
void marquee() {
    const char* text = "  OLED SSD1315 Library  ";
    int textWidth = strlen(text) * 6;  // 5px + 1px gap
    int offset = 0;
    
    while (true) {
        oled.clear();
        oled.setCursor(-offset, 28);
        oled.print(text);
        oled.flush();
        
        offset++;
        if (offset > textWidth) offset = 0;
        
        delay(50);
    }
}
```

### Мигающий курсор

```cpp
void blinkingCursor() {
    bool visible = true;
    
    oled.clear();
    oled.print("Input: ");
    oled.flush();
    
    int cursorX = 42;  // После "Input: "
    
    while (true) {
        // Курсор
        oled.rectFill(cursorX, 0, 6, 8, visible);
        oled.flush();
        
        visible = !visible;
        delay(500);
    }
}
```

### Простой прогресс-бар

```cpp
void progressBar(int percent) {
    oled.clear();
    
    // Рамка прогресс-бара
    oled.rect(10, 25, 108, 14, true);
    
    // Заполнение
    int fillWidth = (percent * 104) / 100;
    oled.rectFill(12, 27, fillWidth, 10, true);
    
    // Процент
    oled.setCursor(50, 45);
    oled.printf("%d%%", percent);
    
    oled.flush();
}

void loop() {
    for (int i = 0; i <= 100; i++) {
        progressBar(i);
        delay(50);
    }
}
```

---

## Отображение датчиков

### Температура и влажность

```cpp
void showSensorData(float temp, float humidity) {
    oled.clear();
    
    // Заголовок
    oled.setCursor(0, 0);
    oled.setTextSize(1);
    oled.print("Датчики:");
    
    // Температура (крупно)
    oled.setCursor(0, 16);
    oled.setTextSize(2);
    oled.printf("%.1f", temp);
    oled.setTextSize(1);
    oled.print(" C");
    
    // Влажность
    oled.setCursor(0, 40);
    oled.setTextSize(2);
    oled.printf("%.0f", humidity);
    oled.setTextSize(1);
    oled.print(" %");
    
    // Иконки (простые)
    oled.setCursor(100, 16);
    oled.print("T");
    oled.setCursor(100, 40);
    oled.print("H");
    
    oled.flush();
}
```

### График значений

```cpp
#define GRAPH_POINTS 100
int graphData[GRAPH_POINTS] = {0};
int graphIndex = 0;

void addGraphPoint(int value) {
    graphData[graphIndex] = value;
    graphIndex = (graphIndex + 1) % GRAPH_POINTS;
}

void drawGraph() {
    oled.clear();
    
    // Оси
    oled.line(10, 0, 10, 50, true);   // Y
    oled.line(10, 50, 127, 50, true); // X
    
    // Данные
    for (int i = 0; i < GRAPH_POINTS - 1; i++) {
        int idx1 = (graphIndex + i) % GRAPH_POINTS;
        int idx2 = (graphIndex + i + 1) % GRAPH_POINTS;
        
        int y1 = 50 - (graphData[idx1] * 45 / 100);
        int y2 = 50 - (graphData[idx2] * 45 / 100);
        
        oled.line(12 + i, y1, 13 + i, y2, true);
    }
    
    oled.flush();
}
```

---

## Простое меню

```cpp
const char* menuItems[] = {
    "Настройки",
    "Яркость",
    "Контраст", 
    "Сброс",
    "Выход"
};
const int menuCount = 5;
int menuSelected = 0;

void drawMenu() {
    oled.clear();
    
    for (int i = 0; i < menuCount; i++) {
        int y = i * 12;
        
        if (i == menuSelected) {
            // Выделенный пункт
            oled.rectFill(0, y, 128, 11, true);
            oled.setCursor(4, y + 2);
            oled.setTextColor(false);
            oled.print(menuItems[i]);
            oled.setTextColor(true);
        } else {
            oled.setCursor(4, y + 2);
            oled.print(menuItems[i]);
        }
    }
    
    oled.flush();
}

void menuUp() {
    if (menuSelected > 0) menuSelected--;
    drawMenu();
}

void menuDown() {
    if (menuSelected < menuCount - 1) menuSelected++;
    drawMenu();
}
```

---

## Экран загрузки

```cpp
void splashScreen() {
    oled.clear();
    
    // Логотип (простой)
    oled.rectFill(44, 10, 40, 30, true);
    oled.setCursor(52, 18);
    oled.setTextColor(false);
    oled.setTextSize(2);
    oled.print("SK");
    oled.setTextColor(true);
    oled.setTextSize(1);
    
    // Название
    oled.setCursor(20, 48);
    oled.print("OLED SSD1315 v1.1");
    
    oled.flush();
    delay(2000);
}
```

---

## Управление питанием

### Режим сна

```cpp
void enterSleep() {
    oled.setPower(false);  // Выключить дисплей
    // Дисплей потребляет ~10µA в режиме сна
}

void wakeUp() {
    oled.setPower(true);   // Включить дисплей
    oled.flush();          // Восстановить изображение
}
```

### Автояркость

```cpp
void adjustBrightness(int lightLevel) {
    // lightLevel: 0-1023 (от датчика освещённости)
    uint8_t contrast = map(lightLevel, 0, 1023, 0, 255);
    oled.setContrast(contrast);
}
```

---

## См. также

- [API Reference](API.md)
- [Архитектура](ARCHITECTURE.md)
