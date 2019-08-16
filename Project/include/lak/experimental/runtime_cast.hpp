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

#include <exception>
#include <lak/experimental/type_list.hpp>

#ifndef RUNTIME_CAST_H
#define RUNTIME_CAST_H

namespace lak
{
    using std::exception;
    class type_exception : public exception { using exception::exception; };

    /*
     * runtime_cast
     * returns the static_cast of obj from From to To
     * or throws type_exception
     */
    
    template<typename To, typename From> 
    struct runtime_cast
    {
    private:
        static auto caster(void* ptr, int) -> decltype(static_cast<To>(*reinterpret_cast<From*>(ptr))) // preferable but may fail at compiletime
        {
            return static_cast<To>(*reinterpret_cast<From*>(ptr));
        }
        static To caster(void* ptr, long) // if preferable fails, throw type_exception
        {
            throw type_exception();
        }
    public:
        static To cast(const From& ptr)
        {
            return caster(const_cast<From*>(&ptr), 0);
        }
        static To cast(From& ptr)
        {
            return caster(&ptr, 0);
        }
        static To cast(From&& ptr)
        {
            return cast(&ptr, 0);
        }
    };
    
    template<typename From> 
    struct runtime_cast<null_t, From>
    {
        static null_t cast(const From& ptr)
        {
            throw type_exception();
        }
        static null_t cast(From&& ptr)
        {
            throw type_exception();
        }
    };

    /*
     * returns ptr static_cast'ed to H* dereferenced and casted static_cast'ed to To
     * return (To)*(H1*)ptr;
     * or throws type_exception
     */
    template<typename To, typename H, typename ...T>
    struct runtime_cast<To, type_list<H, T...>>
    {
        static To cast(size_t type, void* ptr)
        {
            if(type == type_list<H, T...>::value)
            {
                return runtime_cast<To, H>::cast(*static_cast<H*>(ptr));
            }
            else
            {
                return runtime_cast<To, typename type_list<H, T...>::tail>::cast(type, ptr);
            }
        }
    };
    template<typename To>
    struct runtime_cast<To, null_t>
    {
        static To cast(size_t type, void* ptr)
        {
            throw type_exception();
        }
    };

    /*
     * ptr1 static_cast'ed to H1* dereferenced and set to dereferenced ptr2 static_cast'ed to H2*
     * *(H1*)ptr = (H1)*(H2*)ptr2;
     * or throws type_exception
     */
    template<typename H1, typename ...T1, typename H2, typename ...T2>
    struct runtime_cast<type_list<H1, T1...>, type_list<H2, T2...>>
    {
        static void cast(size_t type1, void* ptr1, size_t type2, void* ptr2)
        {
            if(type1 == type_list<H1, T1...>::value)
            {
                if(type2 == type_list<H2, T2...>value)
                {
                    *static_cast<H1*>(ptr1) = *runtime_cast<H1, H2>(*static_cast<H2*>(ptr2));
                }
                else
                {
                    runtime_cast<type_list<H1, T1...>, typename type_list<H2, T2...>::tail>::cast(type1, ptr1, type2, ptr2);
                }
            }
            else
            {
                runtime_cast<typename type_list<H1, T1...>::tail, type_list<H2, T2...>>::cast(type1, ptr1, type2, ptr2);
            }
        }
    };

    template<typename H1, typename ...T1>
    struct runtime_cast<type_list<H1, T1...>, null_t>
    {
        static void cast(size_t type1, void* ptr1, size_t type2, void* ptr2)
        {
            throw type_exception();
        }
    };

    template<typename H2, typename ...T2>
    struct runtime_cast<null_t, type_list<H2, T2...>>
    {
        static void cast(size_t type1, void* ptr1, size_t type2, void* ptr2)
        {
            throw type_exception();
        }
    };

    template<>
    struct runtime_cast<null_t, null_t>
    {
        static void cast(size_t type1, void* ptr1, size_t type2, void* ptr2)
        {
            throw type_exception();
        }
    };
}

#endif