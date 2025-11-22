@echo off
rem === Set file paths ===
set "lexer=lexer.cpp"
set "parser=parser.cpp"
set "scope=scope.cpp" 
set "type=type.cpp" 
set "tac=tac.cpp" 
set "main=main.cpp"

rem === Output executable ===
set "output=compiler.exe"

rem === Determine input file (optional) ===
if "%2"=="" (
    set "inputFile=sample"
) else (
    set "inputFile=%2"
)

rem === Check if first argument is "run" (skip compilation) ===
if /i "%1"=="run" (
    if not exist "%output%" (
        echo Executable %output% not found, compiling first...
        g++ "%lexer%" "%parser%" "%scope%" "%type%" "%tac%" "%main%" -o "%output%"
        if errorlevel 1 (
            echo Compilation failed.
            pause
            exit /b 1
        )
    )
    echo Running %output% with input file tester\%inputFile%.txt...
    "%output%" "%inputFile%"
    pause
    exit /b 0
)

rem === Otherwise, compile and run ===
echo Compiling lexer, parser and scope analysis ...
g++ "%lexer%" "%parser%" "%scope%" "%type%" "%tac%" "%main%" -o "%output%"
if errorlevel 1 (
    echo Compilation failed.
    pause
    exit /b 1
)

echo Running %output% with input file tester\%inputFile%.txt...
"%output%" "%inputFile%"
pause
