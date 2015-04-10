
#include "display.h"
#include "in/camera.h"
#include "in/scene.h"
#include "out/featurehistory.h"
#include "out/image.h"
#include "util/combination.h"
#include "util/logger.h"
#include <chrono>
#include <signal.h>
#include <stdio.h>
#include <thread>
#include <unistd.h>



namespace
{
    bool stop;
}

void finish(int)
{
    stop = true;
}

void testCombination()
{
    Combination c{4,3};

    do
    {
        std::cout << c << std::endl;
    } while (c.increment());
}

void testFeatureHistory()
{
    initLogging();
    FeatureHistory history{5};

    Point3d center;
    center = center * 10;

    for (int i=0;i<50;i++)
    {
        Point3d p1;
        p1 = p1 * 50;

        Point3d direction = center - p1;
        direction /= direction.norm();

        history.contribute(p1, direction);
    }

    getLog() << "The center is " << center << std::endl;
    getLog() << "The best guess is " << history.getEstimate() << std::endl;
    exit(0);
}

int main(int argc, char *argv[])
{
    testCombination();
    if (1) return 0;
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
        camera.moveBy(.1, 0, 0);
        camera.project(scene, image);
        gfx::display(image);

        std::this_thread::sleep_for(std::chrono::milliseconds{20});
    }

    gfx::destroyScene();
    closeLoggin();

    return 0;
}
