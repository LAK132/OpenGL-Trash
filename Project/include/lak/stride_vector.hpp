/*
MIT License

Copyright (c) 2018 LAK132

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <vector>

#ifndef LAK_STRIDE_VECTOR_H
#define LAK_STRIDE_VECTOR_H

namespace lak
{
    using std::vector;
    struct stride_vector
    {
        size_t stride = 1;
        vector<uint8_t> data;
        stride_vector();
        stride_vector(size_t size);
        stride_vector(const stride_vector& other);
        stride_vector(stride_vector&& other);
        void init(size_t size);
        void init(size_t str, size_t size);
        stride_vector& operator=(const stride_vector& other);
        stride_vector& operator=(stride_vector&& other);
        vector<uint8_t> operator[](size_t idx) const;
        template <typename T>
        stride_vector& operator=(const vector<T>& other)
        {
            stride = sizeof(T) / sizeof(uint8_t);
            data.resize(sizeof(T) * other.size());
            memcpy(&(data[0]), &(other[0]), data.size());
            return *this;
        }
        template <typename T>
        inline T* get()
        {
            return (T*)&(data[0]);
        }
        template <typename T>
        static stride_vector strideify(const vector<T>& other)
        {
            stride_vector rtn;
            return rtn = other;
        }
        static stride_vector interleave(const vector<stride_vector*>& vecs);
    };
}

#endif // LAK_STRIDE_VECTOR_H

#ifdef LAK_STRIDE_VECTOR_IMPLEM
#ifndef LAK_STRIDE_VECTOR_HAS_IMPLEM
#define LAK_STRIDE_VECTOR_HAS_IMPLEM

namespace lak
{
    stride_vector::stride_vector(){}

    stride_vector::stride_vector(size_t size)
    {
        init(size);
    }

    stride_vector::stride_vector(const stride_vector &other)
    : stride(other.stride), data(other.data)
    {
    }

    stride_vector::stride_vector(stride_vector &&other)
    : stride(other.stride), data(std::move(other.data))
    {
    }

    void stride_vector::init(size_t size)
    {
        stride = 1;
        data.reserve(size);
    }

    void stride_vector::init(size_t str, size_t size)
    {
        stride = str;
        data.resize(size);
    }

    stride_vector &stride_vector::operator=(const stride_vector &other)
    {
        stride = other.stride;
        data = other.data;
        return *this;
    }

    stride_vector &stride_vector::operator=(stride_vector &&other)
    {
        stride = other.stride;
        data = std::move(other.data);
        return *this;
    }

    vector<uint8_t> stride_vector::operator[](size_t idx) const
    {
        if(stride * idx >= data.size()) return vector<uint8_t>(0);
        vector<uint8_t> rtn(stride);
        memcpy(&rtn[0], &data[stride * idx], stride);
        return rtn;
    }

    stride_vector stride_vector::interleave(const vector<stride_vector*>& vecs)
    {
        size_t size = 0;
        size_t maxlen = 0;
        for(auto it = vecs.begin(); it != vecs.end(); it++)
        {
            size += (*it)->data.size();
            if((*it)->data.size() > maxlen) maxlen = (*it)->data.size() / (*it)->stride;
        }
        stride_vector rtn(size);
        for(size_t i = 0; i < maxlen; i++)
        {
            for (const auto &it : vecs)
            {
                const vector<uint8_t> vec = (*it)[i];
                for (auto it2 : vec)
                {
                    rtn.data.push_back(it2);
                }
            }
        }
        return rtn;
    }
}

#endif // LAK_STRIDE_VECTOR_HAS_IMPLEM
#endif // LAK_STRIDE_VECTOR_IMPLEM