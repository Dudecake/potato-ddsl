#include "dataloader.h"

#include <chrono>
#include <log4cxx/helpers/exception.h>
#include <boost/filesystem.hpp>

Logger DataLoader::logger = PotatoLogger::getLogger("Dataloader");

std::shared_ptr<LoadedData> DataLoader::load()
{
    namespace fs = boost::filesystem;
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    if (POTATO_UNLIKELY(!fs::exists(datasetRoot) || !fs::is_directory(datasetRoot)))
    {
        std::stringstream ss;
        ss << "Directory \"" << datasetRoot << "\" doesn't exist or isn't a directory" << std::endl;
        throw log4cxx::helpers::IOException(ss.str());
    }
    POTATO_INFO(logger, "Started loading images in directory \"" << datasetRoot << '"');
    std::shared_ptr<LoadedData> res = std::make_shared<LoadedData>(percentageSplit);
    auto startLoad = high_resolution_clock::now();
    fs::directory_iterator end_iter;
    size_t lastSize = 0;
    for (fs::directory_iterator iter(datasetRoot); iter != end_iter; iter++)
    {
        if (fs::is_directory(iter->status()))
        {
            auto start = high_resolution_clock::now();
            POTATO_DEBUG(logger, "Found new class \"" << iter->path().filename().string() << '"');
            res->addClassWithName(loadImages(iter->path()), iter->path().filename().string());
            POTATO_DEBUG(logger, "Loaded " << (res->size() - lastSize) << " images in " << duration_cast<milliseconds>(high_resolution_clock::now() - start).count() << "ms");
            lastSize = res->size();
        }
    }
    if (POTATO_UNLIKELY(res->empty()))
    {
        POTATO_ERROR(logger, "Could not find any classes in directory \"" << datasetRoot << '"');
    }
    POTATO_INFO(logger, "Loaded all " << res->size() << " images in " << duration_cast<milliseconds>(high_resolution_clock::now() - startLoad).count() << "ms");
    return res;
}

std::vector<DSImage::ImagePNG<float> > DataLoader::loadImages(boost::filesystem::path path)
{
    std::vector<DSImage::ImagePNG<float>> res;

    namespace fs = boost::filesystem;
    fs::directory_iterator end_iter;
    const std::string suffix(".png");
    for (fs::directory_iterator iter(path); iter != end_iter; iter++)
    {
        std::string filename = iter->path().string();
        bool isPng = endsWith(filename, suffix);
        if (POTATO_LIKELY(fs::is_regular_file(iter->status()) && isPng))
        {
            res.push_back(DSImage::ImagePNG<float>(iter->path().string(), true));
            POTATO_TRACE(logger, "Dimensions of loaded image " << res.back().printSize());
        }
    }
    if (POTATO_UNLIKELY(res.empty()))
    {
        POTATO_ERROR(logger, "Could not find any png's in directory \"" << path << '"');
    }
    return res;
}
