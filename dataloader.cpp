#include "dataloader.h"

#include <chrono>
#include <log4cxx/helpers/exception.h>
#include <boost/filesystem.hpp>

log4cxx::LoggerPtr DataLoader::logger = log4cxx::Logger::getLogger("Dataloader");

std::shared_ptr<LoadedData> DataLoader::load()
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
    LOG4CXX_INFO(logger, "Started loading images in directory \"" << datasetRoot << '"');
    std::shared_ptr<LoadedData> res = std::make_shared<LoadedData>(percentageSplit);
    auto startLoad = high_resolution_clock::now();
    fs::directory_iterator end_iter;
    size_t lastSize = 0;
    for (fs::directory_iterator iter(datasetRoot); iter != end_iter; iter++)
    {
        if (fs::is_directory(iter->status()))
        {
            auto start = high_resolution_clock::now();
            LOG4CXX_DEBUG(logger, "Found new class \"" << iter->path().filename().string() << '"');
            res->addClassWithName(loadImages(iter->path()), iter->path().filename().string());
            LOG4CXX_DEBUG(logger, "Loaded " << (res->size() - lastSize) << " images in " << duration_cast<milliseconds>(high_resolution_clock::now() - start).count() << "ms");
            lastSize = res->size();
        }
    }
    if (res->empty())
    {
        LOG4CXX_ERROR(logger, "Could not find any classes in directory \"" << datasetRoot << '"');
    }
    LOG4CXX_INFO(logger, "Loaded all " << res->size() << " images in " << duration_cast<milliseconds>(high_resolution_clock::now() - startLoad).count() << "ms");
    return res;
}

std::vector<ImagePNG<float> > DataLoader::loadImages(boost::filesystem::path path)
{
    std::vector<ImagePNG<float>> res;

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
