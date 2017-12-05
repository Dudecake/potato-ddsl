#ifndef DATASET_H
#define DATASET_H

template <typename T, typename U>
class DataSet
{
    public:
        DataSet() = delete;
        //DataSet(const DataSet&) = delete;
        DataSet(T first, U second, T third, U fourth) : first(first), second(second), third(third), fourth(fourth) {  }
        T first;
        U second;
        T third;
        U fourth;
};

#endif // DATASET_H
