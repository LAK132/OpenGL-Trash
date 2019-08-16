#include <stdint.h>
#include <vector>
using std::vector;
#include <mutex>
using std::mutex;
using std::lock_guard;
#include <thread>
using std::thread;
#include <atomic>
using std::atomic;
using std::atomic_bool;
#include <iostream>
using std::cout;
using std::endl;
#define SDL_MAIN_HANDLED
#include <SDL.h>

#ifndef APP_NAME
#define APP_NAME "OpenGL Application"
#endif
#define DEBUG cout << __FILE__ << "(" << __LINE__ << ")" << endl;
#define LAK_MULTITHREAD

#ifndef LAKMAIN_H
#define LAKMAIN_H

int main();
void update_loop(atomic_bool* run, mutex* draw_mtx, mutex* update_mtx, SDL_Window** window, SDL_GLContext* glContext, void** userData);
#ifdef LAK_MULTITHREAD
void draw_loop(atomic_bool* run, mutex* draw_mtx, mutex* update_mtx, SDL_Window** window, SDL_GLContext* glContext, void** userData);
#endif

void update(atomic_bool* run, SDL_Window** window, double deltaTime, void** userData);
void draw(atomic_bool* run, SDL_Window** window, void** userData);
int init(SDL_Window** window, SDL_GLContext* glContext, void** userData);
int destroy(SDL_Window** window, SDL_GLContext* glContext, void** userData);

#endif