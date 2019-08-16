#include "main.h"

int lastMouse = 0;

// void trackball_ptov(int x, int y, int width, int height, glm::vec3& v)
// {
//     v[0] = ((2.0*x) - width) / width;
//     v[1] = (height - (2.0*y)) / height;
//     float d = sqrt(v[0]*v[0] + v[1]*v[1]);

//     v[2] = cos((M_PI/2) * (d < 1.0 ? d : 1.0));
//     float a = 1.0 / sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);

//     v[0] *= a;
//     v[1] *= a;
//     v[2] *= a;
// }

// bool mouseClickReset = true;
// void mouseMotion(int x, int y, userData_t* ud)
// {
//     static glm::vec3 lastPos;
//     glm::vec3 curPos;
//     trackball_ptov(x, y, (int)ud->io->DisplaySize.x, (int)ud->io->DisplaySize.y, curPos);
//     if(mouseClickReset) lastPos = curPos;
//     glm::vec3 dv = curPos - lastPos;
//     float len = glm::length2(dv);
//     if(len > 0.0)
//     {
//         float ang = 90.0 * len;
//         glm::vec3 axis(
//             lastPos.y*curPos.z - lastPos.z*curPos.y,
//             lastPos.z*curPos.x - lastPos.x*curPos.z,
//             lastPos.x*curPos.y - lastPos.y*curPos.x
//         );
//         cout << axis.x << endl << axis.y << endl << axis.z << endl << ang << endl << endl;
//         lastPos = curPos;
//         auto t = glm::inverse(ud->obj.transform) * glm::vec4(-axis.x, -axis.y, axis.z, 1.0f);
//         ud->obj.transform = glm::rotate(ud->obj.transform, ang, {t.x, t.y, t.z});
//     }
// }

///
/// loop()
/// Called every loop
///
void update(atomic_bool* run, SDL_Window** window, double deltaTime, void** userDataPtr)
{
    userData_t*& ud = *(userData_t**)userDataPtr;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSdlGL3_ProcessEvent(&event);
        if (event.type == SDL_QUIT) *run = false;
    }
    ImGui_ImplSdlGL3_NewFrame(*window);

    // static bool is not reset every time loop() is called, effectively global variable but cannot be accessed outside of the loop() scope
    static bool rightMenuOpen = true;
    // Check if ImGui wants to use the mouse
    if(!ud->io->WantCaptureMouse)
    {
        if(ud->io->MouseClicked[0])
        {
            lastMouse = 0;
        }
        if(ud->io->MouseClicked[1])
        {
            lastMouse = 1;
        }
        if(ud->io->MouseClicked[2])
        {
            lastMouse = 2;
        }
    }
    // Check if ImGui wants to use the keyboard
    if(!ud->io->WantCaptureKeyboard)
    {
        glm::vec4 worldaxis;
        switch(lastMouse)
        {
            case 0:{
                worldaxis = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            } break;
            case 1:{
                worldaxis = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
            } break;
            case 2:{
                worldaxis = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
            } break;
        }
        if(ud->io->KeysDown[SDL_SCANCODE_LEFT])
        {
            ud->obj.transform.addTranslation(-0.01f * glm::vec3(worldaxis.x, worldaxis.y, worldaxis.z)).append(true);
        }
        if(ud->io->KeysDown[SDL_SCANCODE_RIGHT])
        {
            ud->obj.transform.addTranslation(0.01f * glm::vec3(worldaxis.x, worldaxis.y, worldaxis.z)).append(true);
        }
        if(ud->io->KeysDown[SDL_SCANCODE_UP])
        {
            auto axis = 0.01f * worldaxis;
            ud->obj.transform.addScale({axis.x, axis.y, axis.z}).append(true);
        }
        if(ud->io->KeysDown[SDL_SCANCODE_DOWN])
        {
            auto axis = -0.01f * worldaxis;
            ud->obj.transform.addScale({axis.x, axis.y, axis.z}).append(true);
        }
    }
    
    // If rightMenuOpen draw a small menu
    if(rightMenuOpen)
    {
        // Set the window pos to the mouse pos
        ImGui::SetNextWindowPos(ImVec2(ud->io->MouseClickedPos[1][0] - 2, ud->io->MouseClickedPos[1][1] - 2), ImGuiCond_Appearing);
        // Draw a window with no boarder or menu
        if(ImGui::Begin("Right Click Menu", &rightMenuOpen, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Delta Time %f", deltaTime);
            // Draw the library credits
            glakCredits();
        }
        // End the window
        ImGui::End();
    }
    ImGui::Render();

    switch(lastMouse)
    {
        case 0:{
            /* Checkpoint 1: rotate mesh
            for(auto it = ud->obj.mesh[0].vertex.begin(); it != ud->obj.mesh[0].vertex.end(); it++)
            {
                it->pos = glm::rotateX(it->pos, 0.01f);
            }
            //*/

            // /* Checkpoint 2: Rotate on CPU - works
            ud->obj.transform.addRotation(0.01f, {1.0f, 0.0f, 0.0f}).append(true);
            // */

            /* Checkpoint 3: Rotate on shader - doesn't rotate correctly
            ud->obj.transform.transform[0][0] += 0.01f;
            if(ud->obj.transform.transform[0][0] > M_TAU) ud->obj.transform.transform[0][0] -= M_TAU;
            // */
        } break;
        case 1:{
            /* Checkpoint 1: rotate mesh
            for(auto it = ud->obj.mesh[0].vertex.begin(); it != ud->obj.mesh[0].vertex.end(); it++)
            {
                it->pos = glm::rotateZ(it->pos, 0.01f);
            }
            //*/

            // /* Checkpoint 2: Rotate on CPU - works
            ud->obj.transform.addRotation(0.01f, {0.0f, 0.0f, 1.0f}).append(true);
            // */

            /* Checkpoint 3: Rotate on shader - doesn't rotate correctly
            ud->obj.transform.transform[0][1] += 0.01f;
            if(ud->obj.transform.transform[0][1] > M_TAU) ud->obj.transform.transform[0][1] -= M_TAU;
            // */
        } break;
        case 2:{
            /* Checkpoint 1: rotate mesh
            for(auto it = ud->obj.mesh[0].vertex.begin(); it != ud->obj.mesh[0].vertex.end(); it++)
            {
                it->pos = glm::rotateY(it->pos, 0.01f);
            }
            //*/

            // /* Checkpoint 2: Rotate on CPU - works
            ud->obj.transform.addRotation(0.01f, {0.0f, 1.0f, 0.0f}).append(true);
            // */

            /* Checkpoint 3: Rotate on shader - doesn't rotate correctly
            ud->obj.transform.transform[0][2] += 0.01f;
            if(ud->obj.transform.transform[0][2] > M_TAU) ud->obj.transform.transform[0][2] -= M_TAU;
            // */
        } break;
    }
}

///
/// draw()
/// Called every loop
///
void draw(atomic_bool* run, SDL_Window** window, void** userDataPtr)
{
    userData_t*& ud = *(userData_t**)userDataPtr;
    glViewport(0, 0, (int)ud->io->DisplaySize.x, (int)ud->io->DisplaySize.y);
    glClearColor(ud->clearCol[0], ud->clearCol[1], ud->clearCol[2], ud->clearCol[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ud->obj.updateBuffer();
    // Draw our object
    ud->obj.draw();

    ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(*window);
}

///
/// init()
/// This will only be run once (when the application starts)
///
int init(SDL_Window** window, SDL_GLContext* glContext, void** userDataPtr)
{
    *userDataPtr = (void*)new userData_t();
    userData_t*& ud = *(userData_t**)userDataPtr;
    // Setup SDL
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Setup Window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    *window = SDL_CreateWindow("ImGui SDL2 + OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 720, 720, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    *glContext = SDL_GL_CreateContext(*window);
    if (SDL_GL_SetSwapInterval(-1) == -1) // adaptive vsync
    {
        SDL_GL_SetSwapInterval(1); // standard vsync
    }

    // Initialise gl3w
    gl3wInit();

    // Initialise ImGui
    ImGui::CreateContext();
    ud->io = &ImGui::GetIO();
    ImGui_ImplSdlGL3_Init(*window);
    ImGui::StyleColorsDark();
    ud->style = &ImGui::GetStyle();
    ud->style->WindowRounding = 0;

    /* Load a shader from file (doesn't require recompiling to change shader but less portable)
    ud->shader.init(glakReadFile("shaders/vshader.glsl"), glakReadFile("shaders/fshader.glsl"));//*/

    ///* Create a shader (requires recompiling to change shader but more portable)
    ud->shader.init(
R"(
#version 330 core

in vec4 vPosition;
in vec4 vNormal;
in vec2 vTexCoord;
in vec4 vColor;

uniform mat4 uWorldView;
// uniform vec4 rotation;

out vec4 fColor;

void main()
{
    // vec3 rotation = uWorldView[0].xyz;
    // float c = cos(rotation.x);
    // float s = sin(rotation.x);
    // mat4 rx = mat4(
    //     c,-s, 0, 0,
    //     s, c, 0, 0,
    //     0, 0, 1, 0,
    //     0, 0, 0, 1
    // );
    // c = cos(rotation.y);
    // s = sin(rotation.y);
    // mat4 ry = mat4(
    //     1, 0, 0, 0,
    //     0, c,-s, 0,
    //     0, s, c, 0,
    //     0, 0, 0, 1
    // );
    // c = cos(rotation.z);
    // s = sin(rotation.z);
    // mat4 rz = mat4(
    //     c, 0, s, 0,
    //     0, 1, 0, 0,
    //     -s,0, c, 0,
    //     0, 0, 0, 1
    // );
    // mat4 worldview = rz * ry * rx;
    mat4 worldview = uWorldView;
    gl_Position = worldview * vPosition;
    fColor = vColor;
})",
R"(
#version 330 core

in vec4 fColor;

out vec4 pColor;

void main() 
{ 
    pColor = fColor;
})");//*/

    // Enable OpenGL Z buffer
    glEnable(GL_DEPTH_TEST);

    // Add a shader to the object
    ud->obj.shader.resize(1);
    ud->obj.shader[0] = ud->shader;

    // Add a mesh to the object
    ud->obj.mesh.resize(1);
    ud->obj.mesh[0].material = 0;

    // Add vertices
    ud->obj.mesh[0].vertex.resize(8);
    ud->obj.mesh[0].vertex[0].pos = {-0.4f, -0.4f, -0.4f, 1.0f};
    ud->obj.mesh[0].vertex[1].pos = { 0.4f, -0.4f, -0.4f, 1.0f};
    ud->obj.mesh[0].vertex[2].pos = { 0.4f,  0.4f, -0.4f, 1.0f};
    ud->obj.mesh[0].vertex[3].pos = {-0.4f,  0.4f, -0.4f, 1.0f};
    ud->obj.mesh[0].vertex[4].pos = {-0.4f, -0.4f,  0.4f, 1.0f};
    ud->obj.mesh[0].vertex[5].pos = { 0.4f, -0.4f,  0.4f, 1.0f};
    ud->obj.mesh[0].vertex[6].pos = { 0.4f,  0.4f,  0.4f, 1.0f};
    ud->obj.mesh[0].vertex[7].pos = {-0.4f,  0.4f,  0.4f, 1.0f};

    // Add colors
    ud->obj.mesh[0].vertex[0].col = {0.0f, 0.0f, 0.0f, 1.0f};
    ud->obj.mesh[0].vertex[1].col = {1.0f, 0.0f, 0.0f, 1.0f};
    ud->obj.mesh[0].vertex[2].col = {1.0f, 1.0f, 0.0f, 1.0f};
    ud->obj.mesh[0].vertex[3].col = {0.0f, 1.0f, 0.0f, 1.0f};
    ud->obj.mesh[0].vertex[4].col = {0.0f, 0.0f, 1.0f, 1.0f};
    ud->obj.mesh[0].vertex[5].col = {1.0f, 0.0f, 1.0f, 1.0f};
    ud->obj.mesh[0].vertex[6].col = {1.0f, 1.0f, 1.0f, 1.0f};
    ud->obj.mesh[0].vertex[7].col = {0.0f, 1.0f, 1.0f, 1.0f};

    // Add indices for triagles
    ud->obj.mesh[0].index.resize(16);
    ud->obj.mesh[0].index = {
        0, 3, 2, 2, 1, 0,
        2, 3, 7, 7, 6, 2,
        0, 4, 7, 7, 3, 0,
        1, 2, 6, 6, 5, 1,
        4, 5, 6, 6, 7, 4, 
        0, 1, 5, 5, 4, 0
    };

    ud->obj.updateBuffer();

    return 0;
}

///
/// destroy()
/// Called only once (at application shutdown)
///
int destroy(SDL_Window** window, SDL_GLContext* glContext, void** userDataPtr)
{
    userData_t*& ud = *(userData_t**)userDataPtr;
    ImGui_ImplSdlGL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(*glContext);
    SDL_DestroyWindow(*window);

    delete ud;

    SDL_Quit();

    return 0;
}