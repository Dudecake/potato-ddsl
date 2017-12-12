#ifndef DATALOADER_H
#define DATALOADER_H

#include <string>
#include <memory>
#include <hpp/DS_Types.hpp>
#include <boost/filesystem.hpp>

#include "potatoutils.h"
#include "potatologger.h"
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

        inline std::shared_ptr<DataLoader> withDataSplit(double percentageSplit)
        {
            this->percentageSplit = percentageSplit;
            return weakSelf.lock();
        }

        std::shared_ptr<LoadedData> load();

    private:
        static Logger logger;
        std::weak_ptr<DataLoader> weakSelf;
        long classes;
        std::string datasetRoot;
        double percentageSplit;
        DataLoader() : classes(0), datasetRoot(""), percentageSplit(25) {  }

        static std::vector<DSImage::ImagePNG<float>> loadImages(boost::filesystem::path path);
};

#endif // DATALOADER_H
