
#include <stdio.h>

#include "camera.h"
#include "image.h"
#include "scene.h"
#include <unistd.h>
#include <chrono>
#include <thread>
#include <signal.h>
#include "display.h"
#include "logger.h"


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

    initLogging();
    gfx::initializeDisplay();

    Scene scene{5000, 100};
//    Scene scene;
//    scene.addFeature(Point3d{50, 3, 0});

    Camera camera;
    Image image{camera.getWidth(), camera.getWidth()};

    while (!stop)
    {
//        camera.rotateBy(.005, 0);
        camera.moveBy(.1, 0, 0);
        camera.project(scene, image);
        gfx::display(image);

        std::this_thread::sleep_for(std::chrono::milliseconds{20});
    }

    getLog() << "we get here" << std::endl;
    gfx::destroyScene();
    closeLoggin();

    return 0;
}
