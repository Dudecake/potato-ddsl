#ifndef PROGRAM_H
#define PROGRAM_H

#include <string>
#include <log4cxx/logger.h>

class Program
{
    public:
        explicit Program(std::string datasetRoot, std::string modelName, std::string solverName) : datasetRoot(datasetRoot), modelName(modelName), solverName(solverName) {  }
        int run();
    private:
        static log4cxx::LoggerPtr logger;
        std::string datasetRoot;
        std::string modelName;
        std::string solverName;
};

#endif // PROGRAM_H
