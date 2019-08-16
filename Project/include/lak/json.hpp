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

#include <unordered_map>
#include <vector>
#include <string>
#include <exception>
#include <iostream>
#include <lak/stream_util.hpp>

#ifndef LAK_JSON_H
#define LAK_JSON_H

namespace lak
{
    namespace JSON
    {
        using std::unordered_map;
        using std::vector;
        using std::string;
        using std::exception;
        using std::istream;
        using std::ostream;
        struct value_t;
        typedef value_t JSON;
        typedef unordered_map<string, value_t> object_t;
        typedef vector<value_t> array_t;

        struct value_t
        {
            enum type_t {JNULL = 0, BOOL, NUMBER, STRING, OBJECT, ARRAY} type;
            union {
                bool b;
                long double n;
                string s;
                object_t o;
                array_t a;
            };

            value_t()
            {
                type = JNULL;
            }

            value_t(const value_t& other)
            {
                *this = other;
            }

            value_t(value_t&& other)
            {
                *this = other;
            }

            ~value_t()
            {
                setType(JNULL);
            }

            void setType(type_t t)
            {
                if(type != t)
                {
                    switch(type)
                    {
                        case STRING: s.~string();
                        case OBJECT: o.~unordered_map();
                        case ARRAY: a.~vector();
                        default: break;
                    }
                    type = t;
                    switch(type)
                    {
                        case STRING: new (&s) string;
                        case OBJECT: new (&o) object_t;
                        case ARRAY: new (&a) array_t;
                        default: break;
                    }
                }
            }

            value_t& operator[](size_t index) { return a[index]; }

            value_t& operator[](string index) { return o[index]; }

            value_t& operator=(const value_t& other)
            {
                setType(other.type);
                switch(type)
                {
                    case BOOL: b = other.b;
                    case NUMBER: n = other.n;
                    case STRING: s = other.s;
                    case OBJECT: o = other.o;
                    case ARRAY: a = other.a;
                    default: break;
                }
                return *this;
            }

            value_t& operator=(value_t&& other) { return *this = other; }

            friend ostream& operator<<(ostream& os, const value_t& val)
            {
                switch(val.type)
                {
                    case value_t::JNULL: {
                        os << "null";
                    } break;
                    case value_t::BOOL: {
                        if(val.b) os << "true";
                        else os << "false";
                    } break;
                    case value_t::STRING: {
                        string str = val.s;
                        while(size_t i = str.find_first_of('\b') != string::npos) str.replace(i, 1, "\\b");
                        while(size_t i = str.find_first_of('\f') != string::npos) str.replace(i, 1, "\\f");
                        while(size_t i = str.find_first_of('\n') != string::npos) str.replace(i, 1, "\\n");
                        while(size_t i = str.find_first_of('\r') != string::npos) str.replace(i, 1, "\\r");
                        while(size_t i = str.find_first_of('\t') != string::npos) str.replace(i, 1, "\\t");
                        os << "\"" << val.s << "\"";
                    } break;
                    case value_t::NUMBER: {
                        os << val.n;
                    } break;
                    case value_t::OBJECT: {
                        os << "{";
                        for(auto it = val.o.begin(); it != val.o.end();)
                        {
                            os << "\"" << it->first << "\" : " << it->second;
                            it++;
                            if(it != val.o.end()) os << ", ";
                        }
                        os << "}";
                    } break;
                    case value_t::ARRAY: {
                        os << "[";
                        for(auto it = val.a.begin(); it != val.a.end();)
                        {
                            os << *it;
                            it++;
                            if(it != val.a.end()) os << ", ";
                        }
                        os << "]";
                    } break;
                }
                return os;
            }

            friend istream& operator>>(istream& is, value_t& val)
            {
                skipAll(is, WHITESPACE);
                switch(is.peek())
                {
                    case '{': {
                        val.setType(value_t::OBJECT);
                        val.o.clear();
                        skipOne(is, "\"");
                        char c = is.get();
                        while((c = is.peek()) != '}')
                        {
                            if(c == ',') is.get();
                            string str = getString(is);
                            skipOne(is, ":");
                            value_t v;
                            is >> v;
                            val.o[str] = v;
                            skipAll(is, WHITESPACE);
                        }                
                        is.get(); // remove }
                    } break;

                    case '[': {
                        val.setType(value_t::ARRAY);
                        val.a.clear();
                        char c = is.get();
                        while((c = is.peek()) != ']')
                        {
                            if(c == ',') is.get();
                            value_t v;
                            is >> v;
                            val.a.push_back(v);
                            skipAll(is, WHITESPACE);
                        }
                        is.get(); // remove ]
                    } break;

                    case '\"': {
                        val.setType(value_t::STRING);
                        val.s = getString(is);
                    } break;

                    case 't': {
                        val.setType(value_t::BOOL);
                        val.b = true;
                        skipAll(is, "true");
                    } break;

                    case 'f': {
                        val.setType(value_t::BOOL);
                        val.b = false;
                        skipAll(is, "false");
                    } break;

                    case 'n': {
                        val.setType(value_t::JNULL);
                        skipAll(is, "nul");
                    } break;

                    default: { // assume number
                        val.setType(value_t::NUMBER);
                        skipAll(is, WHITESPACE);
                        getNumber(strm, val.n);
                    } break;
                }
                return is;
            }
        };
    }
}

#endif // LAK_JSON_H