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

#include <stdint.h>
#include <vector>
#include <GL/gl3w.h>
#include <lak/color.hpp>

#ifndef LAK_IMAGE_H
#define LAK_IMAGE_H

namespace lak
{
    using std::vector;

    template <typename C> struct image_t;

    template <GLenum glformat, GLenum gltype>
    struct image_t<color_t<glformat, gltype>>
    {
        typedef color_t<glformat, gltype> col_t;
        typedef typename col_t::type chan_t;
        size_t w = 0;
        size_t h = 0;
        vector<col_t> pixels;
        image_t(){}
        ~image_t(){}
        void resize(size_t W, size_t H)
        {
            w = W;
            h = H;
            pixels.resize(w * h);
        }
        image_t(size_t W, size_t H) { resize(W, H); }
        image_t(size_t W, size_t H, chan_t* pix)
        {
            resize(W, H);
            for(size_t i = 0; i < w * h; i++)
            {
                switch(gltype)
                {
                    case GL_UNSIGNED_BYTE:
                    case GL_BYTE:
                    case GL_UNSIGNED_SHORT:
                    case GL_SHORT:
                    case GL_UNSIGNED_INT:
                    case GL_INT:
                    case GL_FLOAT: {
                        switch(glformat)
                        {
                            case GL_RED: {
                                pixels[i] = {pix[i]};
                            } break;
                            case GL_RG: {
                                size_t j = i * 2;
                                pixels[i] = {pix[j], pix[j+1]};
                            } break;
                            case GL_RGB:
                            case GL_BGR: {
                                size_t j = i * 3;
                                pixels[i] = {pix[j], pix[j+1], pix[j+2]};
                            } break;
                            case GL_RGBA:
                            case GL_BGRA: {
                                size_t j = i * 4;
                                pixels[i] = {pix[j], pix[j+1], pix[j+2], pix[j+3]};
                            } break;
                            default: DEBUG("INVALID TYPE\n"); return;
                        }
                    } break;
                    case GL_UNSIGNED_BYTE_3_3_2:
                    case GL_UNSIGNED_BYTE_2_3_3_REV:
                    case GL_UNSIGNED_SHORT_5_6_5:
                    case GL_UNSIGNED_SHORT_5_6_5_REV:
                    case GL_UNSIGNED_SHORT_4_4_4_4:
                    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
                    case GL_UNSIGNED_SHORT_5_5_5_1:
                    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
                    case GL_UNSIGNED_INT_8_8_8_8:
                    case GL_UNSIGNED_INT_8_8_8_8_REV:
                    case GL_UNSIGNED_INT_10_10_10_2:
                    case GL_UNSIGNED_INT_2_10_10_10_REV: {
                        pixels[i] = pix[i];
                    } break;
                    default: DEBUG("INVALID TYPE\n"); return;
                }
            }
        }
        image_t(size_t W, const vector<col_t>& pix)
        {
            pixels = pix;
            resize(W, pix.size() / W); // resize AFTER copy
        }
        image_t(size_t W, vector<col_t>&& pix)
        {
            pixels = pix;
            resize(W, pix.size() / W); // resize AFTER copy
        }
        image_t(const image_t<col_t>& other) { *this = other; }
        image_t(image_t<col_t>&& other) { *this = other; }
        image_t<col_t>& operator=(const image_t<col_t>& other)
        {
            pixels = other.pixels;
            resize(other.w, other.h); // resize AFTER copy
            return *this;
        }
        image_t<col_t>& operator=(image_t<col_t>&& other) { return *this = other; }
        col_t* operator[](size_t y) { return &(pixels[y * w]); }
    };

    typedef image_t<colorR8_t> imageR8_t;
    typedef image_t<colorRG8_t> imageRG8_t;
    typedef image_t<colorRGB8_t> imageRGB8_t;
    typedef image_t<colorRGBA8_t> imageRGBA8_t;
    typedef image_t<colorBGR8_t> imageBGR8_t;
    typedef image_t<colorBGRA8_t> imageBGRA8_t;

    typedef image_t<colorRs8_t> imageRs8_t;
    typedef image_t<colorRGs8_t> imageRGs8_t;
    typedef image_t<colorRGBs8_t> imageRGBs8_t;
    typedef image_t<colorRGBAs8_t> imageRGBAs8_t;
    typedef image_t<colorBGRs8_t> imageBGRs8_t;
    typedef image_t<colorBGRAs8_t> imageBGRAs8_t;

    typedef image_t<colorR16_t> imageR16_t;
    typedef image_t<colorRG16_t> imageRG16_t;
    typedef image_t<colorRGB16_t> imageRGB16_t;
    typedef image_t<colorRGBA16_t> imageRGBA16_t;
    typedef image_t<colorBGR16_t> imageBGR16_t;
    typedef image_t<colorBGRA16_t> imageBGRA16_t;

    typedef image_t<colorRs16_t> imageRs16_t;
    typedef image_t<colorRGs16_t> imageRGs16_t;
    typedef image_t<colorRGBs16_t> imageRGBs16_t;
    typedef image_t<colorRGBAs16_t> imageRGBAs16_t;
    typedef image_t<colorBGRs16_t> imageBGRs16_t;
    typedef image_t<colorBGRAs16_t> imageBGRAs16_t;

    typedef image_t<colorR32_t> imageR32_t;
    typedef image_t<colorRG32_t> imageRG32_t;
    typedef image_t<colorRGB32_t> imageRGB32_t;
    typedef image_t<colorRGBA32_t> imageRGBA32_t;
    typedef image_t<colorBGR32_t> imageBGR32_t;
    typedef image_t<colorBGRA32_t> imageBGRA32_t;

    typedef image_t<colorRs32_t> imageRs32_t;
    typedef image_t<colorRGs32_t> imageRGs32_t;
    typedef image_t<colorRGBs32_t> imageRGBs32_t;
    typedef image_t<colorRGBAs32_t> imageRGBAs32_t;
    typedef image_t<colorBGRs32_t> imageBGRs32_t;
    typedef image_t<colorBGRAs32_t> imageBGRAs32_t;

    typedef image_t<colorRf_t> imageRf_t;
    typedef image_t<colorRGf_t> imageRGf_t;
    typedef image_t<colorRGBf_t> imageRGBf_t;
    typedef image_t<colorRGBAf_t> imageRGBAf_t;
    typedef image_t<colorBGRf_t> imageBGRf_t;
    typedef image_t<colorBGRAf_t> imageBGRAf_t;

    typedef image_t<colorR3G3B2_t> imageR3G3B2_t;
    typedef image_t<colorB2G3R3_t> imageB2G3R3_t;

    typedef image_t<colorR4G4B4A4_t> imageR4G4B4A4_t;
    typedef image_t<colorA4B4G4R4_t> imageA4B4G4R4_t;
    typedef image_t<colorB4G4R4A4_t> imageB4G4R4A4_t;
    typedef image_t<colorA4R4G4B4_t> imageA4R4G4B4_t;

    typedef image_t<colorR5G5B5A1_t> imageR5G5B5A1_t;
    typedef image_t<colorA1B5G5R5_t> imageA1B5G5R5_t;
    typedef image_t<colorB5G5R5A1_t> imageB5G5R5A1_t;
    typedef image_t<colorA1R5G5B5_t> imageA1R5G5B5_t;

    typedef image_t<colorR8G8B8A8_t> imageR8G8B8A8_t;
    typedef image_t<colorA8B8G8R8_t> imageA8B8G8R8_t;
    typedef image_t<colorB8G8R8A8_t> imageB8G8R8A8_t;
    typedef image_t<colorA8R8G8B8_t> imageA8R8G8B8_t;

    typedef image_t<colorR10G10B10A2_t> imageR10G10B10A2_t;
    typedef image_t<colorA2B10G10R10_t> imageA2B10G10R10_t;
    typedef image_t<colorB10G10R10A2_t> imageB10G10R10A2_t;
    typedef image_t<colorA2R10G10B10_t> imageA2R10G10B10_t;
}

#endif // LAK_IMAGE_H