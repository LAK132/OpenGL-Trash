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
#include <stdint.h>
#include <GL/gl3w.h>

#ifndef LAK_COLOR
#define LAK_COLOR

namespace lak
{
    using std::conditional;
    struct null_t{};

    template<GLenum gltype> struct gltct1 { typedef null_t type; };
    template<> struct gltct1<GL_UNSIGNED_BYTE> { typedef uint8_t type; };
    template<> struct gltct1<GL_BYTE> { typedef int8_t type; };
    template<> struct gltct1<GL_UNSIGNED_SHORT> { typedef uint16_t type; };
    template<> struct gltct1<GL_SHORT> { typedef int16_t type; };
    template<> struct gltct1<GL_UNSIGNED_INT> { typedef uint32_t type; };
    template<> struct gltct1<GL_INT> { typedef int32_t type; };
    template<> struct gltct1<GL_FLOAT> { typedef float type; };

    template<GLenum gltype> struct gltct2 { typedef null_t type; };
    template<> struct gltct2<GL_UNSIGNED_BYTE_3_3_2> { typedef uint8_t type; };
    template<> struct gltct2<GL_UNSIGNED_BYTE_2_3_3_REV> { typedef uint8_t type; };
    template<> struct gltct2<GL_UNSIGNED_SHORT_5_6_5> { typedef uint16_t type; };
    template<> struct gltct2<GL_UNSIGNED_SHORT_5_6_5_REV> { typedef uint16_t type; };

    template<GLenum gltype> struct gltct3 { typedef null_t type; };
    template<> struct gltct3<GL_UNSIGNED_SHORT_4_4_4_4> { typedef uint16_t type; };
    template<> struct gltct3<GL_UNSIGNED_SHORT_4_4_4_4_REV> { typedef uint16_t type; };
    template<> struct gltct3<GL_UNSIGNED_SHORT_5_5_5_1> { typedef uint16_t type; };
    template<> struct gltct3<GL_UNSIGNED_SHORT_1_5_5_5_REV> { typedef uint16_t type; };
    template<> struct gltct3<GL_UNSIGNED_INT_8_8_8_8> { typedef uint32_t type; };
    template<> struct gltct3<GL_UNSIGNED_INT_8_8_8_8_REV> { typedef uint32_t type; };
    template<> struct gltct3<GL_UNSIGNED_INT_10_10_10_2> { typedef uint32_t type; };
    template<> struct gltct3<GL_UNSIGNED_INT_2_10_10_10_REV> { typedef uint32_t type; };

    template<GLenum gltype, typename T> struct itgltct { enum { result = true }; };
    template<GLenum gltype> struct itgltct<gltype, null_t> { enum { result = false }; };

    template<GLenum gltype> struct gltct_t
    {
        typedef typename conditional<itgltct<gltype, typename gltct1<gltype>::type>::result, gltct1<gltype>,
            typename conditional<itgltct<gltype, typename gltct2<gltype>::type>::result, gltct2<gltype>,
                typename conditional<itgltct<gltype, typename gltct3<gltype>::type>::result, gltct3<gltype>, null_t>::type
            >::type
        >::type type;
    };

    template<GLenum glformat, GLenum gltype, typename T> struct _color_t;

    // RX
    template<GLenum gltype> struct _color_t<GL_RED, gltype, gltct1<gltype>>{
        typedef _color_t<GL_RED, gltype, gltct1<gltype>> ctype;
        typedef typename gltct1<gltype>::type type;
        type r = 0;
        _color_t(){}
        _color_t(type R){ r = R; }
        _color_t(ctype&& other) { *this = other; }
        _color_t(const ctype& other) { *this = other; }
        ctype& operator=(ctype&& other) { return *this = other; }
        ctype& operator=(const ctype& other) {
            r = other.r;
            return *this;
        }
    };
    // RX GX
    template<GLenum gltype> struct _color_t<GL_RG, gltype, gltct1<gltype>>{
        typedef _color_t<GL_RG, gltype, gltct1<gltype>> ctype;
        typedef typename gltct1<gltype>::type type;
        type r = 0; type g = 0;
        _color_t(){}
        _color_t(type R, type G){ r = R; g = G; }
        _color_t(ctype&& other) { *this = other; }
        _color_t(const ctype& other) { *this = other; }
        ctype& operator=(ctype&& other) { return *this = other; }
        ctype& operator=(const ctype& other) {
            r = other.r;
            g = other.g;
            return *this;
        }
    };
    // RX GX BX
    template<GLenum gltype> struct _color_t<GL_RGB, gltype, gltct1<gltype>>{
        typedef _color_t<GL_RGB, gltype, gltct1<gltype>> ctype;
        typedef typename gltct1<gltype>::type type;
        type r = 0; type g = 0; type b = 0;
        _color_t(){}
        _color_t(type R, type G, type B){ r = R; g = G; b = B; }
        _color_t(ctype&& other) { *this = other; }
        _color_t(const ctype& other) { *this = other; }
        ctype& operator=(ctype&& other) { return *this = other; }
        ctype& operator=(const ctype& other) {
            r = other.r;
            g = other.g;
            b = other.b;
            return *this;
        }
    };
    // BX GX RX
    template<GLenum gltype> struct _color_t<GL_BGR, gltype, gltct1<gltype>>{
        typedef _color_t<GL_BGR, gltype, gltct1<gltype>> ctype;
        typedef typename gltct1<gltype>::type type;
        type b = 0; type g = 0; type r = 0;
        _color_t(){}
        _color_t(type B, type G, type R){ r = R; g = G; b = B; }
        _color_t(ctype&& other) { *this = other; }
        _color_t(const ctype& other) { *this = other; }
        ctype& operator=(ctype&& other) { return *this = other; }
        ctype& operator=(const ctype& other) {
            r = other.r;
            g = other.g;
            b = other.b;
            return *this;
        }
    };
    // RX GX BX AX
    template<GLenum gltype> struct _color_t<GL_RGBA, gltype, gltct1<gltype>>{
        typedef _color_t<GL_RGBA, gltype, gltct1<gltype>> ctype;
        typedef typename gltct1<gltype>::type type;
        type r = 0; type g = 0; type b = 0; type a = 0;
        _color_t(){}
        _color_t(type R, type G, type B, type A){ r = R; g = G; b = B; a = A; }
        _color_t(ctype&& other) { *this = other; }
        _color_t(const ctype& other) { *this = other; }
        ctype& operator=(ctype&& other) { return *this = other; }
        ctype& operator=(const ctype& other) {
            r = other.r;
            g = other.g;
            b = other.b;
            a = other.a;
            return *this;
        }
    };
    // BX GX RX AX
    template<GLenum gltype> struct _color_t<GL_BGRA, gltype, gltct1<gltype>>{
        typedef _color_t<GL_BGRA, gltype, gltct1<gltype>> ctype;
        typedef typename gltct1<gltype>::type type;
        type b = 0; type g = 0; type r = 0; type a = 0;
        _color_t(){}
        _color_t(type B, type G, type R, type A){ r = R; g = G; b = B; a = A; }
        _color_t(ctype&& other) { *this = other; }
        _color_t(const ctype& other) { *this = other; }
        ctype& operator=(ctype&& other) { return *this = other; }
        ctype& operator=(const ctype& other) {
            r = other.r;
            g = other.g;
            b = other.b;
            a = other.a;
            return *this;
        }
    };
    
    // R3 G3 B2 / B2 G3 R3
    // R5 G6 B5 / B5 G6 R5
    template<GLenum gltype> struct _color_t<GL_RGB, gltype, gltct2<gltype>>{
        typedef _color_t<GL_RGB, gltype, gltct2<gltype>> ctype;
        typedef typename gltct1<gltype>::type type;
        type rgb;
        _color_t(){}
        _color_t(type RGB){ rgb = RGB; }
        _color_t(ctype&& other) { *this = other; }
        _color_t(const ctype& other) { *this = other; }
        ctype& operator=(ctype&& other) { return *this = other; }
        ctype& operator=(const ctype& other) {
            rgb = other.rgb;
            return *this;
        }
    };

    // R4 G4 B4 A4 / A4 B4 G4 R4
    // R5 G5 B5 A1 / A1 B5 G5 R5
    // R8 G8 B8 A8 / A8 B8 G8 R8
    // R10 G10 B10 A2 / B10 G10 R10 A2
    template<GLenum gltype> struct _color_t<GL_RGBA, gltype, gltct3<gltype>>{
        typedef _color_t<GL_RGBA, gltype, gltct3<gltype>> ctype;
        typedef typename gltct1<gltype>::type type;
        type rgba;
        _color_t(){}
        _color_t(type RGBA){ rgba = RGBA; }
        _color_t(ctype&& other) { *this = other; }
        _color_t(const ctype& other) { *this = other; }
        ctype& operator=(ctype&& other) { return *this = other; }
        ctype& operator=(const ctype& other) {
            rgba = other.rgba;
            return *this;
        }
    };

    // B4 G4 R4 A4 / A4 R4 G4 B4
    // B5 G5 R5 A1 / A1 R5 G5 B5
    // B8 G8 R8 A8 / A8 R8 G8 B8
    // B10 G10 R10 A2 / R10 G10 B10 A2
    template<GLenum gltype> struct _color_t<GL_BGRA, gltype, gltct3<gltype>>{
        typedef _color_t<GL_BGRA, gltype, gltct3<gltype>> ctype;
        typedef typename gltct1<gltype>::type type;
        type bgra;
        _color_t(){}
        _color_t(type BGRA){ bgra = BGRA; }
        _color_t(ctype&& other) { *this = other; }
        _color_t(const ctype& other) { *this = other; }
        ctype& operator=(ctype&& other) { return *this = other; }
        ctype& operator=(const ctype& other) {
            bgra = other.bgra;
            return *this;
        }
    };

    template<GLenum glformat, GLenum gltype> using color_t = _color_t<glformat, gltype, typename gltct_t<gltype>::type>;

    typedef color_t<GL_RED, GL_UNSIGNED_BYTE> colorR8_t;
    typedef color_t<GL_RG, GL_UNSIGNED_BYTE> colorRG8_t;
    typedef color_t<GL_RGB, GL_UNSIGNED_BYTE> colorRGB8_t;
    typedef color_t<GL_RGBA, GL_UNSIGNED_BYTE> colorRGBA8_t;
    typedef color_t<GL_BGR, GL_UNSIGNED_BYTE> colorBGR8_t;
    typedef color_t<GL_BGRA, GL_UNSIGNED_BYTE> colorBGRA8_t;

    typedef color_t<GL_RED, GL_BYTE> colorRs8_t;
    typedef color_t<GL_RG, GL_BYTE> colorRGs8_t;
    typedef color_t<GL_RGB, GL_BYTE> colorRGBs8_t;
    typedef color_t<GL_RGBA, GL_BYTE> colorRGBAs8_t;
    typedef color_t<GL_BGR, GL_BYTE> colorBGRs8_t;
    typedef color_t<GL_BGRA, GL_BYTE> colorBGRAs8_t;

    typedef color_t<GL_RED, GL_UNSIGNED_SHORT> colorR16_t;
    typedef color_t<GL_RG, GL_UNSIGNED_SHORT> colorRG16_t;
    typedef color_t<GL_RGB, GL_UNSIGNED_SHORT> colorRGB16_t;
    typedef color_t<GL_RGBA, GL_UNSIGNED_SHORT> colorRGBA16_t;
    typedef color_t<GL_BGR, GL_UNSIGNED_SHORT> colorBGR16_t;
    typedef color_t<GL_BGRA, GL_UNSIGNED_SHORT> colorBGRA16_t;

    typedef color_t<GL_RED, GL_SHORT> colorRs16_t;
    typedef color_t<GL_RG, GL_SHORT> colorRGs16_t;
    typedef color_t<GL_RGB, GL_SHORT> colorRGBs16_t;
    typedef color_t<GL_RGBA, GL_SHORT> colorRGBAs16_t;
    typedef color_t<GL_BGR, GL_SHORT> colorBGRs16_t;
    typedef color_t<GL_BGRA, GL_SHORT> colorBGRAs16_t;

    typedef color_t<GL_RED, GL_UNSIGNED_INT> colorR32_t;
    typedef color_t<GL_RG, GL_UNSIGNED_INT> colorRG32_t;
    typedef color_t<GL_RGB, GL_UNSIGNED_INT> colorRGB32_t;
    typedef color_t<GL_RGBA, GL_UNSIGNED_INT> colorRGBA32_t;
    typedef color_t<GL_BGR, GL_UNSIGNED_INT> colorBGR32_t;
    typedef color_t<GL_BGRA, GL_UNSIGNED_INT> colorBGRA32_t;

    typedef color_t<GL_RED, GL_INT> colorRs32_t;
    typedef color_t<GL_RG, GL_INT> colorRGs32_t;
    typedef color_t<GL_RGB, GL_INT> colorRGBs32_t;
    typedef color_t<GL_RGBA, GL_INT> colorRGBAs32_t;
    typedef color_t<GL_BGR, GL_INT> colorBGRs32_t;
    typedef color_t<GL_BGRA, GL_INT> colorBGRAs32_t;

    typedef color_t<GL_RED, GL_FLOAT> colorRf_t;
    typedef color_t<GL_RG, GL_FLOAT> colorRGf_t;
    typedef color_t<GL_RGB, GL_FLOAT> colorRGBf_t;
    typedef color_t<GL_RGBA, GL_FLOAT> colorRGBAf_t;
    typedef color_t<GL_BGR, GL_FLOAT> colorBGRf_t;
    typedef color_t<GL_BGRA, GL_FLOAT> colorBGRAf_t;

    typedef color_t<GL_RGB, GL_UNSIGNED_BYTE_3_3_2> colorR3G3B2_t;
    typedef color_t<GL_RGB, GL_UNSIGNED_BYTE_2_3_3_REV> colorB2G3R3_t;

    typedef color_t<GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4> colorR4G4B4A4_t;
    typedef color_t<GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4_REV> colorA4B4G4R4_t;
    typedef color_t<GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4> colorB4G4R4A4_t;
    typedef color_t<GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV> colorA4R4G4B4_t;

    typedef color_t<GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1> colorR5G5B5A1_t;
    typedef color_t<GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV> colorA1B5G5R5_t;
    typedef color_t<GL_BGRA, GL_UNSIGNED_SHORT_5_5_5_1> colorB5G5R5A1_t;
    typedef color_t<GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV> colorA1R5G5B5_t;

    typedef color_t<GL_RGBA, GL_UNSIGNED_INT_8_8_8_8> colorR8G8B8A8_t;
    typedef color_t<GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV> colorA8B8G8R8_t;
    typedef color_t<GL_BGRA, GL_UNSIGNED_INT_8_8_8_8> colorB8G8R8A8_t;
    typedef color_t<GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV> colorA8R8G8B8_t;

    typedef color_t<GL_RGBA, GL_UNSIGNED_INT_10_10_10_2> colorR10G10B10A2_t;
    typedef color_t<GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV> colorA2B10G10R10_t;
    typedef color_t<GL_BGRA, GL_UNSIGNED_INT_10_10_10_2> colorB10G10R10A2_t;
    typedef color_t<GL_BGRA, GL_UNSIGNED_INT_2_10_10_10_REV> colorA2R10G10B10_t;
}

#endif // LAK_COLOR