#include <stdio.h>
#include <stdlib.h>

#include <Broadcast.hpp>
#include <Pool.hpp>
#include <Resources.hpp>
#include <Scheduler.hpp>
#include <ThreadPool.hpp>
#include <Updater.hpp>
#include <Window.hpp>

#include "Game.hpp"

using Scheduler = Core::Scheduler<time_base>;

Pool::ThreadPool* threadPool = nullptr;
Scheduler* scheduler = nullptr;

Game* game = nullptr;


int main(int argc, const char* argv[]) {
#if defined(LFS_COMPILER_MSVC) && defined(NDEBUG)
    //FreeConsole();
#endif

    Resources::setup(argv[0], "resources");

    // Setup pool and scheduler
    threadPool = Pool::ThreadPool::create(1);
    scheduler = Scheduler::create(60);

    // Setup window and scene
    Window window(640, 480, "CubGPU");
    game = new Game(&window);
    scheduler->every(1, game);

    // Setup signal bindings
    bind(&window, &Window::initializeGL, game, &Game::initializeGL);
    bind(&window, &Window::resize, game, &Game::onResize);
    bind(scheduler, &Scheduler::updateEnd, game, &Game::draw);

    // Enter the main loop
    window.mainloop();

    LOGD("[END] Stopping all threads");

    threadPool->stop();
    threadPool->join();

    LOGD("[END] Cleaning memory");

    // Clean GLFW
    glfwTerminate();
    
    // Unbind all callbacks (bind/emit)
    unbindAll();

    // Delete all allocated memory
    delete scheduler;
    delete threadPool;
    delete game;

    return 0;
}
