
#include "display.h"
#include "in/camera.h"
#include "in/scene.h"
#include "out/featurehistory.h"
#include "out/image.h"
#include "out/reconstructor.h"
#include "util/combination.h"
#include "util/logger.h"
#include <chrono>
#include <signal.h>
#include <stdio.h>
#include <thread>
#include <unistd.h>
#include <fstream>



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

int main3(int argc, char *argv[])
{
    stop = false;
    (void) signal(SIGINT, finish);

    initLogging();
    gfx::initializeDisplay();

    Scene scene{50, 100};
//    Scene scene;
//    scene.addFeature(Point3d{1, 0, 0});

    {
        std::ofstream in {"input.txt" };
        in << scene;
    }


    Camera camera;
    Image image{camera.getWidth(), camera.getWidth()};

    Reconstructor rec;

    while (!stop)
    {
        camera.moveBy(.1, 0, 0);
        camera.project(scene, image);
        gfx::display(image);

        rec.contribute(image, camera.location(), camera.direction());

        std::this_thread::sleep_for(std::chrono::milliseconds{20});
    }


    {
        std::ofstream out {"output.txt" };
        rec.listPoints(out);
    }

    gfx::destroyScene();
    closeLogging();

    return 0;
}
