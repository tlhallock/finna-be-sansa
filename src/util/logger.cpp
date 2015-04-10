#include "util/logger.h"
#include <fstream>


namespace
{
    std::ofstream* logfile;
}


void initLogging()
{
    logfile = new std::ofstream{"log.txt"};
}

std::ostream& getLog()
{
    return *logfile;
}

void closeLogging()
{
    delete logfile;
}
