call "D:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" %1

set OUTDIR=out
set BINDIR=bin
set LIBDIR=lib
set LIBS=SDL2main.lib SDL2.lib
set APP=app.exe

set SOURCES=main libs

set libs_SRC=lib
set libs_OBJ=gl3w.c grader.cpp
set libs_INC=include include/SDL

set main_SRC=src
set main_OBJ=main.cpp Transform.cpp shaders.cpp readfile.cpp Geometry.cpp display.cpp
set main_INC=include include/SDL

goto :eof

:allcpp
for /f %%F in ('dir /b "!%~1!"') do (
    if "%%~xF"==".cpp" set %~2=!%~2! %%F
    if "%%~xF"==".cc" set %~2=!%~2! %%F
    if "%%~xF"==".c" set %~2=!%~2! %%F
)
EXIT /B