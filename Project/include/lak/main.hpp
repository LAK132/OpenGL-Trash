/*
MIT License

Copyright (c) 2018 LAK132

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <vector>

#ifndef DEBUG
#   ifdef NDEBUG
#       define DEBUG(x)
#   else
#       include <iostream>
#       define DEBUG(x) std::cout << __FILE__ << "(" << std::dec << __LINE__ << ") " << x
#   endif
#endif

#ifdef LAK_MAIN_SDL
#define SDL_MAIN_HANDLED
#include <SDL.h>
#endif // LAK_MAIN_SDL

#ifdef LAK_MAIN_MULTITHREAD
#include <thread>
#include <atomic>
#include <mutex>
#include <lak/queue.hpp>
#endif // LAK_MAIN_MULTITHREAD

#ifndef LAK_MAIN_H
#define LAK_MAIN_H

#ifndef LAK_MAIN_USERDATATYPE
#define LAK_MAIN_USERDATATYPE void*
#endif // LAK_MAIN_USERDATATYPE

namespace lak
{
    using std::vector;
    #ifdef LAK_MAIN_MULTITHREAD
    using std::thread;
    using std::atomic;
    using std::atomic_bool;
    using std::mutex;
    using lak::queue_t;
    using lak::ticket_t;
    #endif // LAK_MAIN_MULTITHREAD

    struct loopData
    {
        #ifdef LAK_MAIN_MULTITHREAD
        atomic_bool running;
        queue_t windowq;
        queue_t drawq;
        atomic_bool updateNeedsContext = false;
        atomic_bool updateHasContext = false;
        #ifdef LAK_MAIN_SDL
        queue_t eventq;
        #endif // LAK_MAIN_SDL
        #else
        bool updateNeedsContext = false;
        const bool updateHasContext = true;
        bool running;
        #endif // LAK_MAIN_MULTITHREAD

        #ifdef LAK_MAIN_SDL
        SDL_Window* window;
        SDL_GLContext context;
        vector<SDL_Event> events;
        #endif // LAK_MAIN_SDL

        double targetDrawTime = 1.0/60.0;
        double drawDelta = 0.0;
        double updateDelta = 0.0;
        LAK_MAIN_USERDATATYPE userData;
    };
}

void init(lak::loopData* ld);
void update(lak::loopData* ld);
void draw(lak::loopData* ld);
void destroy(lak::loopData* ld);
#ifdef LAK_MAIN_SDL
void event(lak::loopData* ld);
#endif // LAK_MAIN_SDL

#endif // LAK_MAIN_H

#ifdef LAK_MAIN_IMPLEM
#ifndef LAK_MAIN_HAS_IMPLEM
#define LAK_MAIN_HAS_IMPLEM

void update_loop(lak::loopData* ld)
{
    using namespace lak;
    #ifdef LAK_MAIN_SDL
    static uint64_t LAST = 0;
    static uint64_t NOW = SDL_GetPerformanceCounter();
    #endif // LAK_MAIN_SDL

    #ifdef LAK_MAIN_SDL
    LAST = NOW;
    NOW = SDL_GetPerformanceCounter();
    ld->updateDelta = (NOW - LAST) / (double) SDL_GetPerformanceFrequency();
    #endif // LAK_MAIN_SDL

    #ifdef LAK_MAIN_MULTITHREAD
    #ifdef LAK_MAIN_SDL
    ticket_t eventt = ld->eventq.lock();
    #endif // LAK_MAIN_SDL
    ticket_t ticket;
    if(ld->updateNeedsContext) // slower but allows for OpenGL calls
    {
        ticket = ld->windowq.lock();
        SDL_GL_MakeCurrent(ld->window, ld->context);
        ld->updateHasContext = true;
    }
    else // faster but thread can't make OpenGL calls
    {
        ticket = ld->drawq.lock();
    }
    #endif // LAK_MAIN_MULTITHREAD

    update(ld);

    #ifdef LAK_MAIN_MULTITHREAD
    if(ld->updateHasContext) 
    {
        SDL_GL_MakeCurrent(ld->window, 0);
        ld->updateHasContext = false;
    }
    ticket = nullptr;
    #ifdef LAK_MAIN_SDL
    eventt = nullptr;
    #endif // LAK_MAIN_SDL
    #endif // LAK_MAIN_MULTITHREAD
}

void draw_loop(lak::loopData* ld)
{
    using namespace lak;
    #ifdef LAK_MAIN_SDL
    static uint64_t LAST = 0;
    static uint64_t NOW = SDL_GetPerformanceCounter();
    bool vsync = SDL_GL_GetSwapInterval() != 0;

    #ifdef LAK_MAIN_MULTITHREAD
    double deltaError = ld->drawDelta - ld->targetDrawTime;
    ld->drawDelta = deltaError < 0.0 ? 0.0 : deltaError - (ld->targetDrawTime * (double)(uint64_t)(deltaError / ld->targetDrawTime));
    #endif // LAK_MAIN_MULTITHREAD

    LAST = NOW;
    NOW = SDL_GetPerformanceCounter();
    #ifdef LAK_MAIN_MULTITHREAD
    ld->drawDelta += (NOW - LAST) / (double) SDL_GetPerformanceFrequency();
    #else
    ld->drawDelta = (NOW - LAST) / (double) SDL_GetPerformanceFrequency();
    #endif // LAK_MAIN_MULTITHREAD

    #ifdef LAK_MAIN_MULTITHREAD
    while(!vsync && ld->drawDelta < ld->targetDrawTime)
    {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        ld->drawDelta += (NOW - LAST) / (double) SDL_GetPerformanceFrequency();
        std::this_thread::yield();
    }
    #endif // LAK_MAIN_MULTITHREAD
    #endif // LAK_MAIN_SDL

    #ifdef LAK_MAIN_MULTITHREAD
    ticket_t windowt = ld->windowq.lock();
    #endif // LAK_MAIN_MULTITHREAD

    #ifdef LAK_MAIN_SDL
    SDL_GL_MakeCurrent(ld->window, ld->context);
    #endif // LAK_MAIN_SDL

    #ifdef LAK_MAIN_MULTITHREAD
    ticket_t drawt = ld->drawq.lock();
    #endif // LAK_MAIN_MULTITHREAD

    draw(ld);

    #ifdef LAK_MAIN_MULTITHREAD
    drawt = nullptr;
    #endif // LAK_MAIN_MULTITHREAD

    #ifdef LAK_MAIN_SDL
    SDL_GL_SwapWindow(ld->window);
    SDL_GL_MakeCurrent(ld->window, 0);
    #endif // LAK_MAIN_SDL

    #ifdef LAK_MAIN_MULTITHREAD
    windowt = nullptr;
    #endif // LAK_MAIN_MULTITHREAD

    #ifdef LAK_MAIN_SDL
    LAST = NOW;
    NOW = SDL_GetPerformanceCounter();
    ld->drawDelta += (NOW - LAST) / (double) SDL_GetPerformanceFrequency();
    #endif // LAK_MAIN_SDL
}

int main()
{
    using namespace lak;
    loopData ld;
    init(&ld);
    ld.running = true;
    #ifdef LAK_MAIN_SDL
    ld.events.reserve(100);
    #endif // LAK_MAIN_SDL
    
    #ifdef LAK_MAIN_MULTITHREAD
    #ifdef LAK_MAIN_SDL
    thread update([](loopData* ld){ while(ld->running) update_loop(ld); }, &ld);
    #endif // LAK_MAIN_SDL
    thread draw([](loopData* ld){ while(ld->running) draw_loop(ld); }, &ld);
    #endif // LAK_MAIN_MULTITHREAD

    while(ld.running)
    {
        #ifndef LAK_MAIN_MULTITHREAD
        #ifdef LAK_MAIN_SDL

        event(&ld);

        #endif // LAK_MAIN_SDL

        draw_loop(&ld);
        update_loop(&ld);

        #else
        #ifdef LAK_MAIN_SDL

        ticket_t windowt = ld.windowq.lock();
        ticket_t eventt = ld.eventq.lock();

        SDL_GL_MakeCurrent(ld.window, ld.context);
        event(&ld);
        SDL_GL_MakeCurrent(ld.window, 0);

        eventt = nullptr;
        windowt = nullptr;

        #else

        update_loop(&ld);

        #endif // LAK_MAIN_SDL
        #endif // LAK_MAIN_MULTITHREAD
    }
    
    #ifdef LAK_MAIN_MULTITHREAD
    #ifdef LAK_MAIN_SDL
    update.join();
    #endif // LAK_MAIN_SDL
    draw.join();
    #endif // LAK_MAIN_MULTITHREAD

    #ifdef LAK_MAIN_SDL
    SDL_GL_MakeCurrent(ld.window, ld.context);
    #endif // LAK_MAIN_SDL

    destroy(&ld);
    return 0;
}

#endif // LAK_MAIN_HAS_IMPLEM
#endif // LAK_MAIN_IMPLEM