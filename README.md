# sem2_lab3

Лабораторная работа №3 (2 семестр)  
Вариант: **№3 — Стек (Stack)**

## Что реализовано

### Базовые АТД (наследие ЛР2)
- `DynamicArray<T>`
- `LinkedList<T>`
- `Sequence<T>`
- `ArraySequence<T>`
- `ListSequence<T>`

### Целевой АТД
- `Stack<T>` на базе `ArraySequence<T>`

### Базовые операции стека
- `Push(const T&)`
- `Pop()`
- `Peek()`
- `IsEmpty()`
- `GetSize()`

### Дополнительные операции по ТЗ
- `Map`
- `Where`
- `Reduce`
- `Concat` (конкатенация)
- `GetSubStack(start, end)` (извлечение подпоследовательности)
- `FindSubStack(pattern)` (поиск вхождения подпоследовательности)

### Безопасные операции (try-семантика)
- `TryPop() -> Option<T>`
- `TryPeek() -> Option<T>`

## UI

### Консольный UI
Файл: `src/main.cpp`  
Поддерживает ручное тестирование:
- Push / TryPop / TryPeek
- Show
- Map / Where / Reduce
- Concat
- SubStack
- FindSub

### GTK UI (графический)
Файл: `ui/gtk_main.cpp`  
Интерактивные кнопки для всех ключевых операций стека (по аналогии с ЛР2).

## Тесты
Файл: `tests/tests.cpp`

Покрыто:
- корректность основных операций
- корректность дополнительных операций
- разные типы элементов (`int`, `double`, `std::string`)
- негативные кейсы (исключения):
  - Pop пустого стека
  - Peek пустого стека
  - некорректный диапазон `GetSubStack`

## Оценка производительности
Файл: `src/benchmark.cpp`  
Цель: `lr3_bench`

Измеряются (в мс):
- `push`
- `pop`
- `concat`
- `findSub`

Для размеров:
- 1 000
- 10 000
- 100 000

Формат вывода: CSV (`op,size,ms`).

## Сборка и запуск
```bash
./build.sh
./build/lr3_tests
./build/lr3_console
./build/lr3_gtk
./build/lr3_bench
./build/lr3_queue_tests
./build/lr3_queue_bench
```

## Структура проекта
- `include/` — заголовочные файлы и АТД
- `src/` — консольное приложение и benchmark
- `tests/` — модульные тесты
- `ui/` — GTK интерфейс
- `CMakeLists.txt` — конфигурация сборки

## Примечание
Проект использует шаблоны C++ и ООП, а также обработку некорректных входных параметров через исключения и `Option<T>` для безопасных операций.
