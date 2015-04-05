#include "logger.h"

#include <fstream>

namespace
{
    std::ofstream* logfile;
}


void initLogging()
{
    logfile = new std::ofstream{"output.txt"};
}

std::ostream& getLog()
{
    return *logfile;
}

void closeLoggin()
{
    delete logfile;
}
