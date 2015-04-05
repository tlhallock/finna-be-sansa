#include "display.h"

#include "image.h"
#include <iostream>
#include <fstream>

#define PXLS_PER_POINT 1

extern "C"
{
#include <curses.h>
}


namespace gfx
{


namespace
{
    WINDOW * mainwin;
}

void initializeDisplay()
{
    mainwin = initscr();
    if (mainwin == nullptr)
    {
	    {
	    	std::ofstream output{"output.txt"};
	    	output << "This would seem to be a problem." << std::endl;
	    }
	    
        std::cerr << "Error initialising ncurses.\n" << std::endl;
        exit(EXIT_FAILURE);
    }
    nonl();
//    wbox(mainwin);
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
//        std::cout << width << std::endl;
        std::cout << it->second.x << ", " << it->second.y << std::endl;

        int x = (int) ((it->second.x - xmin) / (xmax-xmin) * nwidth );
        int y = (int) ((it->second.y - ymin) / (ymax-ymin) * nheight);

        if (x < 0 || y < 0 || x >= nheight || y >= nheight)
        {
            std::cout << x << ", " << y << std::endl;
            exit (-1);
        }

        mvaddstr(x, y, "x");
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
