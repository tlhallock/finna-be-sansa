
#include <stdio.h>

#include "camera.h"
#include "image.h"
#include "scene.h"
#include <unistd.h>
#include <chrono>
#include <thread>

int main(int argc, char *argv[])
{
    Scene scene{50, 10};

    std::cout << "Scene:\n" << scene << std::endl;

    Camera camera;
    Image image;

    std::cout << image << std::endl;

    for (int i=0;i<10000;i++)
    {
        camera.rotateBy(.05, 0);
//        camera.moveBy(.1, 0, 0);
        camera.project(scene, image);
        int printed = image.visualize(camera.getWidth(), std::cout);

        std::this_thread::sleep_for(std::chrono::milliseconds{10});

        for (int j=0;j<printed;j++)
        {
            std::cout << '\r' << std::endl;
        }
    }

    return 0;
}
