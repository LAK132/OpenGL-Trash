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

#include <carnifex.h>

namespace cfx
{
    // deltaTransform
    void deltaTransform::update(float delta)
    {
        rotVel += rotAccel * delta;
        rot += rotVel * delta;
        setRotation(rot.x, glakTransform::XUP);
        addRotation(rot.y, glakTransform::YUP);
        addRotation(rot.z, glakTransform::ZUP);
        posVel += posAccel * delta;
        pos += posVel * delta;
        setTranslation(pos);
        make();
    }

    // referenceFrame
    glm::mat4 referenceFrame::getTransform()
    {
        if(auto par = parent.lock())
            return transform.transform * par->getTransform();
        else
            return transform.transform;
    }

    // model
    void model::updateBuffer()
    {
        for(auto it = mesh.begin(); it != mesh.end(); it++)
        {
            it->updateBuffer();
        }
    }

    void model::draw()
    {
        shared_ptr<glakShader> prev = nullptr;
        for(auto it = mesh.begin(); it != mesh.end(); it++)
        {
            if (it->material < shader.size() && &(shader[it->material]) != nullptr)
            {
                if (prev != shader[it->material])
                {
                    if(prev != nullptr) prev->disable();
                    prev = shader[it->material];
                    prev->enable(&(it->elements));
                    prev->setUniform(modelUniformName, &(transform.transform)[0][0]);
                }
                it->draw();
            }
        }
        if(prev != nullptr) prev->disable();
    }
}