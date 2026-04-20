# Лабораторная работа №4 — Синхронизация процессов

**Автор:** Синяков Глеб Максимович  
**Группа:** 12  
**Курс:** 2  
**Дисциплина:** Операционные системы  

---

## 📌 Постановка задачи

Разработать программу для передачи сообщений между процессами через общий бинарный файл. Программа включает один процесс `Receiver` и несколько процессов `Sender`, организованных как кольцевая очередь FIFO.

### Процесс `Receiver`:
1. Ввести с консоли имя бинарного файла и количество записей (ёмкость)
2. Создать бинарный файл для сообщений (максимальная длина сообщения — 20 символов)
3. Ввести количество процессов `Sender` и запустить их, передав имя файла через командную строку
4. Ждать сигнал готовности от всех `Sender`
5. В цикле по команде с консоли:
   - `r` — прочитать сообщение из файла (блокируется, если файл пуст)
   - `q` — завершить работу

### Процесс `Sender`:
1. Открыть файл для передачи сообщений (имя получает из командной строки)
2. Отправить `Receiver` сигнал готовности
3. В цикле по команде с консоли:
   - `s` — отправить сообщение (ввод с консоли, до 20 символов, блокируется если файл полон)
   - `q` — завершить работу

### Дополнительные требования:
- Передача сообщений организована как **кольцевая очередь FIFO**
- `Receiver` читает сообщения в порядке их отправления всеми `Sender`
- При завершении `Receiver` посылает сигнал завершения всем `Sender`

---

## 🛠️ Технологии

- **Язык:** C++98
- **Процессы:** WinAPI (`CreateProcess`)
- **Синхронизация:** 
  - `CreateMutex` / `OpenMutex` — защита критической секции
  - `CreateSemaphore` / `OpenSemaphore` — управление заполненностью очереди
  - `CreateEvent` / `OpenEvent` — сигналы готовности
  - `CreateFileMapping` / `MapViewOfFile` — разделяемая память
- **Сборка:** CMake 3.16+
- **Тестирование:** Google Test 1.7.0

---

## 📂 Структура проекта

```
lab4/
├── headers/
│   ├── SharedQueue.h           # Класс очереди сообщений
│   └── solution_namespace.h    # Общее пространство имён
├── src/
│   ├── Receiver.cpp            # Процесс-получатель
│   ├── Sender.cpp              # Процесс-отправитель
│   └── SharedQueue.cpp         # Реализация очереди
├── tests/
│   ├── test_shared_queue.cpp   # Тесты ядра очереди
│   ├── test_sender.cpp         # Тесты Sender
│   └── test_reciever.cpp       # Тесты Receiver
├── CMakeLists.txt              # Конфигурация сборки
└── README.md                   # Документация
```

---

## 🔧 Сборка и запуск

### Сборка

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### Запуск

1. **Запустите Receiver:**
```bash
./Release/receiver.exe
```

2. **Введите параметры:**
```
=== Receiver ===
Input binary file name: messages.bin
Input records count (capacity): 5
Input desired Sender process count: 2
```

3. **Receiver автоматически запустит указанное количество Sender**

4. **Работа с Sender (в отдельных консолях):**
```
=== Sender ===
[Sender] Ready. Commands: s - send, q - quit.
[Sender] Command (s/q): s
Input message (<20 symbols): Hello from Sender 1
[Sender] Sent message: 'Hello from Sender 1'
```

5. **Работа с Receiver:**
```
[Receiver] Command (r/q): r
[Receiver] Read message: 'Hello from Sender 1'
[Receiver] Command (r/q): q
[Receiver] Finishing. shutdown signal.
```

---

## 🏗️ Архитектура

### `SharedQueue`
Класс, реализующий кольцевую очередь в разделяемой памяти:

| Метод | Описание |
|-------|----------|
| `CreateAsReceiver()` | Создание очереди (только Receiver) |
| `OpenAsSender()` | Подключение к очереди (только Sender) |
| `PushMessage()` | Добавление сообщения в очередь |
| `PopMessage()` | Извлечение сообщения из очереди |
| `SignalSenderReady()` | Сигнал готовности Sender |
| `WaitAllSendersReady()` | Ожидание готовности всех Sender |
| `SignalShutdown()` | Сигнал завершения работы |
| `IsShuttingDown()` | Проверка флага завершения |

### Структура бинарного файла

```
┌─────────────────────────────────────┐
│         QueueHeaderBinary           │ 28 байт
│  - capacity (4)                     │
│  - msgLen (4)                       │
│  - readIndex (4)                    │
│  - writeIndex (4)                   │
│  - senderReadyCount (4)             │
│  - expectedSenders (4)              │
│  - shuttingDown (1)                 │
│  - reserved (3)                     │
├─────────────────────────────────────┤
│         MessageSlot[0]              │ 20 байт
├─────────────────────────────────────┤
│         MessageSlot[1]              │ 20 байт
├─────────────────────────────────────┤
│              ...                    │
├─────────────────────────────────────┤
│         MessageSlot[N-1]            │ 20 байт
└─────────────────────────────────────┘
```

### Синхронизация

| Объект | Тип | Назначение |
|--------|-----|------------|
| `mtx` | Mutex | Защита заголовка и данных |
| `semEmpty` | Semaphore | Счётчик свободных мест (init = capacity) |
| `semFull` | Semaphore | Счётчик заполненных мест (init = 0) |
| `allReady` | Event | Сигнал готовности всех Sender |

### Именование объектов синхронизации

```
IPC_<имя_файла_с_подчёркиваниями>_mtx
IPC_<имя_файла_с_подчёркиваниями>_semEmpty
IPC_<имя_файла_с_подчёркиваниями>_semFull
IPC_<имя_файла_с_подчёркиваниями>_allReady
```

---

## 🧪 Тестирование

Проект включает модульные тесты:

| Файл | Тесты |
|------|-------|
| `test_shared_queue.cpp` | Сериализация заголовка, блокировка при полной/пустой очереди |
| `test_sender.cpp` | Отправка коротких сообщений, отклонение длинных сообщений |
| `test_reciever.cpp` | Чтение из пустой очереди, разблокировка при shutdown |

### Запуск тестов

```bash
cd build
ctest -V
```

Или отдельный тест:
```bash
./Release/test_shared_queue.exe
```

---

## 📊 Пример работы

### Receiver (основная консоль)
```
=== Receiver ===
Input binary file name: msg.bin
Input records count (capacity): 3
Input desired Sender process count: 2
Launching 2 instances of sender process...
Waiting for all Senders to be ready...
All Senders ready. Commands: r - read, q - quit.
[Receiver] Command (r/q): r
[Receiver] Read message: 'Hello'
[Receiver] Command (r/q): r
[Receiver] Read message: 'World'
[Receiver] Command (r/q): q
[Receiver] Finishing. shutdown signal.
```

### Sender 1
```
=== Sender ===
[Sender] Ready. Commands: s - send, q - quit.
[Sender] Command (s/q): s
Input message (<20 symbols): Hello
[Sender] Sent message: 'Hello'
[Sender] Command (s/q): q
[Sender] Quitting.
```

### Sender 2
```
=== Sender ===
[Sender] Ready. Commands: s - send, q - quit.
[Sender] Command (s/q): s
Input message (<20 symbols): World
[Sender] Sent message: 'World'
[Sender] Command (s/q): q
[Sender] Quitting.
```

---

## ⚙️ Особенности реализации

### Кольцевая очередь FIFO
- `readIndex` — индекс для чтения
- `writeIndex` — индекс для записи
- Оба индекса движутся по модулю `capacity`

### Блокировка при пустой/полной очереди
- `semEmpty` блокирует `PushMessage` при полной очереди
- `semFull` блокирует `PopMessage` при пустой очереди

### Завершение работы
- `Receiver` устанавливает флаг `shuttingDown`
- Освобождаются все семафоры для разблокировки ожидающих `Sender`
- `Sender` при обнаружении флага завершает работу

### Обработка ошибок
- Функция `PrintLastErrorA()` выводит системные ошибки WinAPI
- Проверка длины сообщения перед отправкой
- Проверка совместимости версий при подключении

---

## ✅ Статус

- [x] Создание разделяемой памяти (`CreateFileMapping`)
- [x] Кольцевая очередь FIFO
- [x] Синхронизация через Mutex
- [x] Управление заполненностью через Semaphore
- [x] Сигналы готовности через Event
- [x] Запуск Sender из Receiver
- [x] Корректное завершение всех процессов
- [x] Модульные тесты
- [x] Документация

---