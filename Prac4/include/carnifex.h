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

#include <glak.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_interpolation.hpp>

#ifndef E_EPSILON
#define E_EPSILON 1.19e-07
#endif
#ifndef E_EPSILON_DOUBLE 
#define E_EPSILON_DOUBLE 2.22e-16
#endif
#ifndef E_PI
#define E_PI 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899
#endif

#ifndef CARNIFEX_H
#define CARNIFEX_H

namespace cfx
{
    template<typename T>
    void null_deleter(T*){}

    struct deltaTransform : public glakTransform
    {
        glm::vec3 rotAccel;
        glm::vec3 rotVel;
        glm::vec3 rot;
        glm::vec3 posAccel;
        glm::vec3 posVel;
        glm::vec3 pos;
        void update(float delta);
    };

    struct referenceFrame
    {
        deltaTransform transform;
        glm::mat4 getTransform();
        weak_ptr<referenceFrame> parent;
    };

    struct model : public referenceFrame
    {
        vector<shared_ptr<glakShader>> shader;
        vector<glakMesh> mesh;
        string modelUniformName = "model";
        void updateBuffer();
        void draw();
    };

    struct light : public referenceFrame
    {
        glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    };

    struct camera : public referenceFrame
    {
        glm::mat4 projection;  
    };
}

#endif