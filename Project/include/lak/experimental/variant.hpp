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
#include <lak/experimental/type_list.hpp>
#include <lak/experimental/type_list_union.hpp>
#include <lak/experimental/runtime_cast.hpp>

#ifndef CFX_VARIANT_H
#define CFX_VARIANT_H

namespace lak
{
    using std::conditional;
    /* 
     * C++11 (variadic) implementation of Andrei Alexandrescu's typelists and variants
     * http://www.drdobbs.com/generic-programmingtypelists-and-applica/184403813
     * http://www.drdobbs.com/cpp/discriminated-unions-i/184403821
     * http://www.drdobbs.com/cpp/discriminated-unions-ii/184403828
     * Variadic templates allow for n-length typelists with only two specialisations
     * typelist<uint8_t, uint16_t, uint32_t> -> typelist<uint8_t, typelist<uint16_t, typelist<uint32_t, nullptr_t>>>
     * 
     * If templating a struct/class `name` for a variant, use:
     * template<typename TList>struct name;
     * template<typename H, typename ...T>struct name<typelist<H, T...>> : public name<T>{} // main specialisation
     * template<>struct name<null_t>{} // terminal specialisation
     */

    template<typename TList> struct new_t;

    template<typename H, typename ...T>
    struct new_t<type_list<H, T...>>
    {
        static void run(size_t type, void* ptr)
        {
            if(type == type_list<H, T...>::value)
            {
                new(ptr) H();
            }
            else
            {
                new_t<typename type_list<H, T...>::tail>::run(type, ptr);
            }
        }
        static void run(size_t type, void* dst, void* src)
        {
            if(type == type_list<H, T...>::value)
            {
                new(dst) H(*static_cast<H*>(src));
            }
            else
            {
                new_t<typename type_list<H, T...>::tail>::run(type, dst, src);
            }
        }
    };

    template<>
    struct new_t<null_t>
    {
        static void run(size_t type, void* ptr)
        {
            new(ptr) null_t();
        }
        static void run(size_t type, void* dst, void* src)
        {
            new(dst) null_t();
        }
    };

    typedef void(*deleter_f)(const void*);

    template<typename T>
    void deleter(const void* ptr)
    {
        static_cast<const T*>(ptr)->T::~T();
    }

    template<typename TList> struct getDeleter;

    template<typename H, typename ...T>
    struct getDeleter<type_list<H, T...>>
    {
        static deleter_f run(size_t type)
        {
            if(type == type_list<H, T...>::value)
            {
                return deleter<H>;
            }
            else
            {
                deleter<typename type_list<H, T...>::tail>::run(type);
            }
        }
    };
    
    template<typename ...TArgs>
    struct variant
    {
        typedef type_list<TArgs...> TList;
        typedef typename align_pod<TList>::result Align;
    private:
        union
        {
            unsigned char buffer_[];
            Align dummy_;
        };
        deleter_f _deleter;
        size_t _type_value = 0;
    public:
        size_t typeValue() const { return _type_value; }
        void* getPtr() { return &buffer_[0]; }
        const void* getConstPtr() const { return &buffer_[0]; }

        variant()
        {
            _type_value = valueof<TList, null_t>::value;
            _deleter = deleter<null_t>;
            new(&buffer_[0]) null_t();
        }

        template<typename T>
        variant(const T& val)
        {
            typedef in_list<TList, T>::result _test; // ensure T is a valid type
            _type_value = valueof<TList, T>::value;
            _deleter = deleter<T>;
            new(&buffer_[0]) T(val);
        }

        template<typename T>
        variant(T&& val)
        {
            typedef in_list<TList, T>::result _test; // ensure T is a valid type
            _type_value = valueof<TList, T>::value;
            _deleter = deleter<T>;
            new(&buffer_[0]) T(val);
        }

        ~variant()
        {
            _deleter(&buffer_[0]);
        }
        
        template<typename T>
        variant<TArgs...>& operator=(const T& other)
        {
            typedef in_list<TList, T>::result _test; // ensure T is a valid type
            if(_type_value == valueof<TList, T>::value)
            {
                *reinterpret_cast<T*>(&buffer_[0]) = other;
            }
            else
            {
                _deleter(&buffer_[0]);
                _deleter = deleter<T>;
                _type_value = valueof<TList, T>::value;
                new(&buffer_[0]) T(other);
            }
            return *this;
        }

        template<typename ...TA2>
        variant<TArgs...>& operator=(const variant<TA2...>& other)
        {
            typedef typename variant<TA2...>::TList TL2;
            size_t otherOccurs = where_in_list<TList, type_list_pack<TL2>>::result[other.typeValue()];
            if(otherOccurs == 0)
            {
                _deleter(&buffer_[0]);
                _deleter = deleter<null_t>;
                _type_value = valueof<TList, null_t>::value;
                new(&buffer_[0]) null_t();
            }
            else if(_type_value == 0)
            {
                _deleter(&buffer_[0]);
                _deleter = getDeleter<TList>::run(otherOccurs);
                _type_value = otherOccurs;
                new_t<TList>(otherOccurs, &buffer_[0], other.getPtr());
            }
            else // both variants have a type
            {
                try 
                {
                    runtime_cast<TList, TL2>::run(_type_value, &buffer_[0], other.typeValue(), other.getPtr());
                }
                catch(type_exception te) // coudn't cast the types so clear and override
                {
                    _deleter(&buffer_[0]);
                    _deleter = getDeleter<TList>::run(otherOccurs);
                    _type_value = otherOccurs;
                    new_t<TList>(otherOccurs, &buffer_[0], other.getPtr());
                }
            }
            return *this;
        }

        template<typename T>
        T get()
        {
            typedef in_list<TList, T>::result _test; // ensure T is a valid type
            return runtime_cast<T, TList>::cast(_type_value, &buffer_[0]);
        }

        template<typename T>
        T& getRef()
        {
            typedef in_list<TList, T>::result _test; // ensure T is a valid type
            if(_type_value != valueof<TList, T>::value)
            {
                *this = runtime_cast<T, TList>::cast(_type_value, &buffer_[0]);
            }
            return *reinterpret_cast<T*>(&buffer_[0]);
        }

        template<typename T>
        T* getPtr()
        {
            typedef in_list<TList, T>::result _test;
            if(_type_value != valueof<TList, T>::value)
            {
                *this = runtime_cast<T, TList>::cast(_type_value, &buffer_[0]);
            }
            return reinterpret_cast<T*>(&buffer_[0]);
        }
    };
}

#endif