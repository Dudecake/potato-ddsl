#ifndef DATALOADER_H
#define DATALOADER_H

#include <string>
#include <memory>
#include <boost/filesystem.hpp>
#include <log4cxx/logger.h>

#include "potatoutils.h"
#include "loadeddata.h"

class DataLoader
{
    public:
        inline static std::shared_ptr<DataLoader> newSharedInstance()
        {
            return std::shared_ptr<DataLoader>(new DataLoader);
        }

        inline static DataLoader *newInstance()
        {
            return new DataLoader;
        }

        inline DataLoader *withClasses(int number)
        {
            this->classes = number;
            return this;
        }

        inline DataLoader *withDataRoot(std::string dataRoot)
        {
            this->datasetRoot = dataRoot;
            return this;
        }

        std::shared_ptr<LoadedData> load();

    private:
        static log4cxx::LoggerPtr logger;
        long classes;
        std::string datasetRoot;
        DataLoader() : classes(0), datasetRoot("") {  }

        static std::vector<ImagePNG<float>> loadImages(boost::filesystem::path path);
};

#endif // DATALOADER_H
