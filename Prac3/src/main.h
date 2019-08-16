#include <stdint.h>
#include <iostream>
using std::istream;
#include <fstream>
using std::ifstream;
#include <vector>
using std::vector;
#define LAK_MULTITHREAD
#include "lakmain.h"
#include "glak.h"
#include "imgui_impl_sdl_gl3.h"

#define APP_NAME "OpenGL Application"
#define SDL_MAIN_HANDLED

#ifndef MAIN_H
#define MAIN_H

struct userData_t
{
    float clearCol[4] = {0.0f, 0.3125f, 0.3125f, 1.0f};

    glakShader shader;
    glakObject obj;

    ImGuiIO* io = nullptr;
    ImGuiStyle* style = nullptr;
};

#endif