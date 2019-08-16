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

#ifndef LAK_TYPE_LIST_H
#define LAK_TYPE_LIST_H

namespace lak
{
    struct null_t{};
    struct undefined_t;

    /*
     * type_list
     */
    template<typename ...H> struct type_list;

    template<typename H>
    struct type_list<H>
    {
        typedef H head;
        typedef null_t tail;
        enum { value = size_t(0) };
    };

    template<typename H, typename ...T>
    struct type_list<H, T...>
    {
        typedef H head;
        typedef type_list<T...> tail;
        enum { value = tail::value + 1 };
    };

    /*
     * type_list_pack
     * unwrap a type_list for variadic expansion
     * NOTE: T... is reversed compared to the TList ordering so that where_in_list expands in the correct order
     */
    template<typename TList, typename ...T> struct type_list_pack;

    template<typename H, typename ...T>
    struct type_list_pack<type_list<H, T...>>
    {
        typedef typename type_list_pack<typename type_list<H, T...>::tail, H>::type type;
    };

    template<typename H, typename ...T, typename ...U>
    struct type_list_pack<type_list<H, T...>, U...>
    {
        typedef typename type_list_pack<typename type_list<H, T...>::tail, H, U...>::type type;
    };

    template<typename ...U>
    struct type_list_pack<null_t, U...>
    {
        typedef typename type_list_pack<null_t, U...> type;
    };

    /*
     * typeof
     * type at position value in the type_list or null_t
     */
    template<typename TList, size_t value> struct typeof;

    template<typename H, typename ...T, size_t value>
    struct typeof<type_list<H, T...>, value>
    {
        typedef typename conditional<value == type_list<H, T...>::value, typename type_list<H, T...>::head, typename typeof<typename type_list<H, T...>::tail, value>>::type type;
    };

    template<size_t value>
    struct typeof<null_t, value>
    {
        typedef typename null_t type;
    };

    /*
     * valueof
     * position of the type in the type list or 0
     */
    template<typename TList, typename T>
    struct valueof
    {
        enum { value = valueof<TList::tail, T>::value };
    };

    template<typename T, typename U>
    struct valueof<type_list<T, U>, T>
    {
        enum { value = type_list<T, U>::value };
    };

    template<typename T>
    struct valueof<null_t, T>
    {
        enum { value = size_t(0) };
    };

    /*
     * maxsize
     * sizeof largest type in type_list
     */
    template<typename TList> struct maxsize;

    template<typename H, typename ...T> 
    struct maxsize<type_list<H, T...>>
    {
    private:
        enum { tailResult = size_t(maxsize<typename type_list<H, T...>::tail>::result) };
    public: 
        enum { result = sizeof(H) > tailResult ? sizeof(H) : size_t(tailResult) };
    };

    template<> 
    struct maxsize<null_t>
    {
        enum { result = 0 };
    };

    /*
     * in_list
     * will cause compilation failure if type is not in type_list
     */
    template<typename TList, typename T>
    struct in_list
    {
        typedef typename in_list<typename TList::tail, T>::result result;
    };

    template<typename T, typename ...U>
    struct in_list<type_list<T, U...>, T>
    {
        typedef T result;
    };

    template<typename T>
    struct in_list<null_t, T>
    {
        // typedef unknown_t result; // will intentionally throw a compiler error
    };

    /*
     * where_in_list
     */
    template<typename TList, typename TLPack> struct where_in_list;

    template<typename ...T, typename ...V>
    struct where_in_list<type_list<T...>, type_list_pack<V...>>
    {
        const static size_t size;
        const static size_t result[sizeof...(V)+1];
    };

    template<typename ...T, typename ...V>
    const size_t where_in_list<type_list<T...>, type_list_pack<V...>>::size = sizeof...(V)+1;

    // NOTE: type_list_pack is reversed so that this expands in the correct order
    template<typename ...T, typename ...V>
    const size_t where_in_list<type_list<T...>, type_list_pack<V...>>::result[sizeof...(V)+1] = { valueof<type_list<T...>, V>::value... };
}

#endif // LAK_TYPE_LIST_H