#include "main.h"

#define LAK_MAIN_IMPLEM
#include <lak/main.hpp>
#define LAK_SPACE_IMPLEM
#include <lak/space.hpp>
#define LAK_GRAPHICS_IMPLEM
#include <lak/graphics.hpp>
#define LAK_STREAM_UTIL_IMPLEM
#include <lak/stream_util.hpp>
#define LAK_GIZMO_IMPLEM
#include <lak/gizmo.hpp>
#define LAK_OBJ_IMPLEM
#include <lak/obj.hpp>
#define IMGUI_SPLITTER_IMPLEM
#include <imguisplitter.hpp>
// #include <lak/transform.hpp>

///
/// event()
/// Always called from the main thread, use this thread to access SDL events
///
void event(lak::loopData* ld)
{
    userData_t& ud = ld->userData;
    SDL_Event event;
    while (SDL_PollEvent(&event) && ld->events.size() < 100)
    {
        ld->events.push_back(event);
    }
    ImGui_ImplSdlGL3_NewFrame(ld->window);
}

void MessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam )
{
    DEBUG(std::hex << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") 
        << " type = 0x" << type 
        << ", severity = 0x" << severity
        << ", source = 0x" << source
        << ", message = " << endl << message << endl << endl);
    DEBUG(std::hex << "0x" << glGetError() << endl);
}

///
/// init()
/// This will only be run once (when the application starts)
///
void init(lak::loopData* ld)
{
    userData_t& ud = ld->userData;
    // Setup SDL
    assert(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) == 0);

    ud.screenw = 1280;
    ud.screenh = 720;

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
    ld->window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ud.screenw, ud.screenh, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    ld->context = SDL_GL_CreateContext(ld->window);
    SDL_GL_MakeCurrent(ld->window, ld->context);
    // /*
    if (SDL_GL_SetSwapInterval(-1) == -1) // adaptive vsync
    {
        SDL_GL_SetSwapInterval(1); // standard vsync
    }
    // */
    // SDL_GL_SetSwapInterval(0); // no vsync

    // Initialise gl3w
    gl3wInit();
    // During init, enable debug output
    #ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
    #endif // NDEBUG

    // Initialise ImGui
    ImGui::CreateContext();
    ud.io = &ImGui::GetIO();
    ImGui_ImplSdlGL3_Init(ld->window);
    ImGui::StyleColorsDark();
    ud.style = &ImGui::GetStyle();
    ud.style->WindowRounding = 0;
    ImGui_ImplSdlGL3_NewFrame(ld->window);

    glViewport(0, 0, 500, 500);
    glClearColor(ud.clearCol[0], ud.clearCol[1], ud.clearCol[2], ud.clearCol[3]);
    glEnable(GL_DEPTH_TEST);

    // call the 3 main functions once from the main thread to ensure they all initialise in the right order
    event(ld);
    ld->updateHasContext = true;
    update(ld);
    ld->updateHasContext = false;
    draw(ld);
}

///
/// destroy()
/// Called only once (at application shutdown)
///
void destroy(lak::loopData* ld)
{
    ImGui_ImplSdlGL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(ld->context);
    SDL_DestroyWindow(ld->window);

    SDL_Quit();
}