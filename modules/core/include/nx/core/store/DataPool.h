//
// Created by nexie on 23.10.2025.
//

#ifndef DATA_POOL_H
#define DATA_POOL_H
#include <vector>

namespace nx
{
    template <typename T>
    class DataPool
    {
        using value_type = T;
        using reference = T&;
        using pointer = T*;

        std::vector<value_type> _data;
        std::vector<pointer> _free_pointers;
        std::size_t _cur_free;

        void m_clear();
        void m_resize(size_t p_size);
        void m_init_pointers();
        bool m_owns_structure(pointer p_structure);

    public:
        DataPool(size_t p_size);

        std::size_t inUse() const;
        std::size_t available() const;
        std::size_t capacity() const;

        pointer getNew();
        bool put(pointer p_struct);
    };

    template <typename T>
    void DataPool<T>::m_clear()
    {
        m_init_pointers();
    }

    template <typename T>
    void DataPool<T>::m_resize(size_t p_size)
    {
        _data.resize(p_size);
        _free_pointers.resize(p_size);
        m_clear();
    }

    template <typename T>
    void DataPool<T>::m_init_pointers()
    {
        for (auto i = 0; i < _data.size(); i++)
        {
            _free_pointers[i] = &(_data[i]);
        }
        _cur_free = 0;
    }

    template <typename T>
    bool DataPool<T>::m_owns_structure(pointer p_structure)
    {
        if (capacity() == 0)
            return false;
        if (p_structure == nullptr)
            return false;

        return _data.data() <= p_structure && p_structure <= _data.data() + _data.size();
    }

    template <typename T>
    DataPool<T>::DataPool(size_t p_size)
    {
        m_resize(p_size);
    }

    template <typename T>
    std::size_t DataPool<T>::inUse() const
    {
        return _cur_free;
    }

    template <typename T>
    std::size_t DataPool<T>::available() const
    {
        return capacity() - inUse();
    }

    template <typename T>
    std::size_t DataPool<T>::capacity() const
    {
        return _data.size();
    }

    template <typename T>
    typename DataPool<T>::pointer DataPool<T>::getNew()
    {
        if (_cur_free < _data.size())
        {
            return _free_pointers[_cur_free++];
        }
        return nullptr;
    }

    template <typename T>
    bool DataPool<T>::put(pointer p_struct)
    {
        if (_cur_free <= 0)
            return false;

        if (!m_owns_structure(p_struct))
            return false;

        _free_pointers[--_cur_free] = p_struct;
        return true;
    }
}

#endif //DATA_POOL_H
