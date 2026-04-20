# Лабораторная работа №2 — Создание потоков

**Автор:** Синяков Глеб Максимович  
**Группа:** 12  
**Курс:** 2  
**Дисциплина:** Операционные системы  

---

## Постановка задачи

Разработать программу для консольного процесса, состоящего из трёх потоков: `main`, `min_max` и `average`.

### Поток `main`
1. Создать массив целых чисел (размер и элементы вводятся с консоли)
2. Создать потоки `min_max` и `average`
3. Дождаться завершения обоих потоков через `WaitForSingleObject`
4. Заменить максимальный и минимальный элементы массива на среднее значение
5. Вывести полученные результаты на консоль
6. Завершить работу

### Поток `min_max`
1. Найти минимальный и максимальный элементы массива
2. Вывести их на консоль
3. После **каждого сравнения** элементов — `Sleep(7)` мс
4. Завершить работу

### Поток `average`
1. Найти среднее арифметическое значение элементов массива
2. Вывести его на консоль
3. После **каждой операции суммирования** — `Sleep(12)` мс
4. Завершить работу

---

## Технологии

- **Язык:** C++11
- **Потоки:** WinAPI (`CreateThread`, `WaitForSingleObject`, `Sleep`)
- **Синхронизация:** Общая структура данных `Array`
- **Тестирование:** Google Test
- **Сборка:** CMake 3.10+

---

## Структура проекта

```
lab2/
├── Array.h            # Класс для работы с массивом
├── Array.cpp          # Реализация методов класса Array
├── threads.h          # Объявления потоковых функций
├── threads.cpp        # Реализация MinMaxThread и AverageThread
├── main.cpp           # Главная программа
├── tests.cpp          # Модульные тесты (12 тестов)
├── CMakeLists.txt     # Конфигурация сборки
└── README.md          # Документация
```

---

## Сборка и запуск

### Сборка через CMake

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### Запуск программы

```bash
# Windows
./Release/Main.exe
```

### Запуск тестов

```bash
# Windows
./Release/UnitTests.exe

# Или через CTest
cd build
ctest --output-on-failure
```

---

## Пример работы

```
=== Laboratory work #2: Creating Threads ===
Author: Siniakou Hleb, group 12

Enter array size: 5
Enter 5 integers:
Element [0]: 10
Element [1]: 25
Element [2]: 5
Element [3]: 30
Element [4]: 15

Original array:
10 25 5 30 15

--- Threads execution ---
[Thread min_max] Minimum: 5, Maximum: 30
[Thread average] Average value: 17.0

--- Calculation results ---
Minimum element: 5
Maximum element: 30
Average value: 17.00

Replacing min and max elements with rounded average: 17

Result array:
10 25 17 17 15

=== Program completed successfully ===
```

---

## Архитектура

### `Array.h` / `Array.cpp`
Класс для хранения и обработки массива:
```cpp
struct Array {
    std::vector<int> arr;  // Массив элементов
    int n;                 // Размер массива
    int minVal;            // Минимальный элемент
    int maxVal;            // Максимальный элемент
    double avg;            // Среднее значение
    
    void calculateMinMax();     // Поиск min и max
    void calculateAverage();    // Расчёт среднего
    void makeResArray(int);     // Замена min/max на среднее
};
```

### `threads.h` / `threads.cpp`
Потоковые функции:
- `MinMaxThread` — поиск минимума и максимума с задержкой 7 мс после каждого сравнения
- `AverageThread` — расчёт среднего с задержкой 12 мс после каждого суммирования

### Задержки в потоках
```cpp
// Поток min_max
if (arr[i] < currMin) currMin = arr[i];
Sleep(7);  // После каждого сравнения с минимумом
if (arr[i] > currMax) currMax = arr[i];
Sleep(7);  // После каждого сравнения с максимумом

// Поток average
sum += arr[i];
Sleep(12);  // После каждого суммирования
```

---

## Тестирование

Проект включает **12 модульных тестов**:

| Тест | Описание |
|------|----------|
| `ReplaceMinElement` | Замена минимального элемента на среднее |
| `ReplaceMaxElement` | Замена максимального элемента на среднее |
| `FindCorrectMinMax` | Корректный поиск min и max |
| `CalculateCorrectAverage` | Корректный расчёт среднего |
| `EmptyArray` | Обработка пустого массива |
| `SingleElementArray` | Массив из одного элемента |
| `AllElementsEqual` | Все элементы равны |
| `NegativeAndPositiveValues` | Отрицательные и положительные числа |
| `ReplaceOnlyMinMax` | Замена только min и max |
| `MinMaxAndAverageTogether` | Параллельная работа потоков |
| `DataPointerNotNull` | Проверка указателя на данные |
| `LongArrayAverage` | Большой массив (100 элементов) |

---

## Особенности реализации

### Синхронизация потоков
Потоки `min_max` и `average` работают параллельно с общей структурой `Array`. Синхронизация не требуется, так как:
- `min_max` только читает массив
- `average` только читает массив
- `main` модифицирует массив **после** завершения обоих потоков

### Ожидание завершения
```cpp
WaitForSingleObject(hMinMax, INFINITE);
WaitForSingleObject(hAverage, INFINITE);
```

### Округление среднего
Так как массив целочисленный, среднее значение округляется до ближайшего целого для замены элементов:
```cpp
int roundedAvg = static_cast<int>(data.avg + 0.5);
```

---

## Статус

- [x] Создание массива и ввод данных
- [x] Поток `min_max` с задержками 7 мс
- [x] Поток `average` с задержками 12 мс
- [x] Ожидание потоков через `WaitForSingleObject`
- [x] Замена min и max на среднее
- [x] Вывод результатов
- [x] Модульные тесты (12 тестов)
- [x] Документация

---
