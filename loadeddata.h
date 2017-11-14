#ifndef LOADEDDATA_H
#define LOADEDDATA_H

#include <vector>

#include <hpp/DS_Types.hpp>
#include <hpp/DS_Util.hpp>

#include <log4cxx/logger.h>

#include "dataset.h"

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

        unsigned int classes()
        {
            return static_cast<unsigned int>(classOffsets.size()) - 1;
        }

        size_t size()
        {
            return data.size();
        }

        /**
         * @brief splitSets splits the loaded data in a training and validation set
         * @return a pair containing the training and validation set respectively
         */
        DataSet<std::vector<DSImage::ImagePNG<float>>, std::vector<float>, std::vector<DSImage::ImagePNG<float>>, std::vector<float>> splitSets();

        DataSet<DSLib::Matrix<DSLib::Matrix<float>>, std::vector<float>, DSLib::Matrix<DSLib::Matrix<float>>, std::vector<float>> splitSetsAsMatrixII();

        DataSet<std::vector<DSLib::Matrix<DSLib::Matrix<float>>>, std::vector<float>, std::vector<DSLib::Matrix<DSLib::Matrix<float>>>, std::vector<float>> splitSetsAsMatrix();

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
            //classTable | static_cast<float>(classOffsets.size());
            classTable.cols.add(static_cast<float>(classOffsets.size()));
            classOffsets.push_back(static_cast<uint>(data.size() - 1));
        }

    private:
        static log4cxx::LoggerPtr logger;
        double percentageSplit;
        std::vector<DSImage::ImagePNG<float>> data;
        std::vector<uint> classOffsets;
        DSLib::Matrix<float> classTable;

};

#endif // LOADEDDATA_H
