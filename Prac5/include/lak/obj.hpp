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
#include <tuple>
#include <iostream>
#include <string>
#ifndef vec3
#include <glm/vec3.hpp>
#endif
#include <lak/stream_util.hpp>

#ifndef LAK_OBJ_H
#define LAK_OBJ_H

namespace lak
{
    using std::vector;
    using std::tuple;
    using std::make_tuple;
    using std::istream;
    using std::streampos;
    using std::string;
    #ifndef vec3
    using glm::vec3;
    #endif

    void analyseOBJ(istream& strm, size_t* vcount, size_t* vtcount, size_t* vncount, size_t* fcount)
    {
        *vcount = 0;
        *vtcount = 0;
        *vncount = 0;
        *fcount = 0;
        streampos start = strm.tellg();
        skipAll(strm, " \r\n\t");
        while(strm.good())
        {
            string command;
            strm >> command;
            if(command == "v")
            {
                ++*vcount;
            }
            else if(command == "vt")
            {
                ++*vtcount;
            }
            else if(command == "vn")
            {
                ++*vncount;
            }
            else if(command == "f")
            {
                ++*fcount;
            }
            skipOne(strm, "\n"); // skip to end of line
            skipAll(strm, " \r\n\t"); // skip to next non-whitespace character
        }
        strm.clear();
        strm.seekg(start);
    }

    void readOBJ(istream& strm, vector<vec3>* verts, vector<vec3>* uvw, vector<vec3>* normals, vector<vector<tuple<size_t, size_t, size_t>>>* indices, size_t* vertCount)
    {
        size_t vcount, vtcount, vncount, fcount;
        analyseOBJ(strm, &vcount, &vtcount, &vncount, &fcount);
        *vertCount = 0;
        verts->clear();
        verts->reserve(vcount);
        uvw->clear();
        uvw->reserve(vtcount);
        normals->clear();
        normals->reserve(vncount);
        indices->clear();
        indices->reserve(fcount);
        skipAll(strm, " \r\n\t");
        while(strm.good())
        {
            string command;
            strm >> command;
            if(command == "v")
            {
                float x = 0.0f, y = 0.0f, z = 0.0f;
                strm >> x >> y >> z;
                // float x = getNumber<double>(strm);
                // float y = getNumber<double>(strm);
                // float z = getNumber<double>(strm);
                verts->push_back(vec3(x, y, z));
            }
            else if(command == "vt")
            {
                // float u = getNumber<double>(strm);
                // float v = getNumber<double>(strm);
                float u = 0.0f, v = 0.0f, w = 0.0f;
                strm >> u >> v;
                skipAll(strm, " \t");
                char c;
                if((c = strm.peek()) != '\r' && c != '\n')
                    strm >> w;
                    // w = getNumber<double>(strm);
                uvw->push_back(vec3(u, v, w));
            }
            else if(command == "vn")
            {
                // float x = getNumber<double>(strm);
                // float y = getNumber<double>(strm);
                // float z = getNumber<double>(strm);
                float x = 0.0f, y = 0.0f, z = 0.0f;
                strm >> x >> y >> z;
                normals->push_back(vec3(x, y, z));
            }
            else if(command == "f")
            {
                vector<tuple<size_t, size_t, size_t>> face(0);
                face.reserve(3);
                char c;
                do
                {
                    size_t v = getNumber<size_t>(strm, " \t") - 1;
                    size_t vt = 0, vn = 0;
                    skipAll(strm, " \t");
                    if(strm.peek() == '/')
                    {
                        strm.get();
                        skipAll(strm, " \t");
                        if(strm.peek() != '/')
                        {
                            vt = getNumber<size_t>(strm, " \t") - 1;
                            skipAll(strm, " \t");
                        }
                        if(strm.peek() == '/')
                        {
                            strm.get();
                            vn = getNumber<size_t>(strm, " \t") - 1;
                        }
                    }
                    skipAll(strm, " \t");
                    face.push_back(make_tuple(v, vt, vn));
                    (*vertCount)++;
                } while((c = strm.peek()) != '\r' && c != '\n' && strm.good());
                indices->push_back(face);
            }
            skipOne(strm, "\n"); // skip to end of line
            skipAll(strm, " \r\n\t"); // skip to next non-whitespace character
        }
    }
}

#endif // LAK_OBJ_H