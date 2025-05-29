@echo off
setlocal

:: Назва файлу для зберігання номера версії збірки
set "VERSION_FILE=build_version.txt"

:: Перевіряємо, чи існує файл версії. Якщо ні, створюємо його з початковим значенням 0.
if not exist "%VERSION_FILE%" (
    echo 0 > "%VERSION_FILE%"
)

:: Читаємо поточний номер версії
set /p BUILD_NUMBER=<"%VERSION_FILE%"

:: Збільшуємо номер версії на 1
set /a BUILD_NUMBER+=1

:: Записуємо оновлений номер версії назад у файл
echo %BUILD_NUMBER% > "%VERSION_FILE%"

:: Отримуємо поточний час у форматі HH:MM:SS
for /f "tokens=1-3 delims=: " %%a in ('echo %time%') do (
    set "current_hour=%%a"
    set "current_minute=%%b"
    set "current_second=%%c"
)
:: Видаляємо мілісекунди, якщо вони присутні (наприклад, 12:34:56.78)
set "current_second=%current_second:~0,2%"

:: Отримуємо поточну дату у форматі DD.MM.YYYY
:: Примітка: Формат дати може відрізнятися в залежності від регіональних налаштувань системи.
:: Цей приклад припускає DD.MM.YYYY або DD/MM/YYYY.
:: Перевірте 'date /t' у командному рядку, щоб побачити свій формат.
:: Для dd.mm.yyyy:
for /f "tokens=1-3 delims=./ " %%a in ('date /t') do (
    set "current_day=%%a"
    set "current_month=%%b"
    set "current_year=%%c"
)

:: Форматуємо дату для BuildDate (DD.MM.YYYY)
set "BuildDateFormatted=%current_day%.%current_month%.%current_year%"

:: Створюємо (або перезаписуємо) файл build.h
> "build.h" echo #ifndef BUILD_H
>> "build.h" echo #define BUILD_H
>> "build.h" echo.
>> "build.h" echo #define BuildTime "%current_hour%:%current_minute%:%current_second%"
>> "build.h" echo #define BuildDate "%BuildDateFormatted%"
>> "build.h" echo #define BuildVersion "1.0.%BUILD_NUMBER%"
>> "build.h" echo #define BuildNumber %BUILD_NUMBER%
>> "build.h" echo.
>> "build.h" echo #endif // BUILD_H

echo build.h згенеровано з версією 1.0.%BUILD_NUMBER%
endlocal