#include "display.h"
#include "out/image.h"
#include "util/logger.h"
#include <curses.h>
#include <fstream>

#include <thread>


#define PXLS_PER_POINT 1
#define DEBUG 0



void waitToDie()
{
    void finish(int);
    getch();
    finish(0);
}

namespace gfx
{

namespace
{
    WINDOW * mainwin;
}


void initializeDisplay()
{
//    initscr();

    mainwin = initscr(); //newwin(50, 50, 1, 1);
    box(mainwin, 0, 0);
    if (mainwin == nullptr)
    {
        getLog() << "This would seem to be a problem" << std::endl;
        getLog() << "Error initialising ncurses.\n" << std::endl;
        exit(EXIT_FAILURE);
    }
    nonl();
    cbreak();
    curs_set(0);

    // memory leak
    /*return*/ new std::thread{waitToDie};
}

void display(const Image& image)
{
    wclear(mainwin);
    int nwidth = getmaxx(mainwin);
    int nheight = getmaxy(mainwin);

    double xmin = -image.width;
    double xmax =  image.width;
    double ymin = -image.height;
    double ymax =  image.height;

    auto end = image.getPoints().end();
    for (auto it = image.getPoints().begin(); it != end; ++it)
    {
        int x = (int) ((it->second.x - xmin) / (xmax-xmin) * nwidth );
        int y = (int) ((it->second.y - ymin) / (ymax-ymin) * nheight);

#if DEBUG
        getLog() << "About to draw " << it->second.x << ", " << it->second.y << std::endl;
        getLog() << "This goes at " << x << "; " << nwidth << ", " << y << "/" << nheight << std::endl;
//        getLog() << x << ", " << y << std::endl;
//        getLog() << "curses bounds: " << nwidth << ", " << nheight << std::endl;
#endif

        if (x < 0 || y < 0 || x >= nwidth || y >= nheight)
        {
            getLog() << "We have a really bad X or Y" << std::endl;
            return;
        }

        mvaddstr(y, x, ".");
    }

    refresh();
}

void destroyScene()
{
    delwin(mainwin);
    endwin();
//    refresh();
}

}
