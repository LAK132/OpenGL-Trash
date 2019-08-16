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
#       include <iostream>
#       define DEBUG(x) std::cout << __FILE__ << "(" << std::dec << __LINE__ << ") " << x
#   endif
#endif

#include <lak/stream_util.hpp>

namespace lak
{
    template<typename img_t> // lak::image_t
    img_t readPNM(istream& strm, bool invert_y = true)
    {
        img_t rtn;
        if(!strm.good()) return rtn;
        if(strm.get() != 'P') return rtn;
        switch(strm.get())
        {
            // ASCII black and white
            // case '1': {
            //      
            // } break;
            // ASCII monochome
            // case '2': {
            //      
            // } break;
            // ASCII RGB
            // case '3': {
            // 
            // } break;
            // BINARY black and whie
            // case '4': {
            //      
            // } break;
            // BINARY monochome
            // case '5': {
            //      
            // } break;
            // BINARY RGB
            case '6': {
                char c;
                size_t width = 0, height = 0;
                uint16_t size = 0;

                skipAll(strm, " \t\r\n"); // whitespace
				if(strm.peek() == '#')
				{
					skipOne(strm, "\n");
					skipAll(strm, " \t\r\n");
				}

                skipNoneC(strm, " \t\r\n", c) width = (width * 10) + (c - '0');

                skipAll(strm, " \t\r\n"); // whitespace
                skipNoneC(strm, " \t\r\n", c) height = (height * 10) + (c - '0');

                skipAll(strm, " \t\r\n"); // whitespace
                skipOneC(strm, " \t\r\n", c) size = (size * 10) + (c - '0');
                // NOTE: skipOne already handles the single white space after size

                rtn.resize(width, height);
                for(size_t _y = 0; _y < height; _y++)
                {
                    size_t y = invert_y ? (height - 1) - _y : _y;
                    for(size_t x = 0; x < width; x++)
                    {
                        if(size < 256)
                        {
                            rtn[y][x].r = strm.get();
                            rtn[y][x].g = strm.get();
                            rtn[y][x].b = strm.get();
                        }
                        else
                        {
                            rtn[y][x].r = strm.get() << 8;
                            rtn[y][x].r += strm.get();
                            rtn[y][x].g = strm.get() << 8;
                            rtn[y][x].g += strm.get();
                            rtn[y][x].b = strm.get() << 8;
                            rtn[y][x].b += strm.get();
                        }
                    }
                }
            } break;
            // case '7': {
            //      
            // } break;
            default: {
                return rtn;
            }
        }
        return rtn;
    }
}