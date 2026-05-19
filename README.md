# sem2_lab3

Лабораторная работа №3 (2 семестр), вариант: **Стек**.

## Реализовано
- База из ЛР2: DynamicArray, LinkedList, Sequence, ArraySequence, ListSequence
- Целевой АТД: `Stack<T>`
- Операции стека: `Push`, `Pop`, `Peek`, `IsEmpty`, `GetSize`
- Доп. операции по варианту:
  - `map`, `where`, `reduce`
  - конкатенация (`Concat`)
  - извлечение подпоследовательности (`GetSubStack`)
  - поиск подпоследовательности (`FindSubStack`)
- Консольный UI для ручного тестирования (`lr2_console`)
- Модульные тесты (`lr2_tests`)

## Сборка
```bash
./build.sh
./build/lr3_tests
./build/lr3_console
./build/lr3_gtk
./build/lr3_bench
```
