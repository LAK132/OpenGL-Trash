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

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

#include <lak/stride_vector.hpp>
#include <lak/stream_util.hpp>

#include <lak/texture.hpp>
#include <GL/gl3w.h>

#ifndef LAK_GRAPHICS_H
#define LAK_GRAPHICS_H

namespace lak
{
    using std::shared_ptr;
    using std::make_shared;
    using std::vector;
    using std::string;
    using std::unordered_map;
    using std::istream;
    using std::endl;
    struct shader_t;
    struct mesh_t;

    bool initShader(const string& code, GLuint program, GLenum type);
    bool initShader(string&& code, GLuint program, GLenum type);
    bool linkProgram(GLuint program);

    struct mesh_t
    {
        struct buffer_t
        {
        private:
            bool isInit = false;
        public:
            GLuint vertArr = 0, vertBuff = 0, indxBuff = 0;
            void init();
            void bind();
            ~buffer_t();
        };
        struct element_t
        {
        private:
            bool isInit = false;
        public:
            stride_vector data;
            size_t size = 0;
            size_t stride = 0;
            size_t interlStride = 0;
            GLintptr offset = 0; // buffer position
            bool normalized = false;
            bool active = false;
            element_t();
            element_t(size_t dsize, size_t dlength);
            void init(size_t dsize, size_t dlength);
            template <typename T>
            T* init(size_t dlength)
            {
                init(sizeof(T), dlength);
                return data.get<T>();
            }
        };
    private:
        bool index_mode = false;
        size_t index_count = 0;
        size_t buffer_size = 0;
        size_t buffer_stride = 0;
    public:
        shared_ptr<shader_t> shader;
        buffer_t buffer;
        unordered_map<string, element_t> elements;
        unordered_map<string, shared_ptr<texture_t>> textures;
        vector<GLuint> index;
        GLenum drawMode = GL_TRIANGLES;
        void update();
        void draw();
    };

    struct shader_t
    {
        struct element_t
        {
            GLint position = -1; // program position
            GLenum type = GL_FLOAT;
            GLint size = 4; // element count
            string name = ""; // name in shader
            bool active = false;
        };
        shared_ptr<GLint> program;
        unordered_map<string, element_t> attributes;
        unordered_map<string, element_t> uniforms;
        shader_t();
        ~shader_t();
        shader_t(const shader_t& other);
        shader_t(shader_t&& other);
        shader_t& operator=(const shader_t& other);
        shader_t& operator=(shader_t&& other);
        bool init(const string& vshader, const string& fshader);
        void initAttribs();
        void setUniform(const string& name, const void* data, GLsizei size = 1, GLboolean transpose = GL_FALSE);
        void enable(unordered_map<string, mesh_t::element_t>* attrs);
        void disable();
        GLint operator*() const;
    };
}

#endif // LAK_GRAPHICS_H

// #define LAK_GRAPHICS_IMPLEM
#ifdef LAK_GRAPHICS_IMPLEM
#ifndef LAK_GRAPHICS_HAS_IMPLEM
#define LAK_GRAPHICS_HAS_IMPLEM

namespace lak
{
    bool initShader(const string& code, GLuint program, GLenum type)
    {
        GLuint shader = glCreateShader(type);
        GLchar* shadercstr = (GLchar*)&(code[0]);
        glShaderSource(shader, 1, &shadercstr, NULL);
        glCompileShader(shader);
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if(!compiled)
        {
            GLint msgSize = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &msgSize);
            string msg; msg.resize(msgSize);
            glGetShaderInfoLog(shader, msgSize, NULL, &(msg[0]));
            DEBUG("Error compiling shader " << code << endl << msg << endl);
            return false;
        }
        glAttachShader(program, shader);
        return true;
    }
    bool initShader(string&& code, GLuint program, GLenum type)
    {
        return initShader(code, program, type);
    }
    bool linkProgram(GLuint program)
    {
        glLinkProgram(program);
        GLint linked;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if(!linked)
        {
            GLint msgSize;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &msgSize);
            string msg; msg.resize(msgSize);
            glGetProgramInfoLog(program, msgSize, NULL, &(msg[0]));
            DEBUG("Shader program failed to link" << endl << msg << endl);
            return false;
        }
        return true;
    }

    // shader_t
    shader_t::shader_t(){}

    shader_t::~shader_t()
    {
        if(program.unique()) glDeleteProgram(*program);
    }

    shader_t::shader_t(const shader_t& other)
    { 
        *this = other; 
    }

    shader_t::shader_t(shader_t&& other)
    { 
        *this = other; 
    }
    
    shader_t& shader_t::operator=(const shader_t& other)
    { 
        program = other.program;
        attributes = other.attributes;
        uniforms = other.uniforms;
        return *this; 
    }

    shader_t& shader_t::operator=(shader_t&& other)
    { 
        return *this = other; 
    }

    bool shader_t::init(const string& vshader, const string& fshader)
    {
        program = make_shared<GLint>(glCreateProgram());
        if(!initShader(vshader, *program, GL_VERTEX_SHADER)) return false;
        if(!initShader(fshader, *program, GL_FRAGMENT_SHADER)) return false;
        if(!linkProgram(*program)) return false;
        initAttribs();
        return true;
    }

    void shader_t::initAttribs()
    {
		glUseProgram(*program);

        GLint count;
        vector<GLchar> name;
        GLsizei length;
        GLint size;
        GLenum type;

        GLint nameLen = 0;
        glGetProgramiv(*program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &nameLen);
        name.resize(nameLen);

        attributes.clear();
        glGetProgramiv(*program, GL_ACTIVE_ATTRIBUTES, &count);
        for (GLint i = 0; i < count; i++)
        {
            // FFR: type = advanced type ie GL_UNSIGNED_INT_VEC4, size = units of type (?)
            // elem.type = basic type ie GL_UNSIGNED_INT, elem.size = units of elem.type
            // where type = GL_FLOAT_VEC4 and size = 1 then elem.type = GL_FLOAT and elem.size = 4 (??)
            // https://www.gamedev.net/forums/topic/545284-glsl-glgetactiveattrib---the-size-parameter/
            for(size_t i = 0; i < name.size(); i++) name[i] = '\0';
            glGetActiveAttrib(*program, (GLuint)i, nameLen, &length, &size, &type, &name[0]);
            element_t elem;
            elem.position = glGetAttribLocation(*program, &name[0]);
            // glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_TYPE, (GLint*)&elem.type);
            // glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, &elem.size);
            elem.name = &name[0];
            attributes[elem.name] = elem;
        }

        glGetProgramiv(*program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &nameLen);
        name.resize(nameLen);
        
        uniforms.clear();
        glGetProgramiv(*program, GL_ACTIVE_UNIFORMS, &count);
        for (GLint i = 0; i < count; i++)
        {
            // FFR: type == elem.type
            // size == elem.size (number of type in array, IF it is an array, else 1)
            for(size_t i = 0; i < name.size(); i++) name[i] = '\0';
            element_t elem;
            glGetActiveUniform(*program, (GLuint)i, nameLen, &length, &elem.size, &elem.type, &name[0]);
            elem.position = glGetUniformLocation(*program, &name[0]);
            elem.name = &name[0];
            uniforms[elem.name] = elem;
        }
    }

    void shader_t::setUniform(const string& name, const void* data, GLsizei size, GLboolean transpose)
    {
        if (program.use_count() <= 0) return;
        auto it = uniforms.find(name);
        if(it == uniforms.end()) return;
        glUseProgram(*program);
        element_t& elem = it->second;
        if(elem.position < 0) return;
        switch (elem.type)
        {
            case GL_FLOAT: 
                glUniform1fv(elem.position, size, (GLfloat*)data); break;
            case GL_FLOAT_VEC2:
                glUniform2fv(elem.position, size, (GLfloat*)data); break;
            case GL_FLOAT_VEC3:
                glUniform3fv(elem.position, size, (GLfloat*)data); break;
            case GL_FLOAT_VEC4:
                glUniform4fv(elem.position, size, (GLfloat*)data); break;
            case GL_FLOAT_MAT2:
                glUniformMatrix2fv(elem.position, size, transpose, (GLfloat*)data); break;
            case GL_FLOAT_MAT2x3:
                glUniformMatrix2x3fv(elem.position, size, transpose, (GLfloat*)data); break;
            case GL_FLOAT_MAT2x4:
                glUniformMatrix2x4fv(elem.position, size, transpose, (GLfloat*)data); break;
            case GL_FLOAT_MAT3:
                glUniformMatrix3fv(elem.position, size, transpose, (GLfloat*)data); break;
            case GL_FLOAT_MAT3x2:
                glUniformMatrix3x2fv(elem.position, size, transpose, (GLfloat*)data); break;
            case GL_FLOAT_MAT3x4:
                glUniformMatrix3x4fv(elem.position, size, transpose, (GLfloat*)data); break;
            case GL_FLOAT_MAT4:
                glUniformMatrix4fv(elem.position, size, transpose, (GLfloat*)data); break;
            case GL_FLOAT_MAT4x2:
                glUniformMatrix4x2fv(elem.position, size, transpose, (GLfloat*)data); break;
            case GL_FLOAT_MAT4x3:
                glUniformMatrix4x3fv(elem.position, size, transpose, (GLfloat*)data); break;
            case GL_DOUBLE: 
                glUniform1dv(elem.position, size, (GLdouble*)data); break;
            case GL_DOUBLE_VEC2:
                glUniform2dv(elem.position, size, (GLdouble*)data); break;
            case GL_DOUBLE_VEC3:
                glUniform3dv(elem.position, size, (GLdouble*)data); break;
            case GL_DOUBLE_VEC4:
                glUniform4dv(elem.position, size, (GLdouble*)data); break;
            case GL_DOUBLE_MAT2:
                glUniformMatrix2dv(elem.position, size, transpose, (GLdouble*)data); break;
            case GL_DOUBLE_MAT2x3:
                glUniformMatrix2x3dv(elem.position, size, transpose, (GLdouble*)data); break;
            case GL_DOUBLE_MAT2x4:
                glUniformMatrix2x4dv(elem.position, size, transpose, (GLdouble*)data); break;
            case GL_DOUBLE_MAT3:
                glUniformMatrix3dv(elem.position, size, transpose, (GLdouble*)data); break;
            case GL_DOUBLE_MAT3x2:
                glUniformMatrix3x2dv(elem.position, size, transpose, (GLdouble*)data); break;
            case GL_DOUBLE_MAT3x4:
                glUniformMatrix3x4dv(elem.position, size, transpose, (GLdouble*)data); break;
            case GL_DOUBLE_MAT4:
                glUniformMatrix4dv(elem.position, size, transpose, (GLdouble*)data); break;
            case GL_DOUBLE_MAT4x2:
                glUniformMatrix4x2dv(elem.position, size, transpose, (GLdouble*)data); break;
            case GL_DOUBLE_MAT4x3:
                glUniformMatrix4x3dv(elem.position, size, transpose, (GLdouble*)data); break;
            case GL_INT:
                glUniform1iv(elem.position, size, (GLint*)data); break;
            case GL_INT_VEC2:
                glUniform2iv(elem.position, size, (GLint*)data); break;
            case GL_INT_VEC3:
                glUniform3iv(elem.position, size, (GLint*)data); break;
            case GL_INT_VEC4:
                glUniform4iv(elem.position, size, (GLint*)data); break;
            case GL_UNSIGNED_INT:
                glUniform1uiv(elem.position, size, (GLuint*)data); break;
            case GL_UNSIGNED_INT_VEC2:
                glUniform2uiv(elem.position, size, (GLuint*)data); break;
            case GL_UNSIGNED_INT_VEC3:
                glUniform3uiv(elem.position, size, (GLuint*)data); break;
            case GL_UNSIGNED_INT_VEC4:
                glUniform4uiv(elem.position, size, (GLuint*)data); break;
            default: break;
        }
    }

    void shader_t::enable(unordered_map<string, mesh_t::element_t>* attrs)
    {
        if (program.use_count() <= 0) return;
        glUseProgram(*program);
        
        for(auto it = attributes.begin(); it != attributes.end(); it++)
        {
            if(it->second.position < 0) continue;
            auto attr = attrs->find(it->first);
            if(attr != attrs->end() && attr->second.active)
            {
                glEnableVertexAttribArray(it->second.position);
                it->second.active = true;
                // glVertexAttribPointer correlates the shader variable at 'position' with the data in the buffer at 'offset'
                glVertexAttribPointer(it->second.position, it->second.size, it->second.type, attr->second.normalized, attr->second.interlStride, (GLvoid*)attr->second.offset);
            }
        }
    }

    void shader_t::disable() 
    {
        for(auto it = attributes.begin(); it != attributes.end(); it++)
        {
            if(it->second.active) glDisableVertexAttribArray(it->second.position);
        }
    }

    GLint shader_t::operator*() const
    {
        return *program;
    }

    // mesh_t::buffer_t
    void mesh_t::buffer_t::init()
    {
        if(isInit)
        {
            glDeleteBuffers(1, &indxBuff);
            glDeleteBuffers(1, &vertBuff);
            glDeleteVertexArrays(1, &vertArr);
        }
        glGenVertexArrays(1, &vertArr);
        glGenBuffers(1, &vertBuff);
        glGenBuffers(1, &indxBuff);
        isInit = true;
    }

    void mesh_t::buffer_t::bind()
    {
        if(!isInit) init();
        glBindVertexArray(vertArr);
        glBindBuffer(GL_ARRAY_BUFFER, vertBuff);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indxBuff);
    }

    mesh_t::buffer_t::~buffer_t()
    {
        if(!isInit) return;
        glDeleteBuffers(1, &indxBuff);
        glDeleteBuffers(1, &vertBuff);
        glDeleteVertexArrays(1, &vertArr);
        isInit = false;
    }

    // mesh_t::element_t
    mesh_t::element_t::element_t(){}

    mesh_t::element_t::element_t(size_t dsize, size_t dlength)
    {
        init(dsize, dlength);
    }

    void mesh_t::element_t::init(size_t dsize, size_t dlength)
    {
        size = dsize * dlength;
        stride = dsize;
        data.init(stride, size);
        isInit = true;
    }

    // mesh_t
    void mesh_t::update()
    {
        buffer.bind();

        buffer_size = 0;
        buffer_stride = 0;
        index_mode = index.size() > 0;
        vector<stride_vector*> elems;
        elems.reserve(elements.size());

        GLintptr off = 0;
        for(auto it = elements.begin(); it != elements.end(); it++)
        {
            if(!it->second.active) continue;
            buffer_size += it->second.size;
            buffer_stride += it->second.stride;

            it->second.offset = off; // calculate the buffer offset on the fly
            elems.push_back(&(it->second.data));
            off += it->second.stride;
        }

        for(auto it = elements.begin(); it != elements.end(); it++)
        {
            it->second.interlStride = buffer_stride;
        }

        glBufferData(GL_ARRAY_BUFFER, buffer_size, &(stride_vector::interleave(elems).data[0]), GL_STATIC_DRAW);

        if(index_mode)
        {
            index_count = index.size();
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(GLuint), &(index[0]), GL_STATIC_DRAW);
        }
    }

    void mesh_t::draw()
    {
        GLuint i = 0;
        for(auto it = textures.begin(); it != textures.end(); it++, i++)
        {
            // set the active texture unit
            glActiveTexture(GL_TEXTURE0 + i);
            // bind the image to the active texture unit
            it->second->bind();
            // bind the active texture unit to the sampler
            shader->setUniform(it->first, &i);
        }
        buffer.bind();
        shader->enable(&elements);
        if(index_mode) 
            glDrawElements(drawMode, index_count, GL_UNSIGNED_INT, NULL);
        else 
            glDrawArrays(drawMode, 0, buffer_size);
        shader->disable();
    }
}

#endif // LAK_GRAPHICS_HAS_IMPLEM
#endif // LAK_GRAPHICS_IMPLEM