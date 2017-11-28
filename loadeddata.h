#ifndef LOADEDDATA_H
#define LOADEDDATA_H

#include <vector>
#include <hpp/DS_Types.hpp>
#include <log4cxx/logger.h>

#include "dataset.h"

class LoadedData
{
    public:
        LoadedData(double percentageSplit = 25) : percentageSplit(percentageSplit), classOffsets{ 0 } {  }
        LoadedData(std::vector<DSImage::ImagePNG<float>> images, std::vector<uint> offsets, DSLib::Matrix<float> classTable, double percentageSplit = 25) :
            percentageSplit(percentageSplit), data(images), classOffsets(offsets), classTable(classTable) { }

        DSLib::Matrix<float> getClassTable()
        {
            return classTable;
        }

        uint getClasses()
        {
            return static_cast<uint>(classOffsets.size());
        }

        bool hasClassNames()
        {
            return !classNames.empty();
        }

        std::vector<std::string> getClassNames()
        {
            return classNames;
        }

        std::vector<DSImage::ImagePNG<float>> &getMutableData()
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
        DataSet<std::vector<DSImage::ImagePNG<float>>, std::vector<float>> splitSets();

        DataSet<DSLib::Matrix<DSLib::Matrix<float>>, std::vector<float>> splitSetsAsMatrixII();

        DataSet<std::vector<DSLib::Matrix<DSLib::Matrix<float>>>, std::vector<float>> splitSetsAsMatrix();

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
            classTable.cols.add(static_cast<float>(classOffsets.size() - 1));
            classNames.push_back(std::to_string(classOffsets.size() - 1));
            classOffsets.push_back(static_cast<uint>(data.size() - 1));
        }

        void addClassWithName(const std::vector<DSImage::ImagePNG<float>> images, std::string className)
        {
            data.insert(data.end(), images.begin(), images.end());
            classTable.cols.add(static_cast<float>(classOffsets.size() - 1));
            classNames.push_back(className);
            classOffsets.push_back(static_cast<uint>(data.size() - 1));
        }

    private:
        static log4cxx::LoggerPtr logger;
        double percentageSplit;
        std::vector<DSImage::ImagePNG<float>> data;
        std::vector<uint> classOffsets;
        DSLib::Matrix<float> classTable;
        std::vector<std::string> classNames;

};

#endif // LOADEDDATA_H
