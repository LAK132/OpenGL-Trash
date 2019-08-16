#include <stdint.h>
#include <iostream>
using std::istream;
#include <fstream>
using std::ifstream;
#include <vector>
using std::vector;
#include <tuple>
using std::tuple;
#include <memory>
using std::shared_ptr;
using std::make_shared;
#include <string>
using std::string;
#include <iostream>
using std::cin;
using std::cout;
using std::endl;
#include <cassert>

#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>
#include <imguisplitter.hpp>

using std::default_delete;

#define APP_NAME "OpenGL Demo Application"

#include <lak/image.hpp>
#include <lak/texture.hpp>
#include <lak/pnm.hpp>
#include <lak/stream_util.hpp>
#define LAK_SPACE_IMGUI
#include <lak/space.hpp>
#include <lak/graphics.hpp>
#include <lak/gizmo.hpp>
#include <lak/obj.hpp>

#include <glm/geometric.hpp>

#ifndef MAIN_H
#define MAIN_H

struct model_t : public lak::referenceFrame_t
{
    shared_ptr<lak::mesh_t> mesh;
    string modelUniformName = "model";
    void update() { mesh->update(); }
    void draw(bool reversed = false) { 
        mesh->shader->setUniform(modelUniformName, &getTransform(false, reversed)[0][0]);
        mesh->draw(); 
    }
};

struct light_t : public lak::referenceFrame_t
{
    glm::vec4 color = {0.5f, 0.5f, 0.5f, 1.0f};
};

struct camera_t : public lak::referenceFrame_t
{
    glm::mat4 projection;  
    glm::mat4 view;
    glm::mat4& update() {
        glm::mat4& trans = getTransform();
        glm::vec3& eye = glm::vec3(trans * lak::transform_t::WUP);
        glm::vec3& center = glm::vec3(trans * (lak::transform_t::ZUP + lak::transform_t::WUP));
        glm::vec3& up = glm::vec3(trans * lak::transform_t::YUP);
        view = glm::lookAt(eye, center, up);
        return view;
    }
};

struct scene_t
{
    shared_ptr<lak::shader_t> shader;
    shared_ptr<lak::referenceFrame_t> world = make_shared<lak::referenceFrame_t>();
    shared_ptr<lak::referenceFrame_t> playerPos = make_shared<lak::referenceFrame_t>();
    shared_ptr<lak::referenceFrame_t> player = make_shared<lak::referenceFrame_t>();
    shared_ptr<lak::referenceFrame_t> cameraBoom = make_shared<lak::referenceFrame_t>();
    shared_ptr<lak::referenceFrame_t> frame = make_shared<lak::referenceFrame_t>();
    shared_ptr<camera_t> camera = make_shared<camera_t>();
    vector<light_t> lights;
    vector<shared_ptr<model_t>> blocks;
    vector<shared_ptr<model_t>> coins;
    vector<shared_ptr<model_t>> coins_reset;
    shared_ptr<model_t> ball;

    vector<lak::gizmo_t> gizmos;
    // void draw() { 
    //     ball->draw();
    //     for(auto it = blocks.begin(); it != blocks.end(); it++)
    //         (*it)->draw();
    //     for(auto it = coins.begin(); it != coins.end(); it++)
    //         (*it)->draw(true);
    // }
    // void update(float delta)
    // {
    //     world->transform.update(delta);
    //     playerPos->transform.update(delta);
    //     player->transform.update(delta);
    //     cameraBoom->transform.update(delta);
    //     camera->transform.update(delta);
    //     for(auto it = lights.begin(); it != lights.end(); it++)
    //         it->transform.update(delta);
    //     for(auto it = blocks.begin(); it != blocks.end(); it++)
    //         (*it)->transform.update(delta);
    //     for(auto it = coins.begin(); it != coins.end(); it++)
    //         (*it)->transform.update(delta);
    //     ball->transform.update(delta);
    // }
};

struct userData_t
{
    float clearCol[4] = {0.0f, 0.3125f, 0.3125f, 1.0f};

    scene_t scene;
    
    uint32_t screenw;
    uint32_t screenh;

    ImGuiIO* io = nullptr;
    ImGuiStyle* style = nullptr;

    // shared_ptr<lak::imageRGBA8_t> image;
    // shared_ptr<lak::texture_t> texture;
    // shared_ptr<lak::shader_t> shader;
};

#define LAK_MAIN_MULTITHREAD
#define LAK_MAIN_SDL
#define LAK_MAIN_USERDATATYPE userData_t
#include <lak/main.hpp>

#endif