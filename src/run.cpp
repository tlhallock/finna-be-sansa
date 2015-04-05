
#include <stdio.h>

#include "camera.h"
#include "image.h"
#include "scene.h"
#include <unistd.h>
#include <chrono>
#include <thread>
#include <signal.h>
#include "display.h"


namespace
{
    bool stop;
}

void finish(int)
{
    stop = true;
}

int main(int argc, char *argv[])
{
    stop = false;
    // setup interrupt...
    (void) signal(SIGINT, finish);

    gfx::initializeDisplay();

    Scene scene{50, 10};

    Camera camera;
    Image image{camera.getWidth(), camera.getWidth()};

    while (!stop)
    {
        camera.rotateBy(.05, 0);
        camera.project(scene, image);
        gfx::display(image);

        std::this_thread::sleep_for(std::chrono::milliseconds{10});
    }

    gfx::destroyScene();

    return 0;
}
