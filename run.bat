@echo off
rem === Set file paths ===
set "file1=lexer.cpp"
set "file2=parser.cpp"

rem === Set output names ===
set "out1=first.exe"
set "out2=second.exe"

rem === Compile first file ===
echo Compiling %file1%
g++ "%file1%" -o "%out1%"
if errorlevel 1 (
    echo Failed to compile %file1%
    pause
    exit /b 1
)

rem === Run first program ===
echo Running %out1%
"%out1%"
echo.

rem === Compile second file ===
echo Compiling %file2%
g++ "%file2%" -o "%out2%"
if errorlevel 1 (
    echo Failed to compile %file2%
    pause
    exit /b 1
)

rem === Run second program ===
echo Running %out2%
"%out2%"
echo.