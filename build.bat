@echo off

:: Remove old object files and executable (if they exist)
del /q *.o
del /q program.exe

:: Compile source files into object files
gcc -c src\checkfile.c -o checkfile.o
gcc -c src\getmacro.c -o getmacro.o
gcc -c src\delfile.c -o delfile.o
gcc -c src\isempty.c -o isempty.o
gcc -c src\getfiles.c -o getfiles.o
gcc -c src\getdeco.c -o getdeco.o
gcc -c src\getcomm.c -o getcomm.o
gcc -c src\lexer.c -o lexer.o
gcc -c main.c -o main.o

:: Link object files to create the final executable
gcc -o program main.o checkfile.o getmacro.o delfile.o isempty.o getfiles.o getdeco.o getcomm.o lexer.o

:: Check if compilation was successful
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

echo Compilation successful! Running program...
echo ----------------------------------------
program