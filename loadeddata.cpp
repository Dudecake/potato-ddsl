#include "loadeddata.h"
#include "potatoutils.h"

#include <chrono>
#include <ddsl.hpp>

log4cxx::LoggerPtr LoadedData::logger = log4cxx::Logger::getLogger("LoadedData");

DataSet<std::vector<DSImage::ImagePNG<float> >, std::vector<float>> LoadedData::splitSets()
{
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    auto start = high_resolution_clock::now();
    std::vector<std::pair<DSImage::ImagePNG<float>, float>> validationPair;
    std::vector<std::pair<DSImage::ImagePNG<float>, float>> trainingPair;
    uint classSamples = 0;
    std::set<uint> indexes;
    double classSplit = (percentageSplit * 3.0) / static_cast<double>(classOffsets.size());

    for (uint i = 0; i < classOffsets.size() - 1; i++)
    {
        uint classSize = classOffsets.at(i + 1) - classOffsets.at(i);
        classSamples += static_cast<uint>(static_cast<double>(classSize) * (classSplit / 100.0));
        uint currentIndex = 0;
        //for (int j = classOffsets.at(i); j < classOffsets.at(i + 1); j++)
        while (indexes.size() < classSamples)
        {
            currentIndex = classOffsets.at(i) + (rng() % classSize);
            if (indexes.find(currentIndex) == indexes.end())
            {
                validationPair.push_back(std::make_pair(data.at(currentIndex), static_cast<float>(i)));
                indexes.insert(currentIndex);
            }
        }
        for (uint j = classOffsets.at(i); j < classOffsets.at(i + 1); j++)
        {
            if (indexes.find(j) == indexes.end())
            {
                trainingPair.push_back(std::make_pair(data.at(j), static_cast<float>(i)));
            }
        }
    }
    std::shuffle(validationPair.begin(), validationPair.end(), rng);
    std::shuffle(trainingPair.begin(), trainingPair.end(), rng);
    std::vector<DSImage::ImagePNG<float>> validation;
    std::vector<float> validationClasses;
    std::vector<DSImage::ImagePNG<float>> training;
    std::vector<float> trainingClasses;
    for (std::pair<DSImage::ImagePNG<float>, float> item : validationPair)
    {
        validation.push_back(item.first);
        validationClasses.push_back(item.second);
    }
    for (std::pair<DSImage::ImagePNG<float>, float> item : trainingPair)
    {
        training.push_back(item.first);
        trainingClasses.push_back(item.second);
    }
    LOG4CXX_DEBUG(logger, "Split training and validation (" << training.size() << 'x' << validation.size() << ") set in " << duration_cast<milliseconds>(high_resolution_clock::now() - start).count() << "ms");
    return DataSet<std::vector<DSImage::ImagePNG<float>>, std::vector<float>>(training, trainingClasses, validation, validationClasses);
}

DataSet<DSLib::Matrix<DSLib::Matrix<float>>, std::vector<float>> LoadedData::splitSetsAsMatrixII()
{
    DataSet<std::vector<DSImage::ImagePNG<float>>, std::vector<float>> splitSet = splitSets();
    DSLib::Matrix<DSLib::Matrix<float>> trainImages, valImages;
    DSLib::Matrix<DSLib::Matrix<float>> currentMat;

    ((trainImages ^ splitSet.first.front().getChannel(0).mat()) | splitSet.first.front().getChannel(1).mat())  | splitSet.first.front().getChannel(2).mat();
    ((valImages ^ splitSet.third.front().getChannel(0).mat()) | splitSet.third.front().getChannel(1).mat()) | splitSet.third.front().getChannel(2).mat();
    for (auto imageIt = splitSet.first.begin() + 1; imageIt != splitSet.first.end(); imageIt++)
    {
        ((currentMat | (*imageIt).getChannel(0).mat()) | (*imageIt).getChannel(1).mat())  | (*imageIt).getChannel(2).mat();
        trainImages ^ std::move(currentMat);
        currentMat.clear();
    }
    for (auto imageIt = splitSet.third.begin() + 1; imageIt != splitSet.third.end(); imageIt++)
    {
        ((currentMat | (*imageIt).getChannel(0).mat()) | (*imageIt).getChannel(1).mat())  | (*imageIt).getChannel(2).mat();
        valImages ^ std::move(currentMat);
        currentMat.clear();
    }
    return DataSet<DSLib::Matrix<DSLib::Matrix<float>>, std::vector<float>>(trainImages, splitSet.second, valImages, splitSet.fourth);
}

DataSet<std::vector<DSLib::Matrix<DSLib::Matrix<float>>>, std::vector<float>> LoadedData::splitSetsAsMatrix()
{
    DataSet<std::vector<DSImage::ImagePNG<float>>, std::vector<float>> splitSet = splitSets();
    //std::vector<DSLib::Matrix<float>> trainImages{ DSLib::Matrix<float>() }, valImages{ DSLib::Matrix<float>() };
    std::vector<DSLib::Matrix<DSLib::Matrix<float>>> trainImages{ DSLib::Matrix<DSLib::Matrix<float>>() }, valImages{ DSLib::Matrix<DSLib::Matrix<float>>() };

    for (auto imageIt = splitSet.first.begin(); imageIt != splitSet.first.end(); imageIt++)
    {
        trainImages.back() | (*imageIt).getChannel(0).mat() | (*imageIt).getChannel(1).mat() | (*imageIt).getChannel(2).mat();
        trainImages.push_back(DSLib::Matrix<DSLib::Matrix<float>>());
    }
    trainImages.pop_back();
    for (auto imageIt = splitSet.third.begin(); imageIt != splitSet.third.end(); imageIt++)
    {
        valImages.back() | (*imageIt).getChannel(0).mat() | (*imageIt).getChannel(1).mat() | (*imageIt).getChannel(2).mat();
        valImages.push_back(DSLib::Matrix<DSLib::Matrix<float>>());
    }
    valImages.pop_back();
    return DataSet<std::vector<DSLib::Matrix<DSLib::Matrix<float>>>, std::vector<float>>(trainImages, splitSet.second, valImages, splitSet.fourth);
}
