@echo off

:: Remove old object files and executable (if they exist)
del /q *.o
del /q program.exe

:: Compile source files into object files
gcc -c src\checkfile.c -o checkfile.o
gcc -c src\readenv.c -o readenv.o
gcc -c src\getmacro.c -o getmacro.o
gcc -c src\mkfile.c -o mkfile.o
gcc -c src\readinc.c -o readinc.o
gcc -c src\lexer.c -o lexer.o
gcc -c main.c -o main.o

:: Link object files to create the final executable
gcc -o program main.o checkfile.o readenv.o getmacro.o mkfile.o readinc.o lexer.o

:: Check if compilation was successful
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

echo Compilation successful! Running program...
echo ----------------------------------------
program