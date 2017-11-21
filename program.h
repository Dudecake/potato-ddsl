#ifndef PROGRAM_H
#define PROGRAM_H

#include <string>
#include <log4cxx/logger.h>

class Program
{
    public:
        explicit Program(std::string datasetRoot, std::string modelName, std::string solverName, double percentageSplit = 25) : datasetRoot(datasetRoot), modelName(modelName), solverName(solverName), percentageSplit(percentageSplit) {  }
        int run();
    private:
        static log4cxx::LoggerPtr logger;
        std::string datasetRoot;
        std::string modelName;
        std::string solverName;
        double percentageSplit;
};

#endif // PROGRAM_H
