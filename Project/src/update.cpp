#include "main.h"
#include "shaders.h"

#ifndef DEBUG
#   ifdef NDEBUG
#       define DEBUG(x)
#   else
#       define DEBUG(x) std::cout << __FILE__ << "(" << std::dec << __LINE__ << ") " << x
#   endif
#endif

enum state_t { STARTUP, RUNNING, WIN, LOSS };
state_t state = STARTUP;

void modelFromOBJ(istream& strm, shared_ptr<model_t> model)
{
    if(model->mesh.use_count() < 1) model->mesh = make_shared<lak::mesh_t>();
    vector<glm::vec3> verts, uvw, norms;
    vector<vector<tuple<size_t, size_t, size_t>>> indices;
    size_t vertcount = 0;
    lak::readOBJ(strm, &verts, &uvw, &norms, &indices, &vertcount);

    lak::mesh_t::element_t& vpos = model->mesh->elements["vPosition"];
    glm::vec4* vposp = vpos.init<glm::vec4>(vertcount);
    vpos.normalized = false;
    vpos.active = true;

    lak::mesh_t::element_t& vtex = model->mesh->elements["vTexCoord"];
    glm::vec4* vtexp = vtex.init<glm::vec4>(vertcount);
    vtex.normalized = false;
    vtex.active = true;

    lak::mesh_t::element_t& norm = model->mesh->elements["vNormal"];
    glm::vec4* normp = norm.init<glm::vec4>(vertcount);
    norm.normalized = true;
    norm.active = true;

    for(size_t i = 0; i < indices.size(); i++)
    {
        for(size_t j = 0; j < 3; j++)
        {
            vposp[(i*3)+j] = glm::vec4(verts[std::get<0>(indices[i][j])], 1.0f);
            vtexp[(i*3)+j] = glm::vec4(uvw[std::get<1>(indices[i][j])], 1.0f);
            normp[(i*3)+j] = glm::vec4(norms[std::get<2>(indices[i][j])], 1.0f);
        }
    }
}
    
/*
* model:       model transform     model -> world      updated on model change
* view:        camera transform    world -> camera     updated on camera change
* projection:  screen transform    camera -> screen    updated on screen change
* screenPosition = projection * view * model * vertPosition
*/

///
/// update()
/// Called every loop
///
void update(lak::loopData* ld)
{
    // Get the user data
    userData_t& ud = ld->userData;

    // Check events
    for(auto it = ld->events.begin(); it != ld->events.end(); it++)
    {
        switch(it->type)
        {
            case SDL_QUIT: {
                ld->running = false;
            } break;
            case SDL_WINDOWEVENT: {
                if(it->window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    ud.screenw = it->window.data1;
                    ud.screenh = it->window.data2;
                    glViewport(0, 0, ud.screenw, ud.screenh);
                    ud.scene.camera->projection = glm::perspective(90.0f * glm::pi<float>() / 180.0f, (float)ud.screenw / (float)ud.screenh, 0.01f, 100.0f);
                }
            } break;
            case SDL_KEYDOWN: {
                switch(it->key.keysym.scancode)
                {
                    case SDL_SCANCODE_LEFT: {
                        // ud.scene.player->transform.rotVel.y = 0.5;
                        ud.scene.player->transform.rotVel.y = 2;
                    } break;
                    case SDL_SCANCODE_UP: {
                        ud.scene.ball->transform.rotAccel.x = 3;
                    } break;
                    case SDL_SCANCODE_RIGHT: {
                        // ud.scene.player->transform.rotVel.y = -0.5;
                        ud.scene.player->transform.rotVel.y = -2;
                    } break;
                    case SDL_SCANCODE_DOWN: {
                        ud.scene.ball->transform.rotAccel.x = -3;
                    } break;
                    case SDL_SCANCODE_ESCAPE: {
                        exit(EXIT_SUCCESS);
                    } break;
                }
            } break;
            case SDL_KEYUP: {
                switch(it->key.keysym.scancode)
                {
                    case SDL_SCANCODE_LEFT:
                    case SDL_SCANCODE_RIGHT: {
                        ud.scene.player->transform.rotVel.y = 0;
                    } break;
                    case SDL_SCANCODE_UP:
                    case SDL_SCANCODE_DOWN: {
                        ud.scene.ball->transform.rotAccel.x = 0;
                    } break;
                }
            } break;
        }
        ImGui_ImplSdlGL3_ProcessEvent(&*it);
    }
    ld->events.clear();

    // Start a new GUI frame
    ImGui::NewFrame();

    switch(state)
    {
        case STARTUP: {
            // Make sure we have OpenGL context in this thread
            if(!ld->updateHasContext) 
            {
                ld->updateNeedsContext = true;
                break;
            }
            // setup shader
            ud.scene.shader = make_shared<lak::shader_t>();
            ud.scene.shader->init(vshaderstr, fshaderstr);

            ud.scene.playerPos->addChild(ud.scene.player);
            ud.scene.player->addChild(ud.scene.cameraBoom);

            // setup camera view
            ud.scene.cameraBoom->addChild(ud.scene.camera);
            ud.scene.camera->transform.clear();

            ud.scene.cameraBoom->transform.rot.x = 0.9f;
            ud.scene.cameraBoom->transform.pos.z = -2.2f;
            ud.scene.camera->transform.rot.x = -0.3f;

            ud.scene.shader->setUniform("view", &ud.scene.camera->getTransform());

            // setup camera perspective
            ud.scene.camera->projection = glm::perspective(glm::pi<float>() / 2.0f, (float)ud.screenw / (float)ud.screenh, 0.01f, 100.0f);
            ud.scene.shader->setUniform("projection", &ud.scene.camera->projection);

            // setup ball model
            ud.scene.ball = make_shared<model_t>();
            ud.scene.ball->mesh = make_shared<lak::mesh_t>();
            ud.scene.ball->mesh->shader = ud.scene.shader;
            ud.scene.player->addChild(ud.scene.ball);

            // setup ball texture
            ifstream balltexstrm("ball.ppm", std::ios::binary|ifstream::in);
            if(balltexstrm.is_open())
            {
                lak::imageRGBA8_t ballimg = lak::readPNM<lak::imageRGBA8_t>(balltexstrm);
                shared_ptr<lak::texture_t> balltex = make_shared<lak::texture_t>();
                balltex->generate(GL_TEXTURE_2D, 0, GL_RGBA, 0, ballimg, {
                    lak::texparam_t(GL_TEXTURE_MIN_FILTER, GL_LINEAR),
                    lak::texparam_t(GL_TEXTURE_MAG_FILTER, GL_LINEAR),
                    lak::texparam_t(GL_TEXTURE_WRAP_S, GL_REPEAT),
                    lak::texparam_t(GL_TEXTURE_WRAP_T, GL_REPEAT)
                });
                ud.scene.ball->mesh->textures["albedo"] = balltex;
            } else DEBUG("Failed to open ball.ppm" << endl);

            // get ball model from file
            ifstream ballstrm("ball.obj", std::ios::binary|ifstream::in);
            if(ballstrm.is_open())
            {
                modelFromOBJ(ballstrm, ud.scene.ball);
                ud.scene.ball->update();
            } else DEBUG("Failed to open ball.obj" << endl);

            ifstream mapstrm("map.ppm", std::ios::binary|ifstream::in);
            if(mapstrm.is_open())
            {
                lak::imageRGBA8_t mapimg = lak::readPNM<lak::imageRGBA8_t>(mapstrm);

                ifstream cubetexstrm("cube.ppm", std::ios::binary|ifstream::in);
                ifstream cubestrm("cube.obj", std::ios::binary|ifstream::in);
                if(cubetexstrm.is_open() && cubestrm.is_open())
                {
                    lak::imageRGBA8_t cubeimg = lak::readPNM<lak::imageRGBA8_t>(cubetexstrm);
                    shared_ptr<lak::texture_t> cubetex = make_shared<lak::texture_t>();
                    cubetex->generate(GL_TEXTURE_2D, 0, GL_RGBA, 0, cubeimg, {
                        lak::texparam_t(GL_TEXTURE_MIN_FILTER, GL_LINEAR),
                        lak::texparam_t(GL_TEXTURE_MAG_FILTER, GL_LINEAR),
                        lak::texparam_t(GL_TEXTURE_WRAP_S, GL_REPEAT),
                        lak::texparam_t(GL_TEXTURE_WRAP_T, GL_REPEAT)
                    });

                    shared_ptr<model_t> blk = make_shared<model_t>();
                    blk->mesh = make_shared<lak::mesh_t>();
                    modelFromOBJ(cubestrm, blk);
                    blk->mesh->shader = ud.scene.shader;
                    blk->mesh->textures["albedo"] = cubetex;

                    ud.scene.blocks.clear(); ud.scene.blocks.reserve(mapimg.w * mapimg.h);
                    for(size_t x = 0; x < mapimg.w; x++)
                    {
                        for(size_t y = 0; y < mapimg.h; y++)
                        {
                            if(mapimg[y][x].r > 0)
                            {
                                shared_ptr<model_t> block = make_shared<model_t>();
                                block->mesh = blk->mesh;
                                ud.scene.blocks.push_back(block);
                                block->transform.pos = {x * 2.0f, -2.0f, y * 2.0f};
                                block->update();
                            }
                        }
                    }
                    ud.scene.blocks.shrink_to_fit();
                }

                ifstream cointexstrm("coin.ppm", std::ios::binary|ifstream::in);
                ifstream coinstrm("coin.obj", std::ios::binary|ifstream::in);
                if(cointexstrm.is_open() && coinstrm.is_open())
                {
                    lak::imageRGBA8_t coinimg = lak::readPNM<lak::imageRGBA8_t>(cointexstrm);
                    shared_ptr<lak::texture_t> cointex = make_shared<lak::texture_t>();
                    cointex->generate(GL_TEXTURE_2D, 0, GL_RGBA, 0, coinimg, {
                        lak::texparam_t(GL_TEXTURE_MIN_FILTER, GL_LINEAR),
                        lak::texparam_t(GL_TEXTURE_MAG_FILTER, GL_LINEAR),
                        lak::texparam_t(GL_TEXTURE_WRAP_S, GL_REPEAT),
                        lak::texparam_t(GL_TEXTURE_WRAP_T, GL_REPEAT)
                    });
                    
                    shared_ptr<model_t> _coin = make_shared<model_t>();
                    _coin->mesh = make_shared<lak::mesh_t>();
                    modelFromOBJ(coinstrm, _coin);
                    _coin->mesh->shader = ud.scene.shader;
                    _coin->mesh->textures["albedo"] = cointex;

                    ud.scene.coins.clear(); ud.scene.coins.reserve(mapimg.w * mapimg.h);
                    for(size_t x = 0; x < mapimg.w; x++)
                    {
                        for(size_t y = 0; y < mapimg.h; y++)
                        {
                            if(mapimg[y][x].g > 0)
                            {
                                shared_ptr<model_t> coin = make_shared<model_t>();
                                coin->mesh = _coin->mesh;
                                ud.scene.coins.push_back(coin);
                                coin->transform.pos = {x * 2.0f, 0.0f, y * 2.0f};
                                coin->transform.rotVel.y = 1.0f;
                                coin->update();
                            }
                        }
                    }
                    ud.scene.coins.shrink_to_fit();
                    ud.scene.coins_reset = ud.scene.coins;
                }

                ud.scene.lights.clear();
                for(size_t x = 0; x < mapimg.w; x++)
                {
                    for(size_t y = 0; y < mapimg.h; y++)
                    {
                        if(mapimg[y][x].b > 0)
                        {
                            light_t light;
                            light.transform.pos = {x * 2.0f, 2.0f, y * 2.0f};
                            ud.scene.lights.push_back(light);
                        }
                    }
                }
                ud.scene.lights.shrink_to_fit();
            } else DEBUG("Failed to cube.ppm or open map.ppm or cube.obj" << endl);

            // setup lights
            size_t lightCount = ud.scene.lights.size();
            if(lightCount > 6) lightCount = 6;
            ud.scene.shader->setUniform("lightCount", &lightCount);
            for(size_t i = 0; i < lightCount; i++)
            {
                string lightname = (string)"lights[" + (char)('0' + i) + (string)"]";
                ud.scene.shader->setUniform(lightname+".position", &ud.scene.lights[i].transform.pos);
                ud.scene.shader->setUniform(lightname+".color", &ud.scene.lights[i].color);
            }
            ud.scene.shader->setUniform("ambient", &glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
            ud.scene.shader->setUniform("diffuse", &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            ud.scene.shader->setUniform("specular", &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            float shininess = 100.0f;
            ud.scene.shader->setUniform("shininess", &shininess);
            ud.scene.shader->setUniform("model", &glm::mat4(1.0f));

            // We don't need the OpenGL context in this thread anymore
            ld->updateNeedsContext = false;
            // Change state
            state = RUNNING;
        } break;
        case RUNNING: {
            auto& scene = ud.scene;

            scene.playerPos->transform.posVel.x = sin(scene.player->transform.rot.y) * scene.ball->transform.rotVel.x;
            scene.playerPos->transform.posVel.z = cos(scene.player->transform.rot.y) * scene.ball->transform.rotVel.x;

            scene.world->transform.update(ld->updateDelta);
            scene.playerPos->transform.update(ld->updateDelta);
            scene.player->transform.update(ld->updateDelta);
            scene.cameraBoom->transform.update(ld->updateDelta);
            scene.camera->transform.update(ld->updateDelta);

            for(auto& light : scene.lights)
                light.transform.update(ld->updateDelta);
            for(auto& block : scene.blocks)
                block->transform.update(ld->updateDelta);
            for(auto& coin : scene.coins)
                coin->transform.update(ld->updateDelta);
            
            scene.ball->transform.update(ld->updateDelta);

            for(auto it = scene.coins.begin(); it != scene.coins.end();)
            {
                glm::vec3 dist = (*it)->transform.pos - scene.playerPos->transform.pos;
                float dst = sqrt((dist.x*dist.x) + (dist.z*dist.z));
                if(dst < 1.0f)
                {
                    it = scene.coins.erase(it);
                }
                else ++it;
            }
            if(ud.scene.coins.empty()) state = WIN;

            bool onTrack = false;
            // for(auto it = ud.scene.blocks.begin(); it != ud.scene.blocks.end(); it++)
            for(auto& block : scene.blocks)
            {
                glm::vec3 dist = block->transform.pos - scene.playerPos->transform.pos;
                onTrack |= abs(dist.x) <= 1.0f && abs(dist.z) <= 1.0f;
            }
            if(!onTrack) state = LOSS;

            ImGui::Text("Score");
            ImGui::Text("%x", ud.scene.coins_reset.size() - ud.scene.coins.size());
        } break;
        case WIN: {
            auto& scene = ud.scene;

            ImGui::Text("WINNER WINNER CHICKEN DINNER!");
            if(ImGui::Button("Restart"))
            {
                scene.playerPos->transform.pos = {0.0f, 0.0f, 0.0f};
                scene.playerPos->transform.posVel = {0.0f, 0.0f, 0.0f};
                scene.player->transform.rot = {0.0f, 0.0f, 0.0f};
                scene.player->transform.rotVel = {0.0f, 0.0f, 0.0f};
                scene.ball->transform.rot = {0.0f, 0.0f, 0.0f};
                scene.ball->transform.rotVel = {0.0f, 0.0f, 0.0f};
                scene.coins = scene.coins_reset;
                state = RUNNING;
            }
        } break;
        case LOSS: {
            auto& scene = ud.scene;

            ImGui::Text("Better luck next time");
            if(ImGui::Button("Restart"))
            {
                scene.playerPos->transform.pos = {0.0f, 0.0f, 0.0f};
                scene.playerPos->transform.posVel = {0.0f, 0.0f, 0.0f};
                scene.player->transform.rot = {0.0f, 0.0f, 0.0f};
                scene.player->transform.rotVel = {0.0f, 0.0f, 0.0f};
                scene.ball->transform.rot = {0.0f, 0.0f, 0.0f};
                scene.ball->transform.rotVel = {0.0f, 0.0f, 0.0f};
                scene.coins = scene.coins_reset;
                state = RUNNING;
            }
        } break;
    }

    /*if(ImGui::TreeNode("Player Pos"))
    {
        lak::TransformView(ud.scene.playerPos);
        ImGui::TreePop();
    }
    if(ImGui::TreeNode("Player"))
    {
        lak::TransformView(ud.scene.player);
        ImGui::TreePop();
    }
    if(ImGui::TreeNode("Boom"))
    {
        lak::TransformView(ud.scene.cameraBoom);
        ImGui::TreePop();
    }
    if(ImGui::TreeNode("Camera"))
    {
        lak::TransformView(static_cast<lak::referenceFrame_t*>(ud.scene.camera.get()));
        ImGui::TreePop();
    }
    if(ImGui::TreeNode("Ball"))
    {
        lak::TransformView(static_cast<lak::referenceFrame_t*>(ud.scene.ball.get()));
        ImGui::TreePop();
    }
    if(ImGui::TreeNode("Frame"))
    {
        lak::TransformView(ud.scene.frame);
        ImGui::TreePop();
    }
    if(ImGui::TreeNode("Cubes"))
    {
        for(auto it = ud.scene.blocks.begin(); it != ud.scene.blocks.end(); it++)
            lak::TransformView(&(*it)->transform);
        ImGui::TreePop();
    }*/

    ImGui::Text("%f", ld->updateDelta);
    ImGui::Text("%f", ld->drawDelta);

    // ud.scene.gizmos[0].update((ud.scene.cameraBoom->parent == nullptr ? lak::transform_t::IDEN : ud.scene.cameraBoom->parent->getTransform()), ud.scene.cameraBoom->getTransform());
    // ud.scene.gizmos[1].update((ud.scene.camera->parent == nullptr ? lak::transform_t::IDEN : ud.scene.camera->parent->getTransform()), ud.scene.camera->getTransform());
    // ud.scene.gizmos[2].update((ud.scene.obj->parent == nullptr ? lak::transform_t::IDEN : ud.scene.obj->parent->getTransform()), ud.scene.obj->getTransform());

    // ud.scene.gizmos[0].update(ud.scene.cameraBoom->getTransform());
    // ud.scene.gizmos[1].update(ud.scene.camera->getTransform());
    // ud.scene.gizmos[2].update(ud.scene.ball->getTransform());
    // ud.scene.gizmos[3].update(ud.scene.frame->getTransform());
    // ud.scene.gizmos[3].mesh.shader->setUniform("model", &ud.scene.cameraBoom->getTransform()[0][0]);

    // static bool is not reset every time loop() is called, effectively global variable but cannot be accessed outside of the loop() scope
    // static bool rightMenuOpen = true;
    // Check if ImGui wants to use the mouse
    // if(!ud.io->WantCaptureMouse)
    // {
    //     // If ImGui is not using the mouse, check if the right button has been clicked
    //     if(ud.io->MouseClicked[1])
    //     {
    //         cout << "right mouse clicked\n";
    //         // Toggle rightMenuOpen
    //         rightMenuOpen = !rightMenuOpen;
    //     }
    // }
    // Check if ImGui wants to use the keyboard
    // if(!ud.io->WantCaptureKeyboard)
    // {

    // }
    
    // If rightMenuOpen draw a small menu
    // if(rightMenuOpen)
    // {
    //     // Set the window pos to the mouse pos
    //     ImGui::SetNextWindowPos(ImVec2(ud.io->MouseClickedPos[1][0] - 2, ud.io->MouseClickedPos[1][1] - 2), ImGuiCond_Appearing);
    //     // Draw a window with no boarder or menu
    //     if(ImGui::Begin("Right Click Menu", &rightMenuOpen, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_AlwaysAutoResize))
    //     {
    //         ImGui::Text("Draw Time %d", (size_t)std::ceil(1.0f / ld->drawDelta));
    //         ImGui::Text("Update Time %f", ld->updateDelta * 1000.0);
    //     }
    //     // End the window
    //     ImGui::End();
    // }
    ImGui::Render();
}