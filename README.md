# RegistrationGenerator (Qt) 

Графическое приложение на Qt Widgets (C++/CMake) для генерации бинарных .dat-файлов с сообщениями фиксированной длины и равномерным интервалом по времени.

Сделано с любовью

## Возможности
	•	Генерация регистрационного файла:
	•	запись заголовка TRegFileHdr (маркер, версия, время и пр.);
	•	серия сообщений TUMsgHdr + body c равномерным шагом по времени;
	•	авто-выравнивание записи до 4 байт.
	•	Режимы содержимого тела сообщения:
	1.	нули;
	2.	случайные байты 0/1;
	3.	повтор заданной строки (UTF-8).
	•	Два режима задания количества сообщений:
	•	вручную: MINAMOUNTMSG / MAXAMOUNMSG;
	•	«Заполнить интервал» — вычисляется из TIMEGAPMIN/TIMEGAPMAX/TIMEINTERVAL.
	•	Быстрые валидаторы полей и подсказки об ошибках.

## Стек
	•	Qt 6 (или Qt 5) — Widgets
	•	CMake (Ninja рекомендуем)
	•	C++17

## Быстрый старт

### macOS (Homebrew, Apple Silicon)

brew install qt ninja ccache
rm -rf build
/opt/homebrew/opt/qt/bin/qt-cmake -S . -B build -G Ninja \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

#### Сборка
cmake --build build --config Release

#### Запуск
```bash
open build/RegistrationGenerator.app
```
Для Intel Mac путь к qt-cmake: ``/usr/local/opt/qt/bin/qt-cmake.``

### Linux

#### Qt 6 из дистрибутива/инсталлятора
```bash
sudo apt install qt6-base-dev ninja-build ccache
qt-cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
cmake --build build --config Release
./build/RegistrationGenerator
```
### Windows (MSVC + Qt Installer)

Открой x64 Native Tools for VS:
```bash
set QT=C:\Qt\6.x.x\msvc2022_64
"%QT%\bin\qt-cmake.exe" -S . -B build -G "Ninja"
cmake --build build --config Release
build\Release\RegistrationGenerator.exe
```
##### Как пользоваться (UI):
- Задайте временной интервал:
  - TIMEGAPMIN, TIMEGAPMAX — границы в формате HH:mm:ss;
  - TIMEINTERVAL — шаг между сообщениями.
- Выберите режим количества:
  - галочка FILLINTERVAL — количество вычислится автоматически;
  - иначе введите MINAMOUNTMSG и MAXAMOUNMSG (не больше показанного DMAXAMOUNTMSG).
- Задайте длину тела сообщения LENTHMSG (байт).
  - Выберите тип содержимого:
  - «нули» / «случайные 0/1» / «повтор строки» (для последнего введите текст).
- Нажмите CREATEFILE, выберите путь сохранения (*.dat).

- Формат файла (коротко)
  - TRegFileHdr → затем N сообщений.
  - Каждое сообщение: TUMsgHdr (ровно 24 байта, static_assert) + тело msgLength - 24 с выравниванием до 4 байт.
  - Время сообщений растёт с равным шагом от TIMEGAPMIN к TIMEGAPMAX.

- Сборка быстрее
  - Генератор Ninja: -G Ninja
	-	Кэш компиляции: -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
	-	Инкрементная сборка: не удаляйте build/ зря
	-	(опционально) Unity/PCH:

## Типичные ошибки и решения
`` The application cannot be opened because its executable is missing. `` \
Сборка упала раньше. Проверьте вывод cmake --build ... — чаще всего проблема в путях к исходникам.
`` AutoUic error: could not find mainwindow.ui. `` \
Убедитесь, что в CMakeLists.txt указан правильный путь: \
  `` src/mainwindow/mainwindow.ui ``, и включён `` set(CMAKE_AUTOUIC ON) `` .
`` fatal error: ‘something.h’ file not found. `` \
Не используйте абсолютные инклюды вида #include "/.../structures.h". \
Пишите #include "structures.h" и добавьте папки через:
```cmake
target_include_directories(RegistrationGenerator PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/mainwindow
    ${CMAKE_SOURCE_DIR}/src/generator
    ${CMAKE_SOURCE_DIR}/src/structures
    ${CMAKE_SOURCE_DIR}/src/varstorage
)
```

	•	CMake: Cannot find source file …/varstorage.cpp
Проверьте путь в списке источников — у нас src/varstorage/varstorage.cpp.

.gitignore (рекомендуется)

# CMake/Ninja
/build*/
CMakeFiles/
CMakeCache.txt
*.ninja
*.ninja_log
*.ninja_deps
Makefile

# Qt автогенерация
moc_*.cpp
ui_*.h
qrc_*.cpp

# Бинарники/артефакты
*.o *.obj *.a *.lib *.so *.dylib *.dll
*.exe *.app
.DS_Store

Лицензия

MIT (или укажи свою).

⸻

если хочешь, сенпай, я добавлю в репо этот README.md, сгенерю CMakePresets.json, и настрою GitHub Actions, чтобы релизные .app/.zip собирались автоматически, nya~ ✨
