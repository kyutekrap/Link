@echo off

:: Remove old object files and executable (if they exist)
del /q *.o
del /q program.exe

:: Compile source files into object files
gcc -c transpiler\transpiler.c -o transpiler.o
gcc -c debugger\debugger.c -o debugger.o
gcc -c main.c -o main.o

:: Link object files to create the final executable
gcc -o program main.o transpiler.o debugger.o

:: Check if compilation was successful
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

echo Compilation successful! Running program...
echo ----------------------------------------
program