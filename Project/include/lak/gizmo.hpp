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

#include <memory>
#include <lak/graphics.hpp>
#include <lak/space.hpp>

#ifndef LAK_GIZMO_H
#define LAK_GIZMO_H

namespace lak
{
    using std::make_shared;
    struct gizmo_t
    {
    // private:
        LAK_SPACE_MAT4 modelmat;
        bool isInit = false;
        mesh_t mesh;
        void init();
    public:
        gizmo_t();
        ~gizmo_t();

        gizmo_t(const LAK_SPACE_MAT4& transform);
        gizmo_t(LAK_SPACE_MAT4&& transform);

        gizmo_t(const LAK_SPACE_MAT4& transform, const LAK_SPACE_VEC4& vector);
        gizmo_t(LAK_SPACE_MAT4&& transform, const LAK_SPACE_VEC4& vector);
        gizmo_t(const LAK_SPACE_MAT4& transform, LAK_SPACE_VEC4&& vector);
        gizmo_t(LAK_SPACE_MAT4&& transform, LAK_SPACE_VEC4&& vector);

        gizmo_t(const LAK_SPACE_MAT4& from, const LAK_SPACE_MAT4& to);
        gizmo_t(LAK_SPACE_MAT4&& from, const LAK_SPACE_MAT4& to);
        gizmo_t(const LAK_SPACE_MAT4& from, LAK_SPACE_MAT4&& to);
        gizmo_t(LAK_SPACE_MAT4&& from, LAK_SPACE_MAT4&& to);

        void update(const LAK_SPACE_MAT4& transform);
        void update(LAK_SPACE_MAT4&& transform);

        void update(const LAK_SPACE_MAT4& transform, const LAK_SPACE_VEC4& vector);
        void update(LAK_SPACE_MAT4&& transform, const LAK_SPACE_VEC4& vector);
        void update(const LAK_SPACE_MAT4& transform, LAK_SPACE_VEC4&& vector);
        void update(LAK_SPACE_MAT4&& transform, LAK_SPACE_VEC4&& vector);

        void update(const LAK_SPACE_MAT4& from, const LAK_SPACE_MAT4& to);
        void update(LAK_SPACE_MAT4&& from, const LAK_SPACE_MAT4& to);
        void update(const LAK_SPACE_MAT4& from, LAK_SPACE_MAT4&& to);
        void update(LAK_SPACE_MAT4&& from, LAK_SPACE_MAT4&& to);

        void draw(const LAK_SPACE_MAT4& projview);
        void draw(LAK_SPACE_MAT4&& projview);
    };
}

#endif // LAK_GIZMO_H

// #define LAK_GIZMO_IMPLEM
#ifdef LAK_GIZMO_IMPLEM
#ifndef LAK_GIZMO_HAS_IMPLEM
#define LAK_GIZMO_HAS_IMPLEM

namespace lak
{
    gizmo_t::gizmo_t(){ init(); }

    gizmo_t::~gizmo_t(){ init(); }

    gizmo_t::gizmo_t(const LAK_SPACE_MAT4& transform)
    {
        update(transform);
    }
    gizmo_t::gizmo_t(LAK_SPACE_MAT4&& transform)
    {
        update(transform);
    }

    gizmo_t::gizmo_t(const LAK_SPACE_MAT4& transform, const LAK_SPACE_VEC4& vector)
    {
        update(transform, vector);
    }
    gizmo_t::gizmo_t(LAK_SPACE_MAT4&& transform, const LAK_SPACE_VEC4& vector)
    {
        update(transform, vector);
    }
    gizmo_t::gizmo_t(const LAK_SPACE_MAT4& transform, LAK_SPACE_VEC4&& vector)
    {
        update(transform, vector);
    }
    gizmo_t::gizmo_t(LAK_SPACE_MAT4&& transform, LAK_SPACE_VEC4&& vector)
    {
        update(transform, vector);
    }

    gizmo_t::gizmo_t(const LAK_SPACE_MAT4& from, const LAK_SPACE_MAT4& to)
    {
        LAK_SPACE_VEC4 vector = transform_t::WUP;
        vector = LAK_SPACE_INVERSE(from) * to * vector; // to -> world -> from space
        update(from, vector);
    }
    gizmo_t::gizmo_t(LAK_SPACE_MAT4&& from, const LAK_SPACE_MAT4& to)
    {
        LAK_SPACE_VEC4 vector = transform_t::WUP;
        vector = LAK_SPACE_INVERSE(from) * to * vector; // to -> world -> from space
        update(from, vector);
    }
    gizmo_t::gizmo_t(const LAK_SPACE_MAT4& from, LAK_SPACE_MAT4&& to)
    {
        LAK_SPACE_VEC4 vector = transform_t::WUP;
        vector = LAK_SPACE_INVERSE(from) * to * vector; // to -> world -> from space
        update(from, vector);
    }
    gizmo_t::gizmo_t(LAK_SPACE_MAT4&& from, LAK_SPACE_MAT4&& to)
    {
        LAK_SPACE_VEC4 vector = transform_t::WUP;
        vector = LAK_SPACE_INVERSE(from) * to * vector; // to -> world -> from space
        update(from, vector);
    }

    void gizmo_t::init()
    {
        if(isInit) return;
        isInit = true;
        mesh.shader = make_shared<shader_t>();
        mesh.shader->init(R"(
#version 330 core
in vec4 vPosition;
in vec4 vColor;
uniform mat4 projview;
uniform mat4 model;
out vec4 fColor;
void main()
{
    gl_Position = projview * model * vPosition;
    fColor = vColor;
})", R"(
#version 330 core
in vec4 fColor;
out vec4 pColor;
void main()
{
    pColor = fColor;
})");
    }

    void gizmo_t::update(const LAK_SPACE_MAT4& transform)
    {
        init();
        // mesh.shader->setUniform("model", &transform[0][0]);
        modelmat = transform;

        mesh_t::element_t& vpos = mesh.elements["vPosition"];
        LAK_SPACE_VEC4* vposp = vpos.init<LAK_SPACE_VEC4>(6);
        vposp[0] = {0.0f, 0.0f, 0.0f, 1.0f};
        vposp[1] = {1.0f, 0.0f, 0.0f, 1.0f};
        
        vposp[2] = {0.0f, 0.0f, 0.0f, 1.0f};
        vposp[3] = {0.0f, 1.0f, 0.0f, 1.0f};
        
        vposp[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        vposp[5] = {0.0f, 0.0f, 1.0f, 1.0f};
        vpos.normalized = false;
        vpos.active = true;

        mesh_t::element_t& vcol = mesh.elements["vColor"];
        LAK_SPACE_VEC4* vcolp = vcol.init<LAK_SPACE_VEC4>(6);
        vcolp[0] = {1.0f, 0.0f, 0.0f, 1.0f};
        vcolp[1] = {1.0f, 0.0f, 0.0f, 1.0f};
        
        vcolp[2] = {0.0f, 1.0f, 0.0f, 1.0f};
        vcolp[3] = {0.0f, 1.0f, 0.0f, 1.0f};
        
        vcolp[4] = {0.0f, 0.0f, 1.0f, 1.0f};
        vcolp[5] = {0.0f, 0.0f, 1.0f, 1.0f};
        vcol.normalized = false;
        vcol.active = true;

        mesh.drawMode = GL_LINES;

        // mesh.update();
    }
    void gizmo_t::update(LAK_SPACE_MAT4&& transform)
    {
        update(transform);
    }

    void gizmo_t::update(const LAK_SPACE_MAT4& transform, const LAK_SPACE_VEC4& vector)
    {
        init();
        // mesh.shader->setUniform("model", &transform[0][0]);
        modelmat = transform;

        mesh_t::element_t& vpos = mesh.elements["vPosition"];
        LAK_SPACE_VEC4* vposp = vpos.init<LAK_SPACE_VEC4>(2);
        vposp[0] = {0.0f, 0.0f, 0.0f, 1.0f};
        vposp[1] = vector;
        vpos.normalized = false;
        vpos.active = true;

        mesh_t::element_t& vcol = mesh.elements["vColor"];
        LAK_SPACE_VEC4* vcolp = vcol.init<LAK_SPACE_VEC4>(2);
        vcolp[0] = {0.0f, 0.0f, 0.0f, 1.0f};
        vcolp[1] = LAK_SPACE_VEC4(LAK_SPACE_NOMALIZE(LAK_SPACE_VEC3(vector)), 1.0f);
        vcol.normalized = false;
        vcol.active = true;

        mesh.drawMode = GL_LINES;

        // mesh.update();
    }
    void gizmo_t::update(LAK_SPACE_MAT4&& transform, const LAK_SPACE_VEC4& vector)
    {
        update(transform, vector);
    }
    void gizmo_t::update(const LAK_SPACE_MAT4& transform, LAK_SPACE_VEC4&& vector)
    {
        update(transform, vector);
    }
    void gizmo_t::update(LAK_SPACE_MAT4&& transform, LAK_SPACE_VEC4&& vector)
    {
        update(transform, vector);
    }

    void gizmo_t::update(const LAK_SPACE_MAT4& from, const LAK_SPACE_MAT4& to)
    {
        LAK_SPACE_VEC4 vector = transform_t::WUP;
        vector = LAK_SPACE_INVERSE(from) * to * vector; // to -> world -> from space
        update(from, vector);
    }
    void gizmo_t::update(LAK_SPACE_MAT4&& from, const LAK_SPACE_MAT4& to)
    {
        LAK_SPACE_VEC4 vector = transform_t::WUP;
        vector = LAK_SPACE_INVERSE(from) * to * vector; // to -> world -> from space
        update(from, vector);
    }
    void gizmo_t::update(const LAK_SPACE_MAT4& from, LAK_SPACE_MAT4&& to)
    {
        LAK_SPACE_VEC4 vector = transform_t::WUP;
        vector = LAK_SPACE_INVERSE(from) * to * vector; // to -> world -> from space
        update(from, vector);
    }
    void gizmo_t::update(LAK_SPACE_MAT4&& from, LAK_SPACE_MAT4&& to)
    {
        LAK_SPACE_VEC4 vector = transform_t::WUP;
        vector = LAK_SPACE_INVERSE(from) * to * vector; // to -> world -> from space
        update(from, vector);
    }

    void gizmo_t::draw(const LAK_SPACE_MAT4& projview)
    {
        mesh.shader->setUniform("projview", &projview[0][0]);
        mesh.shader->setUniform("model", &modelmat[0][0]);
        mesh.update();
        mesh.draw();
    }

    void gizmo_t::draw(LAK_SPACE_MAT4&& projview)
    {
        mesh.shader->setUniform("projview", &projview[0][0]);
        mesh.shader->setUniform("model", &modelmat[0][0]);
        mesh.update();
        mesh.draw();
    }
}

#endif // LAK_GIZMO_HAS_IMPLEM
#endif // LAK_GIZMO_IMPLEM