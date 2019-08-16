#include "glak.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

void glakReadFile(const string& src, string& dst)
{
    FILE* ptr = fopen(src.c_str(), "r");
    if(ptr == NULL) 
    {
        cout << "Failed to open file " << src.c_str() << endl;
        dst = "";
        return;
    }
    fseek(ptr, 0L, SEEK_END);
    size_t size = ftell(ptr);
    fseek(ptr, 0L, SEEK_SET);
    dst.resize(size);
    fread((void*)&(dst[0]), 1, size, ptr);
    dst += '\0';
    fclose(ptr);
}

string glakReadFile(const string& src)
{
    string rtn = "";
    glakReadFile(src, rtn);
    return rtn;
}

void glakInitShader(GLuint program, const string& shaderstr, GLenum type)
{
    if(shaderstr == "")
    {
        cout << "Error reading shader " << shaderstr.c_str() << endl;
        throw exception();//"Error reading shader");
    }
    GLuint shader = glCreateShader(type);
    GLchar* shadercstr = (GLchar*)&(shaderstr[0]);
    glShaderSource(shader, 1, &shadercstr, NULL);
    glCompileShader(shader);
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled)
    {
        cout << "Error compiling shader " << shaderstr.c_str() << endl;
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

glakShader::glakShader(){}

glakShader::glakShader(const glakShader& other)
{
    program = other.program;
}

glakShader::~glakShader()
{
    if (program.unique())
    {
        glDeleteProgram(*program);
    }
}

void glakShader::init(const string& vshader, const string& fshader)
{
    program = make_shared<GLint>(glCreateProgram());
    glakInitShader(*program, vshader, GL_VERTEX_SHADER);
    glakInitShader(*program, fshader, GL_FRAGMENT_SHADER);
    glLinkProgram(*program);

    position =  glGetAttribLocation(*program, "vPosition");
    normal =    glGetAttribLocation(*program, "vNormal");
    color =     glGetAttribLocation(*program, "vColor");
    texCoord =  glGetAttribLocation(*program, "vTexCoord");
	worldViewPos = glGetUniformLocation(*program, "uWorldView");
    rotVec = glGetUniformLocation(*program, "rotation");
}

#define GLAK_ENABLE_ATTRIB(N, S, T, NO, PS, O) if(N >= 0) {glEnableVertexAttribArray(N); glVertexAttribPointer(N, S, T, NO, PS, (GLvoid*)O);}

void glakShader::enable()
{
    glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);
    if (program.use_count() <= 0) return;
    glUseProgram(*program);
    GLAK_ENABLE_ATTRIB(position,    glakVertexConst::posSize,   GL_FLOAT, GL_FALSE, sizeof(glakVertex), glakVertexConst::posOff)
    GLAK_ENABLE_ATTRIB(color,       glakVertexConst::colSize,   GL_FLOAT, GL_FALSE, sizeof(glakVertex), glakVertexConst::colOff)
    GLAK_ENABLE_ATTRIB(normal,      glakVertexConst::normSize,  GL_FLOAT, GL_FALSE, sizeof(glakVertex), glakVertexConst::normOff)
    GLAK_ENABLE_ATTRIB(texCoord,    glakVertexConst::coordSize, GL_FLOAT, GL_FALSE, sizeof(glakVertex), glakVertexConst::coordOff)
}

void glakShader::enable(const glm::mat4& worldviewt)
{
    enable();
	glUniformMatrix4fv(worldViewPos, 1, GL_FALSE, &(worldviewt)[0][0]);
	// glUniform3fv(rotVec, 1, &(worldviewt)[0][0]);
}

void glakShader::enable(glm::mat4&& worldviewt)
{
    enable(worldviewt);
}

void glakShader::disable()
{    
    if (position >= 0)   glDisableVertexAttribArray(position);
    if (normal >= 0)     glDisableVertexAttribArray(normal);
    if (color >= 0)      glDisableVertexAttribArray(color);
    if (texCoord >= 0)   glDisableVertexAttribArray(texCoord);
    glUseProgram(prevProgram);
}

GLint glakShader::operator*() const
{
    return *program;
}

void glakBuffer::init()
{
    glDeleteBuffers(1, &idxBuff);
    glDeleteBuffers(1, &vtxBuff);
    glDeleteVertexArrays(1, &vtxArr);
    glGenVertexArrays(1, &vtxArr);
    glGenBuffers(1, &vtxBuff);
    glGenBuffers(1, &idxBuff);
}

glakBuffer::~glakBuffer()
{
    glDeleteBuffers(1, &idxBuff);
    glDeleteBuffers(1, &vtxBuff);
    glDeleteVertexArrays(1, &vtxArr);
}

void glakMesh::updateBuffer()
{
    buffer.init();

    glBindVertexArray(buffer.vtxArr);

    vertex_count = vertex.size();
    glBindBuffer(GL_ARRAY_BUFFER, buffer.vtxBuff);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(glakVertex), &(vertex[0]), GL_STATIC_DRAW);
    
    index_count = index.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.idxBuff);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(glakIndex), &(index[0]), GL_STATIC_DRAW);
}

void glakMesh::draw()
{
    glBindVertexArray(buffer.vtxArr);
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_SHORT, NULL);
    glBindVertexArray(0);
}

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
    translation = glm::translate(glm::mat4(1.0f), position);
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
    scale = glm::scale(glm::mat4(1.0f), sca);
    return *this;
}

glakTransform& glakTransform::setScale(glm::vec3&& sca)
{
    return setScale(sca);
}

glakTransform& glakTransform::clear()
{
    translation = glm::mat4(1.0f);
    rotation = glm::mat4(1.0f);
    scale = glm::mat4(1.0f);
    return *this;
}

glm::mat4& glakTransform::make(bool clearmats)
{
    transform = rotation * scale * transform;
    if(clearmats) clear();
    return transform;
}

glm::mat4& glakTransform::append(bool clearmats)
{
    transform = translation * rotation * scale * transform;
    if(clearmats) clear();
    return transform;
}

glm::mat4& glakTransform::prepend(bool clearmats)
{
    transform = transform * translation * rotation * scale;
    if(clearmats) clear();
    return transform;
}

void glakObject::updateBuffer()
{
    for(auto it = mesh.begin(); it != mesh.end(); it++)
    {
        it->updateBuffer();
    }
}

void glakObject::draw(const glm::mat4& viewt)
{
    glakShader* prev = nullptr;
    for(auto it = mesh.begin(); it != mesh.end(); it++)
    {
        if (it->material < shader.size() && &(shader[it->material]) != nullptr)
        {
            if (prev != &(shader[it->material]))
            {
                if(prev != nullptr) prev->disable();
                prev = &(shader[it->material]);
                prev->enable(viewt * transform.transform); // worldviewt = viewt * worldspace-transform
            }
            it->draw();
        }
    }
    if(prev != nullptr) prev->disable();
}

void glakObject::draw(glm::mat4&& viewt)
{
    draw(viewt);
}

void glakObject::draw()
{
    draw(glm::mat4(1.0f));
}

void glakCredits()
{
    ImGui::PushID("Credits");
    if(ImGui::TreeNode("ImGui"))
    {
        ImGui::Text(R"(https://github.com/ocornut/imgui

The MIT License (MIT)

Copyright (c) 2014-2018 Omar Cornut

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
SOFTWARE.)");
        ImGui::TreePop();
    }
    if(ImGui::TreeNode("glm"))
    {
        ImGui::Text(R"(https://github.com/g-truc/glm

The Happy Bunny License (Modified MIT License)

Copyright (c) 2005 - 2016 G-Truc Creation

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do so, subject to the
following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions
of the Software.

Restrictions: By making use of the Software for military purposes, you choose to make a Bunny unhappy.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.)");
        ImGui::TreePop();
    }
    if(ImGui::TreeNode("tinyobjloader"))
    {
        ImGui::Text(R"(https://github.com/syoyo/tinyobjloader

The MIT License (MIT)

Copyright (c) 2012-2016 Syoyo Fujita and many contributors.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.)");
        ImGui::TreePop();
    }
    if(ImGui::TreeNode("gl3w"))
    {
        ImGui::Text("https://github.com/skaslev/gl3w");
        ImGui::TreePop();
    }
    if(ImGui::TreeNode("SDL2"))
    {
        ImGui::Text("https://www.libsdl.org/");
        ImGui::TreePop();
    }
    ImGui::PopID();
}