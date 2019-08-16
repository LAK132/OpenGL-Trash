#include "lakmain.h"

int main()
{
    SDL_Window* window = nullptr;
    SDL_GLContext glContext;
    void* userData;
    int i = init(&window, &glContext, &userData);
    if (i != 0) return i;

    atomic_bool run = true;
    mutex lock;
    mutex locklock;
    
    update(&run, &window, 0.0, &userData);

    #ifdef LAK_MULTITHREAD
    thread draw(draw_loop, &run, &lock, &locklock, &window, &glContext, &userData);
    #endif
    update_loop(&run, &lock, &locklock, &window, &glContext, &userData);

    #ifdef LAK_MULTITHREAD
    draw.join();
    #endif

    SDL_GL_MakeCurrent(window, glContext);
    return destroy(&window, &glContext, &userData);
}

#ifdef LAK_MULTITHREAD
void update_loop(atomic_bool* run, mutex* lock, mutex* locklock, SDL_Window** window, SDL_GLContext* glContext, void** userData)
{
    uint64_t LAST = 0;
    uint64_t NOW = SDL_GetPerformanceCounter();
    double deltaTime = 0;

    while(*run)
    {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = ((NOW - LAST) * 1000) / (double) SDL_GetPerformanceFrequency();

        locklock->lock();
        lock->lock();
        locklock->unlock();

        SDL_GL_MakeCurrent(*window, *glContext);
        update(run, window, deltaTime, userData);
        SDL_GL_MakeCurrent(0, 0);

        lock->unlock();
    }
}

void draw_loop(atomic_bool* run, mutex* lock, mutex* locklock, SDL_Window** window, SDL_GLContext* glContext, void** userData)
{
    uint64_t LAST = SDL_GetPerformanceCounter();
    uint64_t NOW = SDL_GetPerformanceCounter();
    double deltaTime = 0;
    double targetDrawTime = 12.0;

    while(*run)
    {
        for(int i = 0; i < 2 && *run; i++)
        {
            locklock->lock();
            lock->lock();
            locklock->unlock();

            SDL_GL_MakeCurrent(*window, *glContext);
            draw(run, window, userData);
            SDL_GL_MakeCurrent(0, 0);

            lock->unlock();
        }
        do 
        {
            LAST = NOW;
            NOW = SDL_GetPerformanceCounter();
            deltaTime += ((NOW - LAST) * 1000) / (double) SDL_GetPerformanceFrequency();
        } while (deltaTime < targetDrawTime); // sleep the draw thread
    }
}
#else
void update_loop(atomic_bool* run, mutex* lock, mutex* locklock, SDL_Window** window, SDL_GLContext* glContext, void** userData)
{
    uint64_t LAST = 0;
    uint64_t NOW = SDL_GetPerformanceCounter();
    double deltaTime = 0;

    // SDL_GL_MakeCurrent(*window, *glContext);
    while(*run)
    {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = ((NOW - LAST) * 1000) / (double) SDL_GetPerformanceFrequency();

        update(run, window, deltaTime, userData);
        draw(run, window, userData);
    }
    // SDL_GL_MakeCurrent(0, 0);
}
#endif