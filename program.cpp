#include "program.h"

#include <memory>
#include <vector>
#include <chrono>
#include <ddsl.hpp>
#include <log4cxx/helpers/exception.h>

#include "potatoutils.h"
#include "dataloader.h"
#include "dataset.h"

log4cxx::LoggerPtr Program::logger = log4cxx::Logger::getLogger("Program");

int Program::run()
{
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    google::InitGoogleLogging("potato-ddsl");
    google::LogToStderr();
    //google::SetStderrLogging(google::GLOG_WARNING);
    try
    {
        std::shared_ptr<DataLoader> loader = DataLoader::newSharedInstance()->withDataRoot(datasetRoot);
        std::shared_ptr<LoadedData> data = loader->load();
//        DataSet<std::vector<DSImage::ImagePNG<float>>, std::vector<float>, std::vector<DSImage::ImagePNG<float>>, std::vector<float>> dataSet = data->splitSets();
//        DSLib::Table<> modelData = //(DSTypes::dtString | "data" | "label" | "split") ^
//                (((DSTypes::ctFeature | DSLib::Matrix<DSImage::ImagePNG<float>>(static_cast<unsigned int>(dataSet.first.size()), 1u, dataSet.first)) | (DSTypes::ctTarget | DSLib::Matrix<float>(static_cast<unsigned int>(dataSet.second.size()), 1u, dataSet.second)) | (DSTypes::ctSplit | DSLib::Matrix<float>(static_cast<unsigned int>(dataSet.second.size()), 1u, 0.f))) ^
//                  ((DSTypes::ctFeature | DSLib::Matrix<DSImage::ImagePNG<float>>(static_cast<unsigned int>(dataSet.third.size()), 1u, dataSet.third)) | (DSTypes::ctTarget | DSLib::Matrix<float>(static_cast<unsigned int>(dataSet.fourth.size()), 1u, dataSet.fourth)) | (DSTypes::ctSplit | DSLib::Matrix<float>(static_cast<unsigned int>(dataSet.fourth.size()), 1u, 1.f))));

        DataSet<DSLib::Matrix<DSLib::Matrix<float>>, std::vector<float>, DSLib::Matrix<DSLib::Matrix<float>>, std::vector<float>> dataSet = data->splitSetsAsMatrixII();
        DSLib::Table<> modelData = //(DSTypes::dtString | "data" | "label" | "split") ^
                (((DSTypes::ctFeature | dataSet.first) | (DSTypes::ctTarget | DSLib::Matrix<float>(static_cast<unsigned int>(dataSet.second.size()), 1u, dataSet.second)) | (DSTypes::ctSplit | DSLib::Matrix<float>(static_cast<unsigned int>(dataSet.second.size()), 1u, 0.f))) ^
                  ((DSTypes::ctFeature | dataSet.third) | (DSTypes::ctTarget | DSLib::Matrix<float>(static_cast<unsigned int>(dataSet.fourth.size()), 1u, dataSet.fourth)) | (DSTypes::ctSplit | DSLib::Matrix<float>(static_cast<unsigned int>(dataSet.fourth.size()), 1u, 1.f))));

        LOG4CXX_DEBUG(logger, "Shape of data to load in model:\n" << modelData.printLess());

        auto start = high_resolution_clock::now();
        DSModel::Caffe<float> pipeline = +DSModel::Caffe<float>(data->getClassTable(), modelName, solverName) | -DSModel::Confusion<float>(data->getClassTable());
        LOG4CXX_INFO(logger, "Loaded model in " << duration_cast<milliseconds>(high_resolution_clock::now() - start).count() << "ms");
        start = high_resolution_clock::now();
        DSLib::Table<> trainScore = pipeline.train(modelData(modelData[DSTypes::ctSplit] == 0.f));
        LOG4CXX_INFO(logger, "Trained model in " << duration_cast<milliseconds>(high_resolution_clock::now() - start).count() << "ms");
        std::stringstream ss;
        trainScore.print(ss);
        std::string confusionString = ss.str();
        confusionString.pop_back();
        LOG4CXX_INFO(logger, "Results of training:\n" << confusionString);
        DSLib::Table<> valScore = pipeline.train(modelData(modelData[DSTypes::ctSplit] == 1.f));
        ss.clear();
        valScore.print(ss);
        confusionString = ss.str();
        confusionString.pop_back();
        LOG4CXX_INFO(logger, "Results of evaluation:\n" << confusionString);
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
    //HANDLE_DDSL_ERROR
    return 0;
}
