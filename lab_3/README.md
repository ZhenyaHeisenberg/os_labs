# Лабораторная работа №3 — потоки и синхронизация

Среднее арифметическое 512-битных hex-чисел из файла, многопоточная обработка

## Зависимости

- CMake ≥ 3.20
- Компилятор C11 (MSYS2 UCRT64 `gcc`, MSVC, clang)
- **GMP**:
  - MSYS2 UCRT64: `pacman -S mingw-w64-ucrt-x86_64-gmp`
  - Linux: `sudo apt install libgmp-dev`
  - macOS: `brew install gmp`

## Сборка

```bash
cmake -S . -B build
cmake --build build
```

## Запуск

```bash
./build/lab3
```

Параметры задаются в `main.c` (переменные `t_min`, `t_max`, `mem_mb`, `file`):

- `t_min == t_max` — один прогон с `t_min` потоками (с лимитом `mem_mb`);
- `t_min < t_max` — бенчмарк от `t_min` до `t_max` (лимит памяти в бенчмарке
  отключён: Job Object нельзя переназначить в одном процессе).
