#include <stdint.h>
#include <iostream>
using std::istream;
#include <fstream>
using std::ifstream;
#include <vector>
using std::vector;
#include <algorithm>
using std::max;
using std::pow;
#include <stdexcept>

#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>

#define GLAK_MULTITHREAD
#define GLAK_HANDLE_MAIN
#include <carnifex.h>

#define APP_NAME "OpenGL Demo Application"

#ifndef MAIN_H
#define MAIN_H

struct scene_t
{
    shared_ptr<cfx::referenceFrame> world = make_shared<cfx::referenceFrame>();
    shared_ptr<cfx::referenceFrame> cameraBoom = make_shared<cfx::referenceFrame>();
    shared_ptr<cfx::camera> camera = make_shared<cfx::camera>();
    shared_ptr<cfx::model> obj = make_shared<cfx::model>();
    shared_ptr<cfx::light> light1 = make_shared<cfx::light>();
    shared_ptr<cfx::light> light2 = make_shared<cfx::light>();
    shared_ptr<cfx::light> light3 = make_shared<cfx::light>();
};

struct userData_t
{
    float clearCol[4] = {0.0f, 0.3125f, 0.3125f, 1.0f};

    shared_ptr<glakShader> shader;
    scene_t scene;

    // glm::mat4 projection;
    // glakTransform view;
    // glm::vec2 cameraRot = {0.0f, 0.0f};
    // glm::vec2 cameraRotVel = {0.0f, 0.0f};
    
    uint32_t screenw;
    uint32_t screenh;

    ImGuiIO* io = nullptr;
    ImGuiStyle* style = nullptr;
};

#endif