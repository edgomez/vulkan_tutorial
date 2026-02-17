/* --------------------------------------------------------------------------
 * Vulkan Tutorial Common Library
 *
 * ExplicitBuffer: minimal vector-like container with explicit,
 * non-throwing dynamic storage management.
 *
 * SPDX-FileCopyrightText: 2022-2026 Edouard Gomez
 * SPDX-License-Identifier: MIT
 * ----------------------------------------------------------------------- */

#pragma once

#include <cstddef>
#include <memory>
#include <new>

namespace egomez
{
namespace vulkan_tutorial
{

template <typename T> class ExplicitBuffer
{
  public:
    ExplicitBuffer() : m_data(nullptr), m_size(0), m_capacity(0)
    {
    }

    ~ExplicitBuffer() = default;

    ExplicitBuffer(ExplicitBuffer&& o) noexcept
        : m_data(std::move(o.m_data)), m_size(o.m_size), m_capacity(o.m_capacity)
    {
        o.m_size = 0;
        o.m_capacity = 0;
    }

    ExplicitBuffer& operator=(ExplicitBuffer&& o) noexcept
    {
        if (this != &o)
        {
            m_data = std::move(o.m_data);
            m_size = o.m_size;
            m_capacity = o.m_capacity;
            o.m_size = 0;
            o.m_capacity = 0;
        }
        return *this;
    }

    ExplicitBuffer(const ExplicitBuffer&) = delete;
    ExplicitBuffer& operator=(const ExplicitBuffer&) = delete;

    bool reserve(std::size_t n)
    {
        if (n <= m_capacity)
        {
            return true;
        }

        std::unique_ptr<T[]> p(new (std::nothrow) T[n]);
        if (!p)
        {
            return false;
        }

        for (std::size_t i = 0; i < m_size; ++i)
        {
            p[i] = m_data[i];
        }

        m_data = std::move(p);
        m_capacity = n;
        return true;
    }

    bool resize(std::size_t n)
    {
        if (!reserve(n))
        {
            return false;
        }
        m_size = n;
        return true;
    }

    bool push_back(const T& value)
    {
        if (m_size >= m_capacity)
        {
            std::size_t new_cap = m_capacity ? m_capacity * 2 : 4;
            if (!reserve(new_cap))
            {
                return false;
            }
        }
        m_data[m_size] = value;
        ++m_size;
        return true;
    }

    void clear()
    {
        m_size = 0;
    }

    T* data()
    {
        return m_data.get();
    }

    const T* data() const
    {
        return m_data.get();
    }

    std::size_t size() const
    {
        return m_size;
    }

    bool empty() const
    {
        return m_size == 0;
    }

    T& operator[](std::size_t i)
    {
        return m_data[i];
    }

    const T& operator[](std::size_t i) const
    {
        return m_data[i];
    }

    T* begin()
    {
        return m_data.get();
    }

    T* end()
    {
        return m_data.get() + m_size;
    }

    const T* begin() const
    {
        return m_data.get();
    }

    const T* end() const
    {
        return m_data.get() + m_size;
    }

  private:
    std::unique_ptr<T[]> m_data;
    std::size_t          m_size;
    std::size_t          m_capacity;
};

} // namespace vulkan_tutorial
} // namespace egomez
