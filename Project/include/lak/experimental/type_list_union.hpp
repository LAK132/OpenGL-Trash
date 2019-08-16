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

#ifndef TYPE_LIST_UNION_H
#define TYPE_LIST_UNION_H

namespace lak
{
    using std::conditional;
    template<typename U> struct structify_t { U dummy; };

    typedef type_list<
        signed char, signed short int, signed int, signed long int, signed long long int,
        unsigned char, unsigned short int, unsigned int, unsigned long int, unsigned long long int,
        signed char*, signed short int*, signed int*, signed long int*, signed long long int*,
        unsigned char*, unsigned short int*, unsigned int*, unsigned long int*, unsigned long long int*,
        float, double, long double, float*, double*, long double*, 
        void*, undefined_t(*)(undefined_t), undefined_t* undefined_t::*, undefined_t(undefined_t::*)(undefined_t),
        structify_t<signed char>, structify_t<signed short int>, structify_t<signed int>, structify_t<signed long int>, structify_t<signed long long int>, 
        structify_t<unsigned char>, structify_t<unsigned short int>, structify_t<unsigned int>, structify_t<unsigned long int>, structify_t<unsigned long long int>, 
        structify_t<signed char*>, structify_t<signed short int*>, structify_t<signed int*>, structify_t<signed long int*>, structify_t<signed long long int*>, 
        structify_t<unsigned char*>, structify_t<unsigned short int*>, structify_t<unsigned int*>, structify_t<unsigned long int*>, structify_t<unsigned long long int*>, 
        structify_t<void*>, structify_t<undefined_t(*)(undefined_t)>, structify_t<undefined_t* undefined_t::*>, structify_t<undefined_t(undefined_t::*)(undefined_t)>> 
        primitive_list_t;

    template<typename TList, size_t size> struct compute_align;

    template<size_t size>
    struct compute_align<null_t, size>
    {
        typedef null_t result;
    };

    template<typename H, typename ...T, size_t size>
    struct compute_align<type_list<H, T...>, size>
    {
        typedef typename compute_align<typename type_list<H, T...>::tail, size>::result tailResult;
        typedef typename conditional<sizeof(H) <= size, type_list<H, tailResult>, tailResult>::type result;
    };

    template<typename TList> union type_list_union;

    template<>
    union type_list_union<null_t>
    {
        null_t head;
    };

    template<typename H, typename ...T>
    union type_list_union<type_list<H, T...>>
    {
        H head;
        type_list_union<typename type_list<H, T...>::tail> tail;
    };

    template<typename TList>
    struct align_pod
    {
    private:
        enum { maxsize = maxsize<TList>::result };
        typedef typename compute_align<primitive_list_t, maxsize>::result alignTypes;
    public:
        typedef type_list_union<alignTypes> result;
    };
}

#endif