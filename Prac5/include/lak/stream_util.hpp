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
#include <iostream>
#include <string>

#ifndef LAK_STREAM_UTIL_H
#define LAK_STREAM_UTIL_H

namespace lak
{
    using std::istream;
    using std::string;

    // Skip all characters in strm until it reaches a character that isn't in str (exclusive)
    #define skipAll(strm, str)  for(const string s = str; s.find_first_of(strm.peek()) != string::npos && strm.good(); strm.get())
    #define skipAllS(strm, str) for(const string&s = str; s.find_first_of(strm.peek()) != string::npos && strm.good(); strm.get())
    // Skip all characters in strm until one character from str has been skipped (inclusive)
    #define skipOne(strm, str)  for(const string s = str; s.find_first_of(strm.get())  == string::npos && strm.good();)
    #define skipOneS(strm, str) for(const string&s = str; s.find_first_of(strm.get())  == string::npos && strm.good();)
    // Skip all characters in strm until one character from str has been skipped (exclusive)
    #define skipNone(strm, str) for(const string s = str; s.find_first_of(strm.peek()) == string::npos && strm.good(); strm.get())
    #define skipNoneS(strm, str)for(const string&s = str; s.find_first_of(strm.peek()) == string::npos && strm.good(); strm.get())

    template <typename T, char base = 10>
    T getNumber(istream& strm, const string& whitespace)
    {
        T rtn = 0;
        skipAll(strm, whitespace);
        bool isNeg = strm.peek() == '-';
        char c = (isNeg ? strm.get() : strm.peek());
        while(((c = strm.peek()) >= '0' && c < '0'+base) || 
            (c >= 'a' && c < 'a'+(base-9)) || 
            (c >= 'A' && c < 'A'+(base-9)))
        {
            rtn *= base;
            if(c >= '0' && c < '0'+base)
                rtn += (strm.get() - '0');
            else if(c >= 'a' && c < 'a'+(base-9))
                rtn += (strm.get() - 'a');
            else
                rtn += (strm.get() - 'A');
        }
        if(c == '.')
        {
            strm.get();
            T offset = 1;
            while(((c = strm.peek()) >= '0' && c < '0'+base) || 
                (c >= 'a' && c < 'a'+(base-9)) || 
                (c >= 'A' && c < 'A'+(base-9)))
            {
                offset *= base;
                if(c >= '0' && c < '0'+base)
                    rtn += (strm.get() - '0') / offset;
                else if(c >= 'a' && c < 'a'+(base-9))
                    rtn += (strm.get() - 'a') / offset;
                else
                    rtn += (strm.get() - 'A') / offset;
            }
        }
        return isNeg ? -rtn : rtn;
    }

    template <typename T, unsigned char base = 10>
    T getNumber(istream& strm, string&& whitespace = " \r\n\t") { return getNumber<T, base>(strm, whitespace); }
}

#endif // LAK_STREAM_UTIL_H