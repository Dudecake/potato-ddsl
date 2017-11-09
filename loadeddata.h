#ifndef LOADEDDATA_H
#define LOADEDDATA_H

#include <chrono>
#include <vector>
#include <ddsl.hpp>

#include <log4cxx/logger.h>
//#include <type_traits>

#include "dataset.h"

//template <typename T, typename TInt = DSTypes::MatrixIdx/*, typename std::enable_if<std::is_base_of<DSLib::Matrix<DSLib::Matrix<ImagePNG<float>>>, T>::value>::type* = nullptr*/>
class LoadedData
{
    public:
        LoadedData() : percentageSplit(25.0), classOffsets{ 0 } {  }
        LoadedData(std::vector<DSImage::ImagePNG<float>> images, std::vector<uint> offsets, DSLib::Matrix<float> classTable) : percentageSplit(25.0), data(images), classOffsets(offsets), classTable(classTable) { }

        DSLib::Matrix<float> getClassTable()
        {
            return classTable;
        }

        uint getClasses()
        {
            return static_cast<uint>(classOffsets.size());
        }

        std::vector<DSImage::ImagePNG<float>> &getRData()
        {
            return data;
        }

        std::vector<DSImage::ImagePNG<float>> getData()
        {
            return data;
        }

        bool empty()
        {
            return data.empty();
        }

        size_t size()
        {
            return data.size();
        }

        /**
         * @brief splitSets splits the loaded data in a training and validation set
         * @return a pair containing the training and validation set respectively
         */
        DataSet<std::vector<DSImage::ImagePNG<float>>, std::vector<float>, std::vector<DSImage::ImagePNG<float>>, std::vector<float>> splitSets()
        {
            using std::chrono::high_resolution_clock;
            using std::chrono::duration_cast;
            using std::chrono::milliseconds;
            auto start = high_resolution_clock::now();
            std::vector<std::pair<DSImage::ImagePNG<float>, float>> validationPair;
            std::vector<std::pair<DSImage::ImagePNG<float>, float>> trainingPair;
            uint classSamples = 0;
            std::set<uint> indexes;
            double classSplit = percentageSplit / static_cast<double>(classOffsets.size());

            for (uint i = 0; i < classOffsets.size() - 1; i++)
            {
                uint classSize = classOffsets.at(i + 1) - classOffsets.at(i);
                classSamples += static_cast<uint>(static_cast<double>(classSize) * (classSplit / 100.0));
                uint currentIndex = 0;
                //for (int j = classOffsets.at(i); j < classOffsets.at(i + 1); j++)
                while (indexes.size() < classSamples)
                {
                    currentIndex = classOffsets.at(i) + (static_cast<uint>(rand()) % classSize);
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
            random_shuffle(validationPair.begin(), validationPair.end());
            random_shuffle(trainingPair.begin(), trainingPair.end());
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
            LOG4CXX_DEBUG(logger, "Split training and validation set in " << duration_cast<milliseconds>(high_resolution_clock::now() - start).count() << "ms");
            return DataSet<std::vector<DSImage::ImagePNG<float>>, std::vector<float>, std::vector<DSImage::ImagePNG<float>>, std::vector<float>>(training, trainingClasses, validation, validationClasses);
        }

        std::vector<DSImage::ImagePNG<float>> getClassImages(uint index)
        {
            std::vector<DSImage::ImagePNG<float>> res;
            if (index < classOffsets.size() - 1)
            {
                res.insert(res.begin(), data.begin() + classOffsets.at(index), data.begin() + classOffsets.at(index + 1) + 1);
            }
            return res;
        }

        void addClass(const std::vector<DSImage::ImagePNG<float>> images)
        {
            data.insert(data.end(), images.begin(), images.end());
            classTable | static_cast<float>(classOffsets.size());
            classOffsets.push_back(static_cast<uint>(data.size() - 1));
        }

    private:
        //T images;
        static log4cxx::LoggerPtr logger;
        double percentageSplit;
        std::vector<DSImage::ImagePNG<float>> data;
        std::vector<uint> classOffsets;
        DSLib::Matrix<float> classTable;

};

log4cxx::LoggerPtr LoadedData::logger = log4cxx::Logger::getLogger("LoadedData");

#endif // LOADEDDATA_H
