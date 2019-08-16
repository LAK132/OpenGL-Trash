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

#ifndef DEBUG
#   ifdef NDEBUG
#       define DEBUG(x)
#   else
#       define DEBUG(x) std::cout << __FILE__ << "(" << std::dec << __LINE__ << ") " << x
#   endif
#endif

#include <iostream>
#include <fstream>
#include <string>

#ifndef LAK_STREAM_UTIL_H
#define LAK_STREAM_UTIL_H

using std::istream;
using std::ifstream;
using std::streampos;
using std::string;
using std::endl;

// Skip all characters in strm until it reaches a character that isn't in str (exclusive)
#define skipAll(_strm, _str)        for(const string _s = _str; _s.find_first_of(_strm.peek()) != string::npos && _strm.good(); _strm.get())
#define skipAllS(_strm, _str)       for(;_str.find_first_of(_strm.peek()) != string::npos && _strm.good(); _strm.get())
#define skipAllC(_strm, _str, _c)   for(const string _s = _str; _s.find_first_of(_c = _strm.peek()) != string::npos && _strm.good(); _strm.get())
// Skip all characters in strm until one character from str has been skipped (inclusive)
#define skipOne(_strm, _str)        for(const string _s = _str; _s.find_first_of(_strm.get())  == string::npos && _strm.good();)
#define skipOneS(_strm, _str)       for(;_str.find_first_of(_strm.get())  == string::npos && _strm.good();)
#define skipOneC(_strm, _str, _c)   for(const string _s = _str; _s.find_first_of(_c = _strm.get())  == string::npos && _strm.good();)
// Skip all characters in strm until one character from str has been skipped (exclusive)
#define skipNone(_strm, _str)       for(const string _s = _str; _s.find_first_of(_strm.peek()) == string::npos && _strm.good(); _strm.get())
#define skipNoneS(_strm, _str)      for(;_str.find_first_of(_strm.peek()) == string::npos && _strm.good(); _strm.get())
#define skipNoneC(_strm, _str, _c)  for(const string _s = _str; _s.find_first_of(_c = _strm.peek()) == string::npos && _strm.good(); _strm.get())
#define getNumber(_strm, _num)      {long double _d; _strm >> _d; _num = _d;}

void readFile(const string& src, string* dst);
void readFile(string&& src, string* dst);
string readFile(const string& src);
string readFile(string&& src);
string getString(istream& strm, const char terminator = '\"');

#endif // LAK_STREAM_UTIL_H

#ifdef LAK_STREAM_UTIL_IMPLEM
#ifndef LAK_STREAM_UTIL_HAS_IMPLEM
#define LAK_STREAM_UTIL_HAS_IMPLEM

void readFile(const string& src, string* dst)
{
    *dst = "";
    ifstream strm(src.c_str(), std::ios::binary|ifstream::in);
    if(!strm.is_open())
    {
        DEBUG("Failed to open file " << src.c_str() << endl);
        return;
    }
    streampos start = strm.tellg();
    size_t size = 0;
    string str = ""; str += EOF;
    skipOneS(strm, str) ++size; // I'm not sorry 
    strm.clear();
    strm.seekg(start);
    dst->reserve(size);
    for(char c = strm.get(); strm >> c;)
        *dst += c;
}

void readFile(string&& src, string* dst)
{
    readFile(src, dst);
}

string readFile(const string& src)
{
    string str;
    readFile(src, &str);
    return str;
}

string readFile(string&& src)
{
    string str;
    readFile(src, &str);
    return str;
}

string getString(istream& strm, const char terminator)
{
    string str = "";
    char c = strm.get();
    while((c = strm.get()) != terminator && strm.good())
    {
        if(c == '\\')
        {
            switch(strm.get())
            {
                case '\\': str += '\\'; break;
                case '\'': str += '\''; break;
                case '\"': str += '\"'; break;
                case 'b': str += "\\b"; break;
                case 'f': str += '\f'; break;
                case 'n': str += '\n'; break;
                case 'r': str += '\r'; break;
                case 't': str += '\t'; break;
                case 'u': {
                    str += "\\u";
                    str += strm.get();
                    str += strm.get();
                    str += strm.get();
                    str += strm.get();
                } break;
            }
        }
        else str += c;
    }
    return str;
}

#endif // LAK_STREAM_UTIL_HAS_IMPLEM
#endif // LAK_STREAM_UTIL_IMPLEM