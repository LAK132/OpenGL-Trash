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

#include <vector>
#include <memory>
#include <GL/gl3w.h>
#include <lak/image.hpp>

#ifndef LAK_TEXTURE_H
#define LAK_TEXTURE_H

namespace lak
{
    using std::vector;
    using std::shared_ptr;
    struct texparam_t
    {
        GLenum pname = 0xBAADF00D;
        enum type { NONE, INT, FLOAT, ARRAY } tag = NONE;
        union
        {
            GLint i;
            GLfloat f;
            shared_ptr<void> arr;
        };
        void retype(type t)
        {
            if(tag != t && tag == ARRAY)
            {
                arr.~shared_ptr();
            }
            tag = t;
            if(tag == ARRAY)
            {
                new (&arr) shared_ptr<void>;
            }
        }
        texparam_t(){}
        ~texparam_t()
        {
            retype(NONE);
        }
        texparam_t(const texparam_t& other) { *this = other; }
        texparam_t(texparam_t&& other) { *this = other; }
        texparam_t(GLenum pn, GLint val)
        {
            retype(INT);
            pname = pn;
            i = val;
        }
        texparam_t(GLenum pn, GLfloat val)
        {
            retype(FLOAT);
            pname = pn;
            f = val;
        }
        texparam_t(GLenum pn, shared_ptr<void> val)
        {
            retype(ARRAY);
            pname = pn;
            arr = val;
        }
        texparam_t& operator=(const texparam_t& other)
        {
            pname = other.pname;
            retype(other.tag);
            switch(tag)
            {
                case INT: i = other.i; break;
                case FLOAT: f = other.f; break;
                case ARRAY: arr = other.arr; break;
                default: break;
            }
            return *this;
        }
        texparam_t& operator=(texparam_t&& other) { return *this = other; }
        void apply(GLenum target)
        {
            switch(pname)
            {
                case GL_TEXTURE_BORDER_COLOR:
                    if(tag == ARRAY) glTexParameterfv(target, pname, static_cast<GLfloat*>(arr.get()));
                    break;
                case GL_TEXTURE_SWIZZLE_RGBA:
                    if(tag == ARRAY) glTexParameteriv(target, pname, static_cast<GLint*>(arr.get()));
                    break;
                case GL_TEXTURE_LOD_BIAS:
                    if(tag == FLOAT) glTexParameterf(target, pname, f);
                    break;
                case GL_TEXTURE_BASE_LEVEL:
                case GL_TEXTURE_MIN_FILTER:
                case GL_TEXTURE_MAG_FILTER:
                case GL_TEXTURE_MIN_LOD:
                case GL_TEXTURE_MAX_LOD:
                case GL_TEXTURE_MAX_LEVEL:
                case GL_TEXTURE_WRAP_S:
                case GL_TEXTURE_WRAP_T:
                case GL_TEXTURE_WRAP_R:
                case GL_TEXTURE_SWIZZLE_R:
                case GL_TEXTURE_SWIZZLE_G:
                case GL_TEXTURE_SWIZZLE_B:
                case GL_TEXTURE_SWIZZLE_A:
                case GL_TEXTURE_COMPARE_FUNC:
                case GL_TEXTURE_COMPARE_MODE:
                    if(tag == INT) glTexParameteri(target, pname, i);
                    break;
                default: DEBUG("INVALID TYPE\n"); return;
            }
        }
    };

    template <GLenum glformat, GLenum gltype>
    void imageToTexture(GLenum ttype, GLint level, GLint cformat, GLint border, const image_t<color_t<glformat, gltype>>& img)
    {
        if(img.w > 0 && img.h > 0) glTexImage2D(ttype, level, cformat, img.w, img.h, border, glformat, gltype, &(img.pixels[0]));
    }

    template <GLenum glformat, GLenum gltype>
    void imageToTexture(GLenum ttype, GLint level, GLint cformat, GLint border, image_t<color_t<glformat, gltype>>&& img)
    {
        if(img.w > 0 && img.h > 0) glTexImage2D(ttype, level, cformat, img.w, img.h, border, glformat, gltype, &(img.pixels[0]));
    }

    struct texture_t
    {
    private:
        bool generated = false;
    public:
        size_t w = 0; 
        size_t h = 0;
        GLuint tex;
        GLenum texType = GL_TEXTURE_2D;
        vector<texparam_t> params;
        texture_t(){}
        ~texture_t()
        {
            if(!generated) return;
            glDeleteTextures(1, &tex);
        }
        template <GLenum glformat, GLenum gltype>
        texture_t(GLenum ttype, GLint level, GLint cformat, GLint border, const image_t<color_t<glformat, gltype>>& img, const vector<texparam_t>& prms)
        {
            generate(ttype, level, cformat, border, img, prms);
        }
        template <GLenum glformat, GLenum gltype>
        texture_t(GLenum ttype, GLint level, GLint cformat, GLint border, image_t<color_t<glformat, gltype>>&& img, const vector<texparam_t>& prms)
        {
            generate(ttype, level, cformat, border, img, prms);
        }
        template <GLenum glformat, GLenum gltype>
        texture_t(GLenum ttype, GLint level, GLint cformat, GLint border, const image_t<color_t<glformat, gltype>>& img, vector<texparam_t>&& prms)
        {
            generate(ttype, level, cformat, border, img, prms);
        }
        template <GLenum glformat, GLenum gltype>
        texture_t(GLenum ttype, GLint level, GLint cformat, GLint border, image_t<color_t<glformat, gltype>>&& img, vector<texparam_t>&& prms)
        {
            generate(ttype, level, cformat, border, img, prms);
        }
        template <GLenum glformat, GLenum gltype>
        void generate(GLenum ttype, GLint level, GLint cformat, GLint border, const image_t<color_t<glformat, gltype>>& img, const vector<texparam_t>& prms)
        {
            texType = ttype;
            w = img.w;
            h = img.h;
            params = prms;
            if(!generated)
            {
                glGenTextures(1, &tex);
                generated = true;
            }
            update();
            imageToTexture(texType, level, cformat, border, img);
        }
        template <GLenum glformat, GLenum gltype>
        void generate(GLenum ttype, GLint level, GLint cformat, GLint border, image_t<color_t<glformat, gltype>>&& img, const vector<texparam_t>& prms)
        {
            generate(ttype, level, cformat, border, img, prms);
        }
        template <GLenum glformat, GLenum gltype>
        void generate(GLenum ttype, GLint level, GLint cformat, GLint border, const image_t<color_t<glformat, gltype>>& img, vector<texparam_t>&& prms)
        {
            generate(ttype, level, cformat, border, img, prms);
        }
        template <GLenum glformat, GLenum gltype>
        void generate(GLenum ttype, GLint level, GLint cformat, GLint border, image_t<color_t<glformat, gltype>>&& img, vector<texparam_t>&& prms)
        {
            generate(ttype, level, cformat, border, img, prms);
        }
        void bind()
        {
            if(!generated) return;
            glBindTexture(texType, tex);
        }
        void update()
        {
            if(!generated) return;
            bind();
            for(auto it = params.begin(); it != params.end(); it++)
            {
                it->apply(texType);
            }
        }
    };
}

#endif // LAK_TEXTURE_H