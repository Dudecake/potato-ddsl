#include "program.h"

#include <memory>
#include <vector>
#include <chrono>
#include <ddsl.hpp>
#include <log4cxx/helpers/exception.h>

#include "potatoutils.h"
#include "dataloader.h"
#include "dataset.h"
#include "confusion.hpp"
#include "evaluation.hpp"
#include "minicsv.h"

Logger Program::logger = PotatoLogger::getLogger("Program");

int Program::run()
{
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    try
    {
        std::shared_ptr<DataLoader> loader = DataLoader::newSharedInstance()->withDataRoot(datasetRoot)->withDataSplit(percentageSplit);
        std::shared_ptr<LoadedData> data = loader->load();
        DataSet<std::vector<DSImage::ImagePNG<float>>, std::vector<float>> dataSet = data->splitSets();
        DSLib::Table<> modelData = //(DSTypes::dtString | "data" | "label" | "split") ^
                (((DSTypes::ctFeature | DSLib::Matrix<DSImage::ImagePNG<float>>(static_cast<unsigned int>(dataSet.first.size()), 1u, dataSet.first)) | (DSTypes::ctTarget | DSLib::Matrix<float>(static_cast<unsigned int>(dataSet.second.size()), 1u, dataSet.second)) | (DSTypes::ctSplit | DSLib::Matrix<float>(static_cast<unsigned int>(dataSet.second.size()), 1u, 0.f))) ^
                  ((DSTypes::ctFeature | DSLib::Matrix<DSImage::ImagePNG<float>>(static_cast<unsigned int>(dataSet.third.size()), 1u, dataSet.third)) | (DSTypes::ctTarget | DSLib::Matrix<float>(static_cast<unsigned int>(dataSet.fourth.size()), 1u, dataSet.fourth)) | (DSTypes::ctSplit | DSLib::Matrix<float>(static_cast<unsigned int>(dataSet.fourth.size()), 1u, 1.f))));

//        DataSet<DSLib::Matrix<DSLib::Matrix<float>>, std::vector<float>> dataSet = data->splitSetsAsMatrixII();
//        DSLib::Table<> modelData = //(DSTypes::dtString | "data" | "label" | "split") ^
//                (((DSTypes::ctFeature | dataSet.first) | (DSTypes::ctTarget | DSLib::Matrix<float>(static_cast<unsigned int>(dataSet.second.size()), 1u, dataSet.second)) | (DSTypes::ctSplit | DSLib::Matrix<float>(static_cast<unsigned int>(dataSet.second.size()), 1u, 0.f))) ^
//                  ((DSTypes::ctFeature | dataSet.third) | (DSTypes::ctTarget | DSLib::Matrix<float>(static_cast<unsigned int>(dataSet.fourth.size()), 1u, dataSet.fourth)) | (DSTypes::ctSplit | DSLib::Matrix<float>(static_cast<unsigned int>(dataSet.fourth.size()), 1u, 1.f))));
        std::stringstream ss;
//        modelData.printLess(ss);
//        std::string bufferString = ss.str();
//        ss.str(std::string());
//        bufferString.pop_back();

//        POTATO_TRACE(logger, "Shape of data to load in model:\n" << bufferString);

        #ifdef CPU_ONLY
        caffe::Caffe::set_mode(caffe::Caffe::Brew::CPU);
        caffe::Caffe::set_multiprocess(true);
        DSLib::Matrix<int> gpus;// = (DSTypes::dtInt32 | 0);
        #else
        DSLib::Matrix<int> gpus(DSTypes::dtInt32 | 0);
        setCaffeGPUs(gpus);
        #endif
        auto start = high_resolution_clock::now();
        DSModel::Caffe<float> pipeline = +DSModel::Caffe<float>(data->getClassTable(), modelName, solverName, gpus);// | -DSModel::Confusion<float>(data->getClassTable());
        POTATO_INFO(logger, "Loaded model in " << duration_cast<milliseconds>(high_resolution_clock::now() - start).count() << "ms");
        start = high_resolution_clock::now();
        DSLib::Table<> trainScore = pipeline.train(modelData(modelData[DSTypes::ctSplit] == 0.f));
        POTATO_INFO(logger, "Trained model in " << duration_cast<milliseconds>(high_resolution_clock::now() - start).count() << "ms");

//        trainScore.cols.setIds(DSLib::Matrix<std::string>(static_cast<unsigned int>(data->getClassNames().size()), 1u, data->getClassNames()));
//        trainScore.rows.setIds(DSLib::Matrix<std::string>(static_cast<unsigned int>(data->getClassNames().size()), 1u, data->getClassNames()));
        DSLib::Matrix<float> temp = trainScore.findMatrix(DSTypes::ctResult, DSTypes::dtFloat)->data();
        Confusion confusion(std::vector<unsigned int>(dataSet.second.begin(), dataSet.second.end()), std::vector<unsigned int>(temp.data().begin(), temp.data().begin() + static_cast<long>(dataSet.second.size())));
        confusion.print(ss);
        ss << std::endl;
        Evaluation eval(confusion);
        eval.print(ss);
//        ss.str(std::string());
//        trainScore.print(ss);
//        bufferString = ss.str();
//        bufferString.pop_back();
        POTATO_INFO(logger, "Results of training:\n" << ss.str());

        #ifdef POTATO_UNIX
        std::string csvName = "/var/lib/potato-ddsl/results.csv";
        #else
        std::string csvName = datasetRoot + boost::filesystem::path::preferred_separator + "results.csv";
        #endif // POTATO_UNIX
        bool writeHeaders = !boost::filesystem::exists(csvName);
        mini::csv::ofstream csvOut(csvName, std::ios_base::app);
        csvOut.set_delimiter(';', std::string());
        if (writeHeaders)
        {
            csvOut << "Phase" << "DataSet" << "Model" << "Average System Accuracy" << "System Error"  << "Precision (Macro)" << "Recall (Macro)" << "Fscore (Macro)" << "Precision (Micro)" << "Recall (Micro)" << "Fscore (Micro)" << NEWLINE;
        }
        std::string dataSetName = datasetRoot.substr(datasetRoot.find_last_of("/") + 1);
        std::string cleanModelName = modelName.substr(modelName.find_last_of("/") + 1, modelName.find_last_of("."));
        csvOut << "Train" << dataSetName << cleanModelName << eval._avgAccuray << eval._errRate << eval._precisionMacro << eval._recallMacro << eval._fscoreMacro << eval._precisionMicro << eval._recallMicro << eval._fscoreMicro << NEWLINE;

        DSLib::Table<> valScore = pipeline.apply(modelData(modelData[DSTypes::ctSplit] == 1.f));
        temp = valScore.findMatrix(DSTypes::ctResult, DSTypes::dtFloat)->data();
        ss.str(std::string());
        confusion = Confusion(std::vector<unsigned int>(dataSet.fourth.begin(), dataSet.fourth.end()), std::vector<unsigned int>(temp.data().begin(), temp.data().begin() + static_cast<long>(dataSet.fourth.size())));
        confusion.print(ss);
        ss << std::endl;
        eval.evaluation(confusion);
        eval.print(ss);
//        valScore.cols.setIds(DSLib::Matrix<std::string>(static_cast<unsigned int>(data->getClassNames().size()), 1u, data->getClassNames()));
//        valScore.rows.setIds(DSLib::Matrix<std::string>(static_cast<unsigned int>(data->getClassNames().size()), 1u, data->getClassNames()));
//        valScore.print(ss);
//        bufferString = ss.str();
//        ss.str(std::string());
//        bufferString.pop_back();
        POTATO_INFO(logger, "Results of evaluation:\n" << ss.str());

        csvOut << "Val" << dataSetName << cleanModelName << eval._avgAccuray << eval._errRate << eval._precisionMacro << eval._recallMacro << eval._fscoreMacro << eval._precisionMicro << eval._recallMicro << eval._fscoreMicro << NEWLINE;
        csvOut.flush();
        if (!exportPath.empty())
        {
            pipeline.write(exportPath);
            POTATO_INFO(logger, "Exported model to " << exportPath);
        }

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
        POTATO_ERROR(logger, "Failed to load the dataset: " << ex.what());
    }
    //HANDLE_DDSL_ERROR
    return 0;
}
