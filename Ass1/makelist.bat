call "D:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" %1

set OUTDIR=out
set BINDIR=bin
set LIBDIR=lib
set LIBS=SDL2main.lib SDL2.lib
set APP=app.exe

set SOURCES=main libs grad
REM imgui 

REM set imgui_SRC=lib/imgui
REM set imgui_OBJ=imgui.cpp imgui_draw.cpp
REM set imgui_INC=lib/imgui

set libs_SRC=lib
set libs_OBJ=gl3w.c 
REM imgui_impl_sdl_gl3.cpp
set libs_INC=include include/SDL 
REM lib/imgui

set grad_SRC=UCSD
set grad_OBJ=grader.cpp
set grad_INC=include include/SDL

set main_SRC=src
set main_OBJ=main.cpp Geometry.cpp shaders.cpp Transform.cpp
set main_INC=include include/SDL 
REM lib/imgui

goto :eof

:allcpp
for /f %%F in ('dir /b "!%~1!"') do (
    if "%%~xF"==".cpp" set %~2=!%~2! %%F
    if "%%~xF"==".cc" set %~2=!%~2! %%F
    if "%%~xF"==".c" set %~2=!%~2! %%F
)
EXIT /B