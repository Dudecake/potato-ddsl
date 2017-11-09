#ifndef DATASET_H
#define DATASET_H

#include <log4cxx/logger.h>
//#include <type_traits>

template <typename T, typename U, typename V, typename W>
class DataSet
{
    public:
        DataSet() = delete;
        DataSet(T first, U second, V third, W fourth) : first(first), second(second), third(third), fourth(fourth) {  }
        T first;
        U second;
        V third;
        W fourth;
};

#endif // DATASET_H
