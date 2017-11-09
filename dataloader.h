#ifndef DATALOADER_H
#define DATALOADER_H

#include <ddsl.hpp>
#include <string>
#include <memory>
#include <chrono>
#include <log4cxx/logger.h>
#include <log4cxx/helpers/exception.h>
#include <boost/filesystem.hpp>

#include "potatoutils.h"
#include "loadeddata.h"

//template <typename T, typename TInt = DSTypes::MatrixIdx>
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

        //std::shared_ptr<LoadedData<T, TInt>> load()
        std::shared_ptr<LoadedData> load()
        {
            namespace fs = boost::filesystem;
            using std::chrono::high_resolution_clock;
            using std::chrono::duration_cast;
            using std::chrono::milliseconds;
            if (!fs::exists(datasetRoot) || !fs::is_directory(datasetRoot))
            {
                std::stringstream ss;
                ss << "Directory \"" << datasetRoot << "\" doesn't exist or isn't a directory" << std::endl;
                throw log4cxx::helpers::IOException(ss.str());
            }
            //std::shared_ptr<LoadedData<T, TInt>> res = std::make_shared<LoadedData<T, TInt>>();

            std::shared_ptr<LoadedData> res = std::make_shared<LoadedData>();
            //Matrix<float> classTable;
            //DSLib::Matrix<ImagePNG<float>> resImages;
            fs::directory_iterator end_iter;
            for (fs::directory_iterator iter(datasetRoot); iter != end_iter; iter++)
            {
                if (fs::is_directory(iter->status()))
                {
                    auto start = high_resolution_clock::now();
                    LOG4CXX_DEBUG(logger, "Found new class \"" << iter->path().filename().string() << '"');
                    //classTable.data().push_back(resImages.rows.count());
                    res->addClass(loadImages(iter->path()));
                    //resImages ^ loadImages(iter->path());
                    LOG4CXX_DEBUG(logger, "Loaded " << res->size() << " images in " << duration_cast<milliseconds>(high_resolution_clock::now() - start).count() << "ms");
                }
            }
            if (res->empty())
            {
                LOG4CXX_ERROR(logger, "Could not find any classes in directory \"" << datasetRoot << '"');
            }
            //classes = static_cast<int>(resImages.cols.count());
            //std::shared_ptr<LoadedData> res = std::make_shared<LoadedData>(resImages, classTable);
            return res;
        }

    private:
        static log4cxx::LoggerPtr logger;
        long classes;
        std::string datasetRoot;
        DataLoader() : classes(0), datasetRoot("") {  }

        static std::vector<ImagePNG<float>> loadImages(boost::filesystem::path path)
        {
            std::vector<ImagePNG<float>> res;
            DSImage::ImagePNG<float> image;

            namespace fs = boost::filesystem;
            fs::directory_iterator end_iter;
            const std::string suffix(".png");
            for (fs::directory_iterator iter(path); iter != end_iter; iter++)
            {
                std::string filename = iter->path().string();
                bool isPng = endsWith(filename, suffix);
                if (fs::is_regular_file(iter->status()) && isPng)
                {
                    res.push_back(ImagePNG<float>(iter->path().string(), true));
                    LOG4CXX_TRACE(logger, "Dimensions of loaded image " << res.back().printSize());
                }
            }
            if (res.empty())
            {
                LOG4CXX_ERROR(logger, "Could not find any png's in directory \"" << path << '"');
            }
            return res;
        }
};

/*
template <typename T, typename TInt>
log4cxx::LoggerPtr DataLoader<T, TInt>::logger = log4cxx::Logger::getLogger("Dataloader");
*/
log4cxx::LoggerPtr DataLoader::logger = log4cxx::Logger::getLogger("Dataloader");
#endif // DATALOADER_H
