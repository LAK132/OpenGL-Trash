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

#ifndef LAK_SPACE_VEC3
#include <glm/vec3.hpp>
#define LAK_SPACE_VEC3 glm::vec3
#endif

#ifndef LAK_SPACE_VEC4
#include <glm/vec4.hpp>
#define LAK_SPACE_VEC4 glm::vec4
#endif

#ifndef LAK_SPACE_MAT4
#include <glm/mat4x4.hpp>
#define LAK_SPACE_MAT4 glm::mat4
#endif

#if !defined(LAK_SPACE_TRANSLATE) || !defined(LAK_SPACE_ROTATE) || !defined(LAK_SPACE_SCALE) || !defined(LAK_SPACE_INVERSE)
#include <glm/gtc/matrix_transform.hpp>
#endif
#ifndef LAK_SPACE_TRANSLATE
#define LAK_SPACE_TRANSLATE glm::translate
#endif
#ifndef LAK_SPACE_ROTATE
#define LAK_SPACE_ROTATE glm::rotate
#endif
#ifndef LAK_SPACE_SCALE
#define LAK_SPACE_SCALE glm::scale
#endif
#ifndef LAK_SPACE_INVERSE
#define LAK_SPACE_INVERSE glm::inverse
#endif
#ifndef LAK_SPACE_TRANSPOSE
#define LAK_SPACE_TRANSPOSE glm::transpose
#endif 
#ifndef LAK_SPACE_NOMALIZE
#define LAK_SPACE_NOMALIZE glm::normalize
#endif

// #define LAK_SPACE_IMGUI
#ifdef LAK_SPACE_IMGUI
#include <imgui.h>
#endif

#ifndef M_2_PI
#define M_2_PI  6.28318530717958647692528676655900577
#endif
#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288
#endif
#ifndef M_PI_2
#define M_PI_2  1.57079632679489661923132169163975144
#endif

#include <vector>
#include <memory>
#include <cmath>

#ifndef LAK_SPACE_H
#define LAK_SPACE_H

namespace lak
{
    using std::vector;
    using std::shared_ptr;
    using std::weak_ptr;
    using std::make_shared;
    struct transform_t
    {
        const static LAK_SPACE_VEC4 XUP;
        const static LAK_SPACE_VEC4 YUP;
        const static LAK_SPACE_VEC4 ZUP;
        const static LAK_SPACE_VEC4 WUP;
        const static LAK_SPACE_MAT4 IDEN;
        LAK_SPACE_MAT4 translation = IDEN;
        LAK_SPACE_MAT4 rotation = IDEN;
        LAK_SPACE_MAT4 scale = IDEN;
        LAK_SPACE_MAT4 combined = IDEN;

        transform_t& addTranslation(const LAK_SPACE_VEC3& displace);
        transform_t& addTranslation(LAK_SPACE_VEC3&& displace);

        transform_t& setTranslation(const LAK_SPACE_VEC3& position);
        transform_t& setTranslation(LAK_SPACE_VEC3&& position);

        transform_t& addScale(const LAK_SPACE_VEC3& sca);
        transform_t& addScale(LAK_SPACE_VEC3&& sca);

        transform_t& setScale(const LAK_SPACE_VEC3& sca);
        transform_t& setScale(LAK_SPACE_VEC3&& sca);

        transform_t& addRotation(const LAK_SPACE_VEC3& angles);
        transform_t& addRotation(LAK_SPACE_VEC3&& angles);

        transform_t& setRotation(const LAK_SPACE_VEC3& angles);
        transform_t& setRotation(LAK_SPACE_VEC3&& angles);

        transform_t& clear();
        transform_t& make(bool inverted = false);

        template <typename T>
        transform_t& addRotation(T angle, const LAK_SPACE_VEC3& axis)
        {
            rotation = LAK_SPACE_ROTATE(rotation, angle, axis);
            return *this;
        }
        template <typename T>
        transform_t& addRotation(T angle, LAK_SPACE_VEC3&& axis)
        {
            return addRotation(angle, axis);
        }
        template <typename T>
        transform_t& setRotation(T angle, const LAK_SPACE_VEC3& axis)
        {
            rotation = LAK_SPACE_ROTATE(IDEN, angle, axis);
            return *this;
        }
        template <typename T>
        transform_t& setRotation(T angle, LAK_SPACE_VEC3&& axis)
        {
            return setRotation(angle, axis);
        }
    };

    struct deltaTransform_t : public transform_t
    {
        LAK_SPACE_VEC3 rotAccel = LAK_SPACE_VEC3(0.0f);
        LAK_SPACE_VEC3 rotVel = LAK_SPACE_VEC3(0.0f);
        LAK_SPACE_VEC3 rot = LAK_SPACE_VEC3(0.0f);
        LAK_SPACE_VEC3 posAccel = LAK_SPACE_VEC3(0.0f);
        LAK_SPACE_VEC3 posVel = LAK_SPACE_VEC3(0.0f);
        LAK_SPACE_VEC3 pos = LAK_SPACE_VEC3(0.0f);
        void update(float delta);
    };

    struct referenceFrame_t
    {
        vector<shared_ptr<referenceFrame_t>> children;
        referenceFrame_t* parent = nullptr;
        deltaTransform_t transform;
        ~referenceFrame_t();
        void addChild(shared_ptr<referenceFrame_t> other);
        void removeChild(shared_ptr<referenceFrame_t> other);
        void removeChild(referenceFrame_t* other);
        void removeChild(size_t index);
        LAK_SPACE_MAT4 getTransform(bool inverted = false, bool reversed = false);
    };

    #ifdef LAK_SPACE_IMGUI
    template<typename T>
    void TransformView(T transform);
    template<>
    void TransformView<transform_t*>(transform_t* transform);
    template<>
    void TransformView<shared_ptr<transform_t>>(shared_ptr<transform_t> transform);
    template<>
    void TransformView<deltaTransform_t*>(deltaTransform_t* transform);
    template<>
    void TransformView<shared_ptr<deltaTransform_t>>(shared_ptr<deltaTransform_t> transform);
    template<>
    void TransformView<referenceFrame_t*>(referenceFrame_t* transform);
    template<>
    void TransformView<shared_ptr<referenceFrame_t>>(shared_ptr<referenceFrame_t> transform);
    #endif
}

#endif // LAK_SPACE_H

// #define LAK_SPACE_IMPLEM
#ifdef LAK_SPACE_IMPLEM
#ifndef LAK_SPACE_HAS_IMPLEM
#define LAK_SPACE_HAS_IMPLEM

namespace lak
{
    transform_t& transform_t::addTranslation(const LAK_SPACE_VEC3& displace)
    {
        translation = LAK_SPACE_TRANSLATE(translation, displace);
        return *this;
    }
    transform_t& transform_t::addTranslation(LAK_SPACE_VEC3&& displace)
    {
        return addTranslation(displace);
    }

    transform_t& transform_t::setTranslation(const LAK_SPACE_VEC3& position)
    {
        translation = LAK_SPACE_TRANSLATE(IDEN, position);
        return *this;
    }
    transform_t& transform_t::setTranslation(LAK_SPACE_VEC3&& position)
    {
        return setTranslation(position);
    }

    transform_t& transform_t::addScale(const LAK_SPACE_VEC3& sca)
    {
        scale = LAK_SPACE_SCALE(scale, sca + LAK_SPACE_VEC3(1.0f, 1.0f, 1.0f));
        return *this;
    }
    transform_t& transform_t::addScale(LAK_SPACE_VEC3&& sca)
    {
        return addScale(sca);
    }

    transform_t& transform_t::setScale(const LAK_SPACE_VEC3& sca)
    {
        scale = LAK_SPACE_SCALE(IDEN, sca);
        return *this;
    }
    transform_t& transform_t::setScale(LAK_SPACE_VEC3&& sca)
    {
        return setScale(sca);
    }

    transform_t& transform_t::addRotation(const LAK_SPACE_VEC3& angles)
    {
        rotation = LAK_SPACE_ROTATE(rotation, angles.z, LAK_SPACE_VEC3(ZUP));
        rotation = LAK_SPACE_ROTATE(rotation, angles.y, LAK_SPACE_VEC3(YUP));
        rotation = LAK_SPACE_ROTATE(rotation, angles.x, LAK_SPACE_VEC3(XUP));
        return *this;
    }
    transform_t& transform_t::addRotation(LAK_SPACE_VEC3&& angles)
    {
        return addRotation(angles);
    }

    transform_t& transform_t::setRotation(const LAK_SPACE_VEC3& angles)
    {
        rotation = LAK_SPACE_ROTATE(IDEN, angles.z, LAK_SPACE_VEC3(ZUP));
        rotation = LAK_SPACE_ROTATE(rotation, angles.y, LAK_SPACE_VEC3(YUP));
        rotation = LAK_SPACE_ROTATE(rotation, angles.x, LAK_SPACE_VEC3(XUP));
        return *this;
    }
    transform_t& transform_t::setRotation(LAK_SPACE_VEC3&& angles)
    {
        return setRotation(angles);
    }

    transform_t& transform_t::clear()
    {
        translation = IDEN;
        rotation = IDEN;
        scale = IDEN;
        return *this;
    }
    transform_t& transform_t::make(bool inverted)
    {
        if(inverted) combined = translation * rotation * scale;
        else combined = scale * rotation * translation;
        // combined = translation * rotation * scale;
        return *this;
    }

    const LAK_SPACE_VEC4 transform_t::XUP = {1.0f, 0.0f, 0.0f, 0.0f};
    const LAK_SPACE_VEC4 transform_t::YUP = {0.0f, 1.0f, 0.0f, 0.0f};
    const LAK_SPACE_VEC4 transform_t::ZUP = {0.0f, 0.0f, 1.0f, 0.0f};
    const LAK_SPACE_VEC4 transform_t::WUP = {0.0f, 0.0f, 0.0f, 1.0f};
    const LAK_SPACE_MAT4 transform_t::IDEN = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 1.0f, 0.0f, 
        0.0f, 0.0f, 0.0f, 1.0f
    };

    void deltaTransform_t::update(float delta)
    {
        rotVel += rotAccel * delta;
        rot += rotVel * delta;
        if(rot.z > M_2_PI || rot.z < -M_2_PI) rot.z = fmod(rot.z, M_2_PI);
        if(rot.y > M_2_PI || rot.y < -M_2_PI) rot.y = fmod(rot.y, M_2_PI);
        if(rot.x > M_2_PI || rot.x < -M_2_PI) rot.x = fmod(rot.x, M_2_PI);
        // addRotation(rot.z, ZUP);
        // addRotation(rot.y, YUP);
        // setRotation(rot.x, XUP);
        setRotation(rot);
        posVel += posAccel * delta;
        pos += posVel * delta;
        setTranslation(pos);
        make();
    }

    referenceFrame_t::~referenceFrame_t()
    {
        for(auto it = children.begin(); it != children.end(); it++)
            (*it)->parent = nullptr;
    }

    void referenceFrame_t::addChild(shared_ptr<referenceFrame_t> other)
    {
        children.push_back(other);
        other->parent = this;
    }

    void referenceFrame_t::removeChild(shared_ptr<referenceFrame_t> other)
    {
        for(auto it = children.begin(); it != children.end(); it++)
        {
            if(*it == other)
            {
                (*it)->parent = nullptr;
                it = children.erase(it);
            }
            else
            {
                it++;
            }
        }
    }

    void referenceFrame_t::removeChild(referenceFrame_t* other)
    {
        for(auto it = children.begin(); it != children.end(); it++)
        {
            if(it->get() == other)
            {
                (*it)->parent = nullptr;
                it = children.erase(it);
            }
            else
            {
                it++;
            }
        }
    }

    void referenceFrame_t::removeChild(size_t index)
    {
        auto it = children.begin() + index;
        (*it)->parent = nullptr;
        children.erase(it);
    }

    LAK_SPACE_MAT4 referenceFrame_t::getTransform(bool inverted, bool reversed)
    {
        LAK_SPACE_MAT4& trans = transform.make(reversed).combined;
        if(inverted)
        {
            if(parent != nullptr)
                return trans * parent->getTransform(true);
            else
                return trans;
        }
        else
        {
            if(parent != nullptr)
                return parent->getTransform(false) * trans;
            else
                return trans;
        }
    }

    #ifdef LAK_SPACE_IMGUI    
    template<>
    void TransformView<transform_t*>(transform_t* transform)
    {
        ImGui::PushID(transform);

        LAK_SPACE_VEC3 pretranslate = LAK_SPACE_VEC3(transform->translation * transform_t::WUP);
        LAK_SPACE_VEC3 prerotate = LAK_SPACE_VEC3(transform->rotation * LAK_SPACE_VEC4{1.0f, 1.0f, 1.0f, 0.0f});
        LAK_SPACE_VEC3 prescale = LAK_SPACE_VEC3(transform->scale * LAK_SPACE_VEC4{1.0f, 1.0f, 1.0f, 0.0f});

        LAK_SPACE_VEC3 posttranslate = pretranslate;
        LAK_SPACE_VEC3 postrotate = prerotate;
        LAK_SPACE_VEC3 postscale = prescale;

        ImGui::DragFloat3("Transl", &posttranslate[0], 0.01f);
        ImGui::DragFloat3("Rotate", &postrotate[0], 0.01f);
        ImGui::DragFloat3("Scale", &postscale[0], 0.01f);

        if(posttranslate != pretranslate) transform->setTranslation(posttranslate);
        if(postrotate != prerotate) transform->setRotation(postrotate);
        if(postscale != prescale) transform->setScale(postscale);

        ImGui::PopID();
    }
    template<>
    void TransformView<shared_ptr<transform_t>>(shared_ptr<transform_t> transform)
    {
        TransformView(static_cast<transform_t*>(transform.get()));
    }

    template<>
    void TransformView<deltaTransform_t*>(deltaTransform_t* transform)
    {
        ImGui::PushID(transform);

        ImGui::DragFloat3("Rot Accel", &transform->rotAccel[0], 0.01f);
        ImGui::DragFloat3("Rot Vel", &transform->rotVel[0], 0.01f);
        ImGui::DragFloat3("Rotate", &transform->rot[0], 0.01f);
        ImGui::DragFloat3("Tran Accel", &transform->posAccel[0], 0.01f);
        ImGui::DragFloat3("Tran Vel", &transform->posVel[0], 0.01f);
        ImGui::DragFloat3("Transl", &transform->pos[0], 0.01f);

        if(ImGui::TreeNode("Base Transform"))
        {
            TransformView(static_cast<transform_t*>(transform));
            ImGui::TreePop();
        }

        ImGui::PopID();
    }
    template<>
    void TransformView<shared_ptr<deltaTransform_t>>(shared_ptr<deltaTransform_t> transform)
    {
        TransformView(static_cast<deltaTransform_t*>(transform.get()));
    }

    template<>
    void TransformView<referenceFrame_t*>(referenceFrame_t* transform)
    {
        ImGui::PushID(transform);
        TransformView(&transform->transform);
        for(auto it = transform->children.begin(); it != transform->children.end(); it++)
        {
            if(ImGui::TreeNode(&**it, "Child %p", &**it))
            {
                TransformView(*it);
                ImGui::TreePop();
            }
        }
        ImGui::PopID();
    }
    template<>
    void TransformView<shared_ptr<referenceFrame_t>>(shared_ptr<referenceFrame_t> transform)
    {
        TransformView(static_cast<referenceFrame_t*>(transform.get()));
    }
    #endif
}

#endif // LAK_SPACE_HAS_IMPLEM
#endif // LAK_SPACE_IMPLEM