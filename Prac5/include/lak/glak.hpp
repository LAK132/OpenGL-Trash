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

// C++ 
#include <cmath>
#include <stddef.h>
#include <stdint.h>

#include <fstream>
using std::ifstream;

#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

#include <exception>
using std::exception;

#include <memory>
using std::shared_ptr;
using std::make_shared;
using std::weak_ptr;

#include <vector>
using std::vector;

#include <unordered_map>
using std::unordered_map;

#include <lak/stream_util.hpp>
#include <lak/stride_vector.hpp>
using lak::stride_vector;
#include <lak/void_ptr.hpp>
using lak::void_ptr;

// OpenGL
#include <GL/gl3w.h>
#ifndef GLAK_DISABLE_3D
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#endif

#ifndef GLAK_H
#define GLAK_H

extern bool debugOn;
extern bool throwOn;
#define DEBUG if(debugOn) std::cout << __FILE__ << "(" << std::dec << __LINE__ << ")" << std::endl
#define THROW(x) if(throwOn) throw x

#ifndef GLAK_DISABLE_3D

void glakReadFile(string src, string* dst);
string glakReadFile(string src);
void glakInitShader(GLuint program, const string& src, GLenum type);
void glakLinkProgram(GLuint program);

struct glakColor
{
    uint8_t r = 0, g = 0, b = 0, a = 255;
    glakColor();
    glakColor(uint8_t RGB);
    glakColor(uint16_t RGB, bool fifteenMode = false);
    glakColor(uint32_t RGBA);
    glakColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255);
    glakColor(const glakColor& other);
    glakColor(glakColor&& other);
    glakColor& operator=(const glakColor& other);
    glakColor& operator=(glakColor&& other);
    glakColor& operator=(uint8_t RGB);
    glakColor& operator=(uint16_t RGB);
    glakColor& operator=(uint32_t RGBA);
    bool operator==(const glakColor& other) const;
};

struct glakBitmap
{
    size_t w = 0, h = 0;
    vector<glakColor> pixels;
    glakBitmap();
    glakBitmap(size_t W, size_t H, uint8_t channels, uint8_t* pix);
    glakBitmap(size_t W, size_t H);
    glakBitmap(size_t W, const vector<glakColor>& pix);
    glakBitmap(size_t W, vector<glakColor>&& pix);
    glakBitmap(const glakBitmap& other);
    glakBitmap(glakBitmap&& other);
    glakBitmap& operator=(const glakBitmap& other);
    glakBitmap& operator=(glakBitmap&& other);
    glakColor* operator[](size_t y); // glakBitmap[y][x] -> glakColor
    vector<uint8_t> toRGB() const;
    vector<uint8_t> toRGBA() const;
    void resize(size_t width, size_t height);
};

glakBitmap glakReadPNM(const string& src);
glakBitmap glakReadPNM(string&& src);

struct glakTexParam
{
    GLenum pname;
    enum { INT, FLOAT, ARRAY, NONE } tag;
    union
    {
        GLint i;
        GLfloat f;
        shared_ptr<void> arr;
    };
    glakTexParam();
    glakTexParam(const glakTexParam& other);
    glakTexParam(glakTexParam&& other);
    glakTexParam(GLenum pn, GLint val);
    glakTexParam(GLenum pn, GLfloat val);
    glakTexParam(GLenum pn, shared_ptr<void> val);
    ~glakTexParam();
    glakTexParam& operator=(const glakTexParam& other);
    glakTexParam& operator=(glakTexParam&& other);
    void apply(GLenum target);
};

void bitmapToTexImage(GLenum textype, GLint level, GLint colformat, GLint border, const glakBitmap& bitmap);
void bitmapToTexImage(GLenum textype, GLint level, GLint colformat, GLint border, glakBitmap&& bitmap);

struct glakImage
{
private:
    bool generated = false;
public:
    size_t w = 0, h = 0;
    GLuint tex;
    GLenum texType = GL_TEXTURE_2D;
    vector<glakTexParam> params;
    glakImage();
    ~glakImage();
    glakImage(GLenum textype, GLint level, GLint colformat, GLint border, const glakBitmap& bitmap, const vector<glakTexParam>& prms);
    glakImage(GLenum textype, GLint level, GLint colformat, GLint border, const glakBitmap& bitmap, vector<glakTexParam>&& prms = vector<glakTexParam>(0));
    glakImage(GLenum textype, GLint level, GLint colformat, GLint border, glakBitmap&& bitmap, const vector<glakTexParam>& prms);
    glakImage(GLenum textype, GLint level, GLint colformat, GLint border, glakBitmap&& bitmap, vector<glakTexParam>&& prms = vector<glakTexParam>(0));
    void generate(GLenum textype, GLint level, GLint colformat, GLint border, const glakBitmap& bitmap, const vector<glakTexParam>& prms);
    void generate(GLenum textype, GLint level, GLint colformat, GLint border, const glakBitmap& bitmap, vector<glakTexParam>&& prms = vector<glakTexParam>(0));
    void generate(GLenum textype, GLint level, GLint colformat, GLint border, glakBitmap&& bitmap, const vector<glakTexParam>& prms);
    void generate(GLenum textype, GLint level, GLint colformat, GLint border, glakBitmap&& bitmap, vector<glakTexParam>&& prms = vector<glakTexParam>(0));
    void bind();
    void update();
};

struct glakShaderElement
{
    GLint position = -1; // program position
    GLenum type = GL_FLOAT;
    GLint size = 4; // element count
    string name = ""; // name in shader
    bool active = false;
};

struct glakMeshElement
{
    stride_vector data;
    bool active = false;
    size_t size = 0;
    size_t stride = 0;
    size_t interlStride = 0;
    GLintptr offset = 0; // buffer position
    bool normalized = false;
    glakMeshElement();
    glakMeshElement(size_t dsize, size_t dlength);
    void init(size_t dsize, size_t dlength);
    template <typename T>
    T* init(size_t dlength)
    {
        init(sizeof(T), dlength);
        return data.get<T>();
    }
private:
    bool isInit = false;
};

struct glakShader
{
    shared_ptr<GLint> program;
    unordered_map<string, glakShaderElement> attributes; // user defined type name ->  element
    unordered_map<string, glakShaderElement> uniforms; // user defined type name ->  element
    glakShader();
    glakShader(const glakShader& other);
    glakShader(glakShader&& other);
    glakShader(string vshader, string fshader);
    glakShader& operator=(const glakShader& other);
    glakShader& operator=(glakShader&& other);
    ~glakShader();
    void init(string vshader, string fshader);
    void initAttribs();
    void setUniform(string name, void* data, GLsizei size = 1, GLboolean transpose = GL_FALSE);
    void enable(unordered_map<string, glakMeshElement>* attrs);
    void disable();
    GLint operator*() const;
};

struct glakBuffer
{
    bool isInit = false;
    GLuint vertArr = 0, vertBuff = 0, indxBuff = 0;
    void init();
    void bind();
    ~glakBuffer();
};

struct glakMesh
{
private:
    bool index_mode = false;
    size_t index_count = 0;
    size_t buffer_size = 0;
    size_t buffer_stride = 0;
public:
    shared_ptr<glakShader> shader;
    glakBuffer buffer;
    unordered_map<string, glakMeshElement> elements;
    unordered_map<string, shared_ptr<glakImage>> images; // user defined type name ->  element
    vector<GLuint> index;
    void update();
    void draw();
};

struct glakTransform
{
    const static glm::vec4 XUP;
    const static glm::vec4 YUP;
    const static glm::vec4 ZUP;
    const static glm::vec4 WUP;
    const static glm::mat4 IDENTITY;
    glm::mat4 translation = IDENTITY;
    glm::mat4 rotation = IDENTITY;
    glm::mat4 scale = IDENTITY;
    glm::mat4 transform = IDENTITY;
    
    glakTransform& addTranslation(const glm::vec3& displace);   
    glakTransform& addTranslation(glm::vec3&& displace);   

    glakTransform& setTranslation(const glm::vec3& position);
    glakTransform& setTranslation(glm::vec3&& position);

    glakTransform& addScale(const glm::vec3& sca);
    glakTransform& addScale(glm::vec3&& sca);

    glakTransform& setScale(const glm::vec3& sca);
    glakTransform& setScale(glm::vec3&& sca);

    glakTransform& clear();
    glakTransform& make();

    template<typename T>
    glakTransform& addRotation(T angle, const glm::vec3& axis)
    {
        rotation = glm::rotate(rotation, angle, axis);
        return *this;
    }
    template<typename T>
    glakTransform& addRotation(T angle, glm::vec3&& axis)
    {
        return addRotation(angle, axis);
    }
    template<typename T>
    glakTransform& setRotation(T angle, const glm::vec3& axis)
    {
        rotation = glm::rotate(glm::mat4(1.0f), angle, axis);
        return *this;
    }
    template<typename T>
    glakTransform& setRotation(T angle, glm::vec3&& axis)
    {
        return setRotation(angle, axis);
    }
};

#endif // GLAK_DISABLE_3D

/*
#define GLAK_HANDLE_MAIN
#define GLAK_MULTITHREAD
//*/

#ifdef GLAK_HANDLE_MAIN

#define SDL_MAIN_HANDLED
#include <SDL.h>

#ifdef GLAK_MULTITHREAD

#include <thread>
using std::thread;
#include <atomic>
using std::atomic;
using std::atomic_bool;
#include <mutex>
using std::mutex;

struct glakLoopData
{
    atomic_bool run;
    double targetDrawTime = 1.0/60.0;
    double drawDelta = 0.0;
    double updateDelta = 0.0;
    mutex draw_mtx;
    mutex update_mtx;
    SDL_Window* window;
    SDL_GLContext glContext;
    void* userData;
};

#else

struct glakLoopData
{
    bool run;
    double targetDrawTime = 1.0/60.0;
    double drawDelta = 0.0;
    double updateDelta = 0.0;
    SDL_Window* window;
    SDL_GLContext glContext;
    void* userData;
};

#endif // GLAK_MULTITHREAD

int main();
void update_loop(glakLoopData* ld);
#ifdef GLAK_MULTITHREAD
void draw_loop(glakLoopData* ld);
#endif

void update(glakLoopData* ld);
void draw(glakLoopData* ld);

void init(glakLoopData* ld);
void destroy(glakLoopData* ld);

#endif // GLAK_HANDLE_MAIN

#endif // GLAK_H

/*
#define GLAK_IMPLEMENTATION
//*/

#ifdef GLAK_IMPLEMENTATION

bool debugOn = true;
bool throwOn = true;

#ifndef GLAK_DISABLE_3D

void glakReadFile(string src, string* dst)
{
    FILE* ptr = fopen(src.c_str(), "r");
    if(ptr == NULL) 
    {
        cout << "Failed to open file " << src.c_str() << endl;
        *dst = "";
        return;
    }
    fseek(ptr, 0L, SEEK_END);
    size_t size = ftell(ptr);
    fseek(ptr, 0L, SEEK_SET);
    dst->resize(size);
    fread((void*)&((*dst)[0]), 1, size, ptr);
    *dst += '\0';
    fclose(ptr);
}

string glakReadFile(string src)
{
    string rtn = "";
    glakReadFile(src, &rtn);
    return rtn;
}

glakBitmap glakReadPNM(const string& src)
{
    glakBitmap rtn;
    ifstream strm(src.c_str(), std::ios::binary|std::ifstream::in);
    if(!strm.is_open()) return rtn;
    if(strm.get() != 'P') return rtn;
    switch(strm.get())
    {
        // case '1': {
        //      
        // } break;
        // case '2': {
        //      
        // } break;
        // case '3': {
        // 
        // } break;
        // case '4': {
        //      
        // } break;
        // case '5': {
        //      
        // } break;
        case '6': {
            char c;
            size_t width = 0, height = 0;
            uint16_t size = 0;
            skipAll(strm, " \t\r\n"); // whitespace
            while((c = strm.get()) != ' ' && c != '\t' && c != '\r' && c != '\n')
                width = (width * 10) + (c - '0');
            skipAll(strm, " \t\r\n"); // whitespace
            while((c = strm.get()) != ' ' && c != '\t' && c != '\r' && c != '\n')
                height = (height * 10) + (c - '0');
            skipAll(strm, " \t\r\n"); // whitespace
            while((c = strm.get()) != ' ' && c != '\t' && c != '\r' && c != '\n')
                size = (size * 10) + (c - '0');
            // NOTE: previous while loop actually already handles the single white space after size
            rtn.resize(width, height);
            for(size_t y = 0; y < height; y++)
            {
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

glakBitmap glakReadPNM(string&& src)
{
    glakBitmap& rtn = glakReadPNM(src);
    return rtn;
}

void glakInitShader(GLuint program, const string& src, GLenum type)
{
    if(src == "")
    {
        cout << "Error reading shader " << src.c_str() << endl;
        throw exception();//"Error reading shader");
    }
    GLuint shader = glCreateShader(type);
    GLchar* shadercstr = (GLchar*)&(src[0]);
    glShaderSource(shader, 1, &shadercstr, NULL);
    glCompileShader(shader);
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled)
    {
        cout << "Error compiling shader " << src.c_str() << endl;
        GLint msgSize = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &msgSize);
        string msg; msg.resize(msgSize);
        glGetShaderInfoLog(shader, msgSize, NULL, &(msg[0]));
        cout << msg.c_str() << endl;
        throw exception();//msg.c_str());
    }
    glAttachShader(program, shader);
}

void glakLinkProgram(GLuint program)
{
    glLinkProgram(program);
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if(!linked)
    {
        cout << "Shader program failed to link" << endl;
        GLint msgSize;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &msgSize);
        string msg; msg.resize(msgSize);
        glGetProgramInfoLog(program, msgSize, NULL, &(msg[0]));
        cout << msg.c_str() << endl;
        throw exception();//msg.c_str());
    }
}

// glakShader
glakShader::glakShader(){}

glakShader::glakShader(const glakShader& other)
{
    program = other.program;
    attributes = other.attributes;
    uniforms = other.uniforms;
}

glakShader::glakShader(glakShader&& other)
{
    program = other.program;
    attributes = other.attributes;
    uniforms = other.uniforms;
}

glakShader::glakShader(string vshader, string fshader)
{
    init(vshader, fshader);
}

glakShader& glakShader::operator=(const glakShader& other)
{
    program = other.program;
    attributes = other.attributes;
    uniforms = other.uniforms;
    return *this;
}

glakShader& glakShader::operator=(glakShader&& other)
{
    program = other.program;
    attributes = other.attributes;
    uniforms = other.uniforms;
    return *this;
}

glakShader::~glakShader()
{
    if (program.unique())
    {
        glDeleteProgram(*program);
    }
}

void glakShader::init(string vshader, string fshader)
{
    program = make_shared<GLint>(glCreateProgram());
    glakInitShader(*program, vshader, GL_VERTEX_SHADER);
    glakInitShader(*program, fshader, GL_FRAGMENT_SHADER);
    glakLinkProgram(*program);
    initAttribs();
}

void glakShader::initAttribs()
{
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
        glGetActiveAttrib(*program, (GLuint)i, nameLen, &length, &size, &type, &name[0]);
        glakShaderElement elem;
        elem.position = glGetAttribLocation(*program, &name[0]);
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_TYPE, (GLint*)&elem.type);
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, &elem.size);
        elem.name = &name[0];
        attributes[elem.name] = elem;
    }

    glGetProgramiv(*program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &nameLen);
    name.resize(nameLen);
    
    uniforms.clear();
    glGetProgramiv(*program, GL_ACTIVE_UNIFORMS, &count);
    for (GLint i = 0; i < count; i++)
    {
        glGetActiveUniform(*program, (GLuint)i, nameLen, &length, &size, &type, &name[0]);
        glakShaderElement elem;
        elem.position = glGetUniformLocation(*program, &name[0]);
        elem.type = type;
        elem.size = size;
        elem.name = &name[0];
        uniforms[elem.name] = elem;
    }
}

void glakShader::setUniform(string name, void* data, GLsizei size, GLboolean transpose)
{
    if (program.use_count() <= 0) return;
    auto it = uniforms.find(name);
    if(it == uniforms.end()) return;
    glUseProgram(*program);
    glakShaderElement& elem = it->second;
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

void glakShader::enable(unordered_map<string, glakMeshElement>* attrs)
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

void glakShader::disable()
{
    for(auto it = attributes.begin(); it != attributes.end(); it++)
    {
        if(it->second.active) glDisableVertexAttribArray(it->second.position);
    }
}

GLint glakShader::operator*() const
{
    return *program;
}

// glakMeshElement
glakMeshElement::glakMeshElement(){}

glakMeshElement::glakMeshElement(size_t dsize, size_t dlength)
{
    init(dsize, dlength);
}

void glakMeshElement::init(size_t dsize, size_t dlength)
{
    size = dsize * dlength;
    stride = dsize;
    data.init(stride, size);
    isInit = true;
}

// glakBuffer
void glakBuffer::init()
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

void glakBuffer::bind()
{
    if(!isInit) init();
    glBindVertexArray(vertArr);
    glBindBuffer(GL_ARRAY_BUFFER, vertBuff);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indxBuff);
}

glakBuffer::~glakBuffer()
{
    if(!isInit) return;
    glDeleteBuffers(1, &indxBuff);
    glDeleteBuffers(1, &vertBuff);
    glDeleteVertexArrays(1, &vertArr);
    isInit = false;
}

// glakMesh
void glakMesh::update()
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

void glakMesh::draw()
{
    GLuint i = 0;
    for(auto it = images.begin(); it != images.end(); it++, i++)
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
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, NULL);
    else 
        glDrawArrays(GL_TRIANGLES, 0, buffer_size);
    shader->disable();
}

// glakColor
glakColor::glakColor(){}

glakColor::glakColor(uint8_t RGB)
{
    (*this) = RGB;
}

glakColor::glakColor(uint16_t RGB, bool fifteenMode)
{
    if(fifteenMode)
    {
        r = ((RGB & 0x7C00) >> 7); // * (0xFF / 0x1F);  // 01111100 00000000
        g = ((RGB & 0x03E0) >> 2); // * (0xFF / 0x1F);  // 00000011 11100000
        b = ((RGB & 0x001F) << 3); // * (0xFF / 0x1F);  // 00000000 00011111
    }
    else
    {
        (*this) = RGB;
    }
}

glakColor::glakColor(uint32_t RGBA)
{
    (*this) = RGBA;
}

glakColor::glakColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
{
    r = R;
    b = B;
    g = G;
    a = A;
}

glakColor::glakColor(const glakColor& other)
{
    r = other.r;
    b = other.b;
    g = other.g;
    a = other.a;
}

glakColor::glakColor(glakColor&& other)
{
    r = other.r;
    b = other.b;
    g = other.g;
    a = other.a;
}

glakColor& glakColor::operator=(const glakColor& other)
{
    r = other.r;
    b = other.b;
    g = other.g;
    a = other.a;
    return *this;
}

glakColor& glakColor::operator=(glakColor&& other)
{
    r = other.r;
    b = other.b;
    g = other.g;
    a = other.a;
    return *this;
}

glakColor& glakColor::operator=(uint8_t RGB)
{
    r = (((RGB >> 4) & 0x7) * (0xFF / 0x3));    // 11100000
    g = (((RGB >> 2) & 0x7) * (0xFF / 0x3));    // 00011100
    b = (( RGB       & 0x2) * (0xFF / 0x2));    // 00000011
    return *this;
}

glakColor& glakColor::operator=(uint16_t RGB)
{
    r = ((RGB & 0xF800) >> 8); // * (0xFF / 0x1F);  // 11111000 00000000
    g = ((RGB & 0x07E0) >> 3); // * (0xFF / 0x3F);  // 00000111 11100000
    b = ((RGB & 0x001F) << 3); // * (0xFF / 0x1F);  // 00000000 00011111
    return *this;
}

glakColor& glakColor::operator=(uint32_t RGBA)
{
    r = ((RGBA & 0xFF000000) >> 24);    // 11111111 00000000 00000000 00000000
    g = ((RGBA & 0x00FF0000) >> 16);    // 00000000 11111111 00000000 00000000
    b = ((RGBA & 0x0000FF00) >> 8);     // 00000000 00000000 11111111 00000000
    a = ((RGBA & 0x000000FF));          // 00000000 00000000 00000000 11111111
    return *this;
}

// glakBitmap
glakBitmap::glakBitmap()
{
    pixels.clear();
}

glakBitmap::glakBitmap(size_t W, size_t H, uint8_t channels, uint8_t* pix)
{
    w = W;
    h = H;
    pixels.resize(w * h);
    for(size_t i = 0, j = 0; i < pixels.size(); i++)
    {
        switch(channels)
        {
            case 1:
                pixels[i].r = pix[i];
                pixels[i].g = pix[i];
                pixels[i].b = pix[i];
                pixels[i].a = 255;
                break;
            case 2:
                pixels[i].r = pix[j];
                pixels[i].g = pix[j];
                pixels[i].b = pix[j++];
                pixels[i].a = pix[j++];
                break;
            case 3:
                pixels[i].r = pix[j++];
                pixels[i].g = pix[j++];
                pixels[i].b = pix[j++];
                pixels[i].a = 255;
                break;
            case 4:
                pixels[i].r = pix[j++];
                pixels[i].g = pix[j++];
                pixels[i].b = pix[j++];
                pixels[i].a = pix[j++];
                break;
            default: break;
        }
    }
}

glakBitmap::glakBitmap(size_t W, size_t H)
{
    w = W;
    h = H;
    pixels.resize(w * h);
}

glakBitmap::glakBitmap(size_t W, const vector<glakColor>& pix)
{
    w = W;
    h = pix.size() / w;
    pixels.resize(w * h);
    for(size_t i = 0; i < pixels.size(); i++)
    {
        pixels[i] = pix[i];
    }
}

glakBitmap::glakBitmap(size_t W, vector<glakColor>&& pix)
{
    w = W;
    h = pix.size() / w;
    pixels.resize(w * h);
    for(size_t i = 0; i < pixels.size(); i++)
    {
        pixels[i] = pix[i];
    }
}

glakBitmap::glakBitmap(const glakBitmap& other)
{
    w = other.w;
    h = other.h;
    pixels = other.pixels;
}

glakBitmap::glakBitmap(glakBitmap&& other)
{
    w = other.w;
    h = other.h;
    pixels = other.pixels;
}

glakBitmap& glakBitmap::operator=(const glakBitmap& other)
{
    w = other.w;
    h = other.h;
    pixels = other.pixels;
    return *this;
}

glakBitmap& glakBitmap::operator=(glakBitmap&& other)
{
    w = other.w;
    h = other.h;
    pixels = other.pixels;
    return *this;
}

glakColor* glakBitmap::operator[](size_t y)
{
    return &(pixels[y * w]);
}

vector<uint8_t> glakBitmap::toRGB() const
{
    const size_t size = pixels.size() * 3;
    vector<uint8_t> rtn(size);
    for(size_t i = 0, j = 0; i < size; j++)
    {
        rtn[i++] = pixels[j].r;
        rtn[i++] = pixels[j].g;
        rtn[i++] = pixels[j].b;
    }
    return rtn;
}

vector<uint8_t> glakBitmap::toRGBA() const
{
    const size_t size = pixels.size() * 4;
    vector<uint8_t> rtn(size);
    for(size_t i = 0, j = 0; i < size; j++)
    {
        rtn[i++] = pixels[j].r;
        rtn[i++] = pixels[j].g;
        rtn[i++] = pixels[j].b;
        rtn[i++] = pixels[j].a;
    }
    return rtn;
}

void glakBitmap::resize(size_t width, size_t height)
{
    w = width;
    h = height;
    pixels.resize(w * h);
}

// glakTexParam
glakTexParam::glakTexParam()
{
    pname = 0xBAADF00D;
    tag = NONE;
}

glakTexParam::glakTexParam(const glakTexParam& other)
{
    pname = other.pname;
    tag = other.tag;
    switch(tag)
    {
        case INT: i = other.i; break;
        case FLOAT: f = other.f; break;
        case ARRAY: {
            new (&arr) shared_ptr<void>;
            arr = other.arr;
        } break;
        default: break;
    }
}

glakTexParam::glakTexParam(glakTexParam&& other)
{
    pname = other.pname;
    tag = other.tag;
    switch(tag)
    {
        case INT: i = other.i; break;
        case FLOAT: f = other.f; break;
        case ARRAY: {
            new (&arr) shared_ptr<void>;
            arr = other.arr;
        } break;
        default: break;
    }
}

glakTexParam::glakTexParam(GLenum pn, GLint val)
{
    pname = pn;
    tag = INT;
    i = val;
}

glakTexParam::glakTexParam(GLenum pn, GLfloat val)
{
    pname = pn;
    tag = FLOAT;
    f = val;
}

glakTexParam::glakTexParam(GLenum pn, shared_ptr<void> val)
{
    pname = pn;
    tag = ARRAY;
    new (&arr) shared_ptr<void>;
    arr = val;
}

glakTexParam::~glakTexParam()
{
    if(tag == ARRAY)
        arr.~shared_ptr<void>();
}

glakTexParam& glakTexParam::operator=(const glakTexParam& other)
{
    pname = other.pname;
    if(tag == other.tag)
    {
        switch(other.tag)
        {
            case INT: i = other.i; break;
            case FLOAT: f = other.f; break;
            case ARRAY: arr = other.arr; break;
            default: break;
        }
    }
    else if(tag == ARRAY)
    {
        arr.~shared_ptr<void>();
        switch(other.tag)
        {
            case INT: i = other.i; break;
            case FLOAT: f = other.f; break;
            default: break;
        }
    }
    else
    {
        switch(other.tag)
        {
            case INT: i = other.i; break;
            case FLOAT: f = other.f; break;
            case ARRAY: {
                new (&arr) shared_ptr<void>;
                arr = other.arr;
            } break;
            default: break;
        }
    }
    tag = other.tag;
    return *this;
}

glakTexParam& glakTexParam::operator=(glakTexParam&& other)
{
    return *this = other;
}

void glakTexParam::apply(GLenum target)
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
        default: return;
    }
}

// glakImage
glakImage::glakImage(){}

glakImage::~glakImage()
{
    if(!generated) return;
    glDeleteTextures(1, &tex);
}

void glakImage::bind()
{
    if(!generated) return;
    glBindTexture(texType, tex);
}

void glakImage::update()
{
    if(!generated) return;
    bind();
    for(auto it = params.begin(); it != params.end(); it++)
    {
        it->apply(texType);
    }
}

glakImage::glakImage(GLenum textype, GLint level, GLint colformat, GLint border, const glakBitmap& bitmap, const vector<glakTexParam>& prms)
{
    generate(textype, level, colformat, border, bitmap, prms);
}

glakImage::glakImage(GLenum textype, GLint level, GLint colformat, GLint border, glakBitmap&& bitmap, const vector<glakTexParam>& prms)
{
    generate(textype, level, colformat, border, bitmap, prms);
}

glakImage::glakImage(GLenum textype, GLint level, GLint colformat, GLint border, const glakBitmap& bitmap, vector<glakTexParam>&& prms)
{
    generate(textype, level, colformat, border, bitmap, prms);
}

glakImage::glakImage(GLenum textype, GLint level, GLint colformat, GLint border, glakBitmap&& bitmap, vector<glakTexParam>&& prms)
{
    generate(textype, level, colformat, border, bitmap, prms);
}

void bitmapToTexImage(GLenum textype, GLint level, GLint colformat, GLint border, const glakBitmap& bitmap)
{
    glTexImage2D(textype, level, colformat, bitmap.w, bitmap.h, border, GL_RGBA, GL_UNSIGNED_BYTE, &(bitmap.toRGBA()[0]));
}

void bitmapToTexImage(GLenum textype, GLint level, GLint colformat, GLint border,  glakBitmap&& bitmap)
{
    bitmapToTexImage(textype, level, colformat, border, bitmap);
}

void glakImage::generate(GLenum textype, GLint level, GLint colformat, GLint border, const glakBitmap& bitmap, const vector<glakTexParam>& prms)
{
    texType = textype;
    w = bitmap.w;
    h = bitmap.h;
    params = prms;
    if(!generated)
    {
        glGenTextures(1, &tex);
        generated = true;
    }
    update();
    if(w > 0 && h > 0) bitmapToTexImage(texType, level, colformat, border, bitmap);
}

void glakImage::generate(GLenum textype, GLint level, GLint colformat, GLint border, glakBitmap&& bitmap, const vector<glakTexParam>& prms)
{
    generate(textype, level, colformat, border, bitmap, prms);
}

void glakImage::generate(GLenum textype, GLint level, GLint colformat, GLint border, const glakBitmap& bitmap, vector<glakTexParam>&& prms)
{
    generate(textype, level, colformat, border, bitmap, prms);
}

void glakImage::generate(GLenum textype, GLint level, GLint colformat, GLint border, glakBitmap&& bitmap, vector<glakTexParam>&& prms)
{
    generate(textype, level, colformat, border, bitmap, prms);
}

// glakTransform
const glm::vec4 glakTransform::XUP = {1.0f, 0.0f, 0.0f, 0.0f};
const glm::vec4 glakTransform::YUP = {0.0f, 1.0f, 0.0f, 0.0f};
const glm::vec4 glakTransform::ZUP = {0.0f, 0.0f, 1.0f, 0.0f};
const glm::vec4 glakTransform::WUP = {0.0f, 0.0f, 0.0f, 1.0f};
const glm::mat4 glakTransform::IDENTITY = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 
    0.0f, 0.0f, 1.0f, 0.0f, 
    0.0f, 0.0f, 0.0f, 1.0f
};

glakTransform& glakTransform::addTranslation(const glm::vec3& displace)
{
    translation = glm::translate(translation, displace);
    return *this;
}

glakTransform& glakTransform::addTranslation(glm::vec3&& displace)
{
    return addTranslation(displace);
}

glakTransform& glakTransform::setTranslation(const glm::vec3& position)
{
    translation = glm::translate(IDENTITY, position);
    return *this;
}

glakTransform& glakTransform::setTranslation(glm::vec3&& position)
{
    return setTranslation(position);
}

glakTransform& glakTransform::addScale(const glm::vec3& sca)
{
    scale = glm::scale(scale, sca + glm::vec3(1.0f, 1.0f, 1.0f));
    return *this;
}

glakTransform& glakTransform::addScale(glm::vec3&& sca)
{
    return addScale(sca);
}

glakTransform& glakTransform::setScale(const glm::vec3& sca)
{
    scale = glm::scale(IDENTITY, sca);
    return *this;
}

glakTransform& glakTransform::setScale(glm::vec3&& sca)
{
    return setScale(sca);
}

glakTransform& glakTransform::clear()
{
    translation = IDENTITY;
    rotation = IDENTITY;
    scale = IDENTITY;
    return *this;
}

glakTransform& glakTransform::make()
{
    // transform = translation * rotation * scale;
    transform = scale * rotation * translation;
    return *this;
}

#endif // GLAK_DISABLE_3D

#ifdef GLAK_HANDLE_MAIN

int main()
{
    glakLoopData ld;
    init(&ld);

    ld.run = true;

    update(&ld); // Make sure to update once before drawing
    
    #ifdef GLAK_MULTITHREAD
    thread draw(draw_loop, &ld);
    #endif

    update_loop(&ld);

    #ifdef GLAK_MULTITHREAD
    draw.join();
    #endif

    SDL_GL_MakeCurrent(ld.window, ld.glContext);
    destroy(&ld);

    return 0;
}

#ifdef GLAK_MULTITHREAD

void update_loop(glakLoopData* ld)
{
    uint64_t LAST = 0;
    uint64_t NOW = SDL_GetPerformanceCounter();

    while(ld->run)
    {
        ld->update_mtx.lock();
        ld->draw_mtx.lock();
        ld->update_mtx.unlock();
        
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        ld->updateDelta = (NOW - LAST) / (double) SDL_GetPerformanceFrequency();

        SDL_GL_MakeCurrent(ld->window, ld->glContext);
        update(ld);
        SDL_GL_MakeCurrent(ld->window, 0);

        ld->draw_mtx.unlock();
    }
}

void draw_loop(glakLoopData* ld)
{
    uint64_t LAST = SDL_GetPerformanceCounter();
    uint64_t NOW = SDL_GetPerformanceCounter();
    bool vsyncDisabled = SDL_GL_GetSwapInterval() == 0;

    while(ld->run)
    {
        double deltaError = ld->drawDelta - ld->targetDrawTime;

        deltaError = deltaError > 0.0 ? deltaError - (ld->targetDrawTime * (double)(uint64_t)(deltaError / ld->targetDrawTime)) : 0.0;

        ld->drawDelta = deltaError;

        do // busy wait the call thread if vsyncDisabled
        {
            LAST = NOW;
            NOW = SDL_GetPerformanceCounter();
            ld->drawDelta += (NOW - LAST) / (double) SDL_GetPerformanceFrequency();
            std::this_thread::yield();
        } while (vsyncDisabled && ld->drawDelta < ld->targetDrawTime);

        ld->update_mtx.lock();
        ld->draw_mtx.lock();
        ld->update_mtx.unlock();

        SDL_GL_MakeCurrent(ld->window, ld->glContext);

        draw(ld);

        SDL_GL_SwapWindow(ld->window);
        SDL_GL_MakeCurrent(ld->window, 0);  

        ld->draw_mtx.unlock();

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        ld->drawDelta += (NOW - LAST) / (double) SDL_GetPerformanceFrequency();
    }
}

#else

void update_loop(glakLoopData* ld)
{
    uint64_t LAST = 0;
    uint64_t NOW = SDL_GetPerformanceCounter();

    // SDL_GL_MakeCurrent(ld->window, ld->glContext);
    while(ld->run)
    {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        ld->drawDelta = ((NOW - LAST) * 1000) / (double) SDL_GetPerformanceFrequency();

        update(ld);

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        ld->updateDelta = ((NOW - LAST) * 1000) / (double) SDL_GetPerformanceFrequency();

        draw(ld);

        SDL_GL_SwapWindow(ld->window);
    }
    // SDL_GL_MakeCurrent(0, 0);
}

#endif // GLAK_MULTITHREAD

#endif // GLAK_HANDLE_MAIN

#endif // GLAK_IMPLEMENTATION