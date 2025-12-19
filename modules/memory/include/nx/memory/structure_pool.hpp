//
// Created by nexie on 18.12.2025.
//

#ifndef NXTBOT_STRUCTURE_POOL_HPP
#define NXTBOT_STRUCTURE_POOL_HPP
#include <vector>

namespace nx::collections
{
    template <typename T, typename C = std::vector>
    class data_pool_t
    {
        using value_type = typename T;
        using reference = T&;
        using pointer = T*;
        using data_container_type = C<value_type>;
        using ptr_container_type = C<pointer>;

        data_container_type _data;
        ptr_container_type _free_pointers;
        std::size_t _cur_free;

        void m_clear();
        void m_resize(size_t p_size);
        void m_init_pointers();
        bool m_owns_structure(pointer p_structure);

    public:
        data_pool_t(size_t p_size);

        std::size_t in_use() const;
        std::size_t available() const;
        std::size_t capacity() const;

        pointer get_structure();
        bool put_structure(pointer p_struct);
    };

    template <typename T, typename C>
    void data_pool_t<T, C>::m_clear()
    {
        m_init_pointers();
    }

    template <typename T, typename C>
    void data_pool_t<T, C>::m_resize(size_t p_size)
    {
        _data.resize(p_size);
        _free_pointers.resize(p_size);
        m_clear();
    }

    template <typename T, typename C>
    void data_pool_t<T, C>::m_init_pointers()
    {
        for (auto i = 0; i < _data.size(); i++)
        {
            _free_pointers[i] = &(_data[i]);
        }
        _cur_free = 0;
    }

    template <typename T, typename C>
    bool data_pool_t<T, C>::m_owns_structure(pointer p_structure)
    {
        if (capacity() == 0)
            return false;
        if (p_structure == nullptr)
            return false;

        return _data.data() <= p_structure && p_structure <= _data.data() + _data.size();
    }

    template <typename T, typename C>
    data_pool_t<T, C>::data_pool_t(size_t p_size)
    {
        m_resize(p_size);
    }

    template <typename T, typename C>
    std::size_t data_pool_t<T, C>::in_use() const
    {
        return _cur_free;
    }

    template <typename T, typename C>
    std::size_t data_pool_t<T, C>::available() const
    {
        return capacity() - in_use();
    }

    template <typename T, typename C>
    std::size_t data_pool_t<T, C>::capacity() const
    {
        return _data.size();
    }

    template <typename T, typename C>
    typename data_pool_t<T, C>::pointer data_pool_t<T, C>::get_structure()
    {
        if (_cur_free < _data.size())
        {
            return _free_pointers[_cur_free++];
        }
        return nullptr;
    }

    template <typename T, typename C>
    bool data_pool_t<T, C>::put_structure(pointer p_struct)
    {
        if (_cur_free <= 0)
            return false;

        if (!m_owns_structure(p_struct))
            return false;

        _free_pointers[--_cur_free] = p_struct;
        return true;
    }
}

#endif // NXTBOT_STRUCTURE_POOL_HPP
