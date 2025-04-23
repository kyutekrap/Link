@echo off

:: Remove old object files and executable (if they exist)
del /q *.o
del /q program.exe

:: Compile source files into object files
gcc -c transpiler\transpiler.c -o transpiler.o
gcc -c transpiler\helper.c -o helper.o
gcc -c debugger\debugger.c -o debugger.o
gcc -c utils\utils.c -o utils.o
gcc -c flow\flow.c -o flow.o
gcc -c step\step.c -o step.o
gcc -c main.c -o main.o

:: Link object files to create the final executable
gcc -o program main.o transpiler.o debugger.o utils.o flow.o step.o helper.o

:: Check if compilation was successful
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

echo Compilation successful! Running program...
echo ----------------------------------------
program