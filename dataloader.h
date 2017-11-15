#ifndef DATALOADER_H
#define DATALOADER_H

#include <string>
#include <memory>
#include <hpp/DS_Types.hpp>
#include <boost/filesystem.hpp>
#include <log4cxx/logger.h>

#include "potatoutils.h"
#include "loadeddata.h"

class DataLoader
{
    public:
        inline static std::shared_ptr<DataLoader> newSharedInstance()
        {
            std::shared_ptr<DataLoader> res = std::shared_ptr<DataLoader>(new DataLoader);
            res->weakSelf = res;
            return res;
        }

        inline std::shared_ptr<DataLoader> withClasses(int number)
        {
            this->classes = number;
            return weakSelf.lock();
        }

        inline std::shared_ptr<DataLoader> withDataRoot(std::string dataRoot)
        {
            this->datasetRoot = dataRoot;
            return weakSelf.lock();
        }

        std::shared_ptr<LoadedData> load();

    private:
        static log4cxx::LoggerPtr logger;
        std::weak_ptr<DataLoader> weakSelf;
        long classes;
        std::string datasetRoot;
        DataLoader() : classes(0), datasetRoot("") {  }

        static std::vector<ImagePNG<float>> loadImages(boost::filesystem::path path);
};

#endif // DATALOADER_H
