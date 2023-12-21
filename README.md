# zip_unzip

## Безопасный архиватор

Решение должно удовлетворять следующим критериям:
- Решение должно быть написано на языке C со стандартной библиотекой libc;
- Решение должно работать в GNU/Linux;
- Решение должно содержать комментарии, которые помогут проверяющим разобраться в решении;
- Алгоритм архивации можно взять из open source библиотек с открытой лицензией;
- Решение должно успешно собираться компилятором GCC/clang;
- К решению должен быть приложен Makefile файл, который позволит:
  - Собирать решение: make build
  - Запускать тесты для примеров: make test
   - Опционально: Запускать фаззинг тесты, если они есть: make fuzz
   - Опционально: Запускать сканирование статическим анализатором: make analyze

Требуется разработать защищенные программы для архивирования и разархивирования zip/unzip. Программа должна работать в консоли в неинтерактивном режиме. Формат запуска программы: 
- `zip [options] filename [archive.zip]`
- `unzip archive.zip`

Программа должна подмаппировать файл в память, создать в памяти архив и записать на диск. И в обратную сторону – подмаппировать архив, распаковать и записать на диск файл. Маппирование идет чанками, размер чанка - необязательный параметр. Лучше, если программа поддерживает несколько различных алгоритмов архивации.

Сама программа должна включать различные механизмы защиты от взлома. Считайте, что почему-то ей требуется много защит. Напишите в сопроводительной записке, какие опции вы подключили и какие атаки они затрудняют.

Разработка фаззинг тестов, прогон статических анализаторов и отдельные опции для запуска с санитайзерами приветствуются. В этом случае мы ожидаем от вас:
- отчета от прогона стат анализатора
- рабочих и собирающихся fuzzing тестов, запускаемых командой make fuzz
- дополнительных флагов для сборки с санитарами, описанных в readme.md

Обработка ошибок входных аргументов и разработка качественного кода, устойчивого к отказам, обязательна.

## Решение

`make build` - скомпилировать программы zip и unzip

`make test` - запустить выполнение тестов. Тесты исполнены как bash скрипт.
Сначала генерируются тестовые файлы в директории tests/samples.
На вход программе zip подается файл c разными опциями. На выходе получаем 
архив. Сравниваем, что входной и выходной файлы разные утилитой cmp.
Дальше передаем его программе unzip. И сравниваем разархивированный файл с исходным.

`make memory_test` - скомпилировать программы с санитайзерами и прогнать тесты.

`make clean` - очистить проект от объектных и исполняемых файлов.

`make fuzz` - запустить файзинг-тесты. На вход программам подаются некорректные данные.

## Exploit

- Переполнение буфера.
Внимательно слежу за границами массивов (статического и динамических)
- Ошибка форматирующей строки.
Вывожу пользовательские данные с помощью *printf, явно указывая форматную строку.
