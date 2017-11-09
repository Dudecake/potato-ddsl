#include "program.h"

#include <memory>
#include <vector>
#include <chrono>
#include <ddsl.hpp>

#include "potatoutils.h"
#include "dataloader.h"
#include "dataset.h"

log4cxx::LoggerPtr Program::logger = log4cxx::Logger::getLogger("Program");

int Program::run()
{
    try
    {
        using std::chrono::high_resolution_clock;
        using std::chrono::duration_cast;
        using std::chrono::milliseconds;
        //std::shared_ptr<DataLoader<DSLib::Matrix<ImagePNG<float>>>> loader = DataLoader<DSLib::Matrix<ImagePNG<float>>>::newSharedInstance();
        //std::shared_ptr<LoadedData<DSLib::Matrix<ImagePNG<float>>>> data = loader->withDataRoot(datasetRoot)->load();

        LOG4CXX_INFO(logger, "Started loading images in directory \"" << datasetRoot << '"');
        std::shared_ptr<DataLoader> loader = DataLoader::newSharedInstance();
        std::shared_ptr<LoadedData> data = loader->withDataRoot(datasetRoot)->load();
        //DSLib::Matrix<DSImage::ImagePNG<float>> loadedImages = data->getImages();
        //DSLib::Matrix<float> classTable = data->getClassTable();
        LOG4CXX_INFO(logger, "Loaded all images");
        DataSet<std::vector<DSImage::ImagePNG<float>>, std::vector<float>, std::vector<DSImage::ImagePNG<float>>, std::vector<float>> dataSet = data->splitSets();
        LOG4CXX_DEBUG(logger, "Split data in training and validation set");
        //trans
        auto mat1 = (DSTypes::ctFeature | DSLib::Matrix<ImagePNG<float>>(static_cast<unsigned int>(dataSet.first.size()), 1u, dataSet.first));// | (DSTypes::ctTarget | DSLib::Matrix<float>(dataSet.second));// | (DSTypes::ctSplit | DSLib::Matrix<float>(static_cast<unsigned int>(dataSet.second.size()), 1u, 0.f));
        LOG4CXX_TRACE(logger, mat1.printLess());
        auto mat2 = (DSTypes::ctFeature | DSLib::Matrix<ImagePNG<float>>(dataSet.third)) | (DSTypes::ctTarget | DSLib::Matrix<float>(dataSet.fourth));// | (DSTypes::ctSplit | DSLib::Matrix<float>(static_cast<unsigned int>(dataSet.fourth.size()), 1u, 1.f));
        DSLib::Table<> modelData = (mat1) ^
                  (mat2);

        auto start = high_resolution_clock::now();
        DSModel::Caffe<float> pipeline = +DSModel::Caffe<float>(data->getClassTable(), modelName, solverName) | -DSModel::Confusion<float>();
        LOG4CXX_INFO(logger, "Loaded model in " << duration_cast<milliseconds>(high_resolution_clock::now() - start).count() << "ms");
        start = high_resolution_clock::now();
        DSLib::Table<> tab;
        //DSLib::Table<> trainScore = tab(tab[DSTypes::ctSplit == 0u]) > pipeline;
        LOG4CXX_INFO(logger, "Trained model in " << duration_cast<milliseconds>(high_resolution_clock::now() - start).count() << "ms");
        //trainScore.pr;

        return 0;
        String f = "c:\\temp\\CT.png";
        //while (true) {
        Matrix<Double> m;
        readPNG(m, String(f));
        cout << "Next" << endl;
        //}

        /*
        unsigned int h, w;
        unsigned char c, bits;
        readPNGInfo(&h, &w, &c, &bits, f.c_str());
        m.clear();
        m.setOrder(oRowMajor);
        m.resize(h, w);
        readRawPNG(m.getMem(), f.c_str());
        */
    }
    catch (log4cxx::helpers::Exception &ex)
    {
        LOG4CXX_ERROR(logger, "Failed to load the dataset: " << ex.what())
    }
    HANDLE_DDSL_ERROR
    return 0;
}
