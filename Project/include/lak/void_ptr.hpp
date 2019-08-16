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

#include <type_traits>

#ifndef LAK_VOID_PTR_H
#define LAK_VOID_PTR_H

namespace lak
{
    using std::remove_reference;
    struct void_ptr
    {
        void(*deleter)(void*) = [](void*){};
        void* ptr = nullptr;
        void_ptr(){}
        ~void_ptr() { deleter(ptr); }
        void_ptr& operator=(void_ptr& other) // transfers ownership
        {
            deleter(ptr);
            deleter = other.deleter;
            ptr = other.ptr;
            other.deleter = [](void*){};
            return *this;
        }
        void_ptr& operator=(void_ptr&& other) { return *this = other; }
        template <typename T>
        void init()
        {
            deleter = [](void* ptr){
                if(ptr == nullptr) return;
                T* t = static_cast<T*>(ptr);
                delete t;
            };
            ptr = new (std::nothrow) T;
        }
        template <typename T>
        void init(void* _ptr, void(*_deleter)(void*) = [](void*)
            { if(ptr == nullptr) return; T* t = static_cast<T*>(ptr); delete t; })
        {
            ptr = _ptr;
            deletor = _deleter;
        }
        template <typename T>
        void init(T* _ptr, void(*_deleter)(void*) = [](void*)
            { if(ptr == nullptr) return; T* t = static_cast<T*>(ptr); delete t; })
        {
            ptr = _ptr;
            deletor = _deleter;
        }
        template <typename T>
        void_ptr& operator=(const typename remove_reference<T>::type& other)
        {
            init<T>();
            if(ptr != nullptr) *static_cast<T*>(ptr) = other;
            return *this;
        }
        template <typename T>
        void_ptr& operator=(typename remove_reference<T>::type&& other)
        {
            init<T>();
            if(ptr != nullptr) *static_cast<T*>(ptr) = other;
            return *this;
        }
        template <typename T>
        T* getPtr()
        {
            return static_cast<T*>(ptr);
        }
        template <typename T>
        T& getRef()
        {
            return *static_cast<T*>(ptr);
        }
    };
}

#endif // LAK_VOID_PTR_H