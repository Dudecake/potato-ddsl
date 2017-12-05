#ifndef PROGRAM_H
#define PROGRAM_H

#include <string>

#include "potatologger.h"

class Program
{
    public:
        explicit Program(std::string datasetRoot, std::string modelName, std::string solverName, std::string exportPath = std::string(), double percentageSplit = 25) : datasetRoot(datasetRoot), modelName(modelName), solverName(solverName), exportPath(exportPath), percentageSplit(percentageSplit) {  }
        int run();
    private:
        static Logger logger;
        std::string datasetRoot;
        std::string modelName;
        std::string solverName;
        std::string exportPath;
        double percentageSplit;
};

#endif // PROGRAM_H
