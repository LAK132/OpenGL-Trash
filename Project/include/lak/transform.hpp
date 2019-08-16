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

/*
    Dual Quaternion concepts from "A Beginners Guide to Dual-Quaternions" (Ben Kenwright)
    https://cs.gmu.edu/~jmlien/teaching/cs451/uploads/Main/dual-quaternion.pdf
    https://web.archive.org/web/20180622093933/https://cs.gmu.edu/~jmlien/teaching/cs451/uploads/Main/dual-quaternion.pdf
*/

#include <cmath>
#include <type_traits>
// #include <lak/mstream.hpp>

#ifndef DEBUG
#   ifdef NDEBUG
#       define DEBUG(x)
#   else
#       include <iostream>
#       define DEBUG(x) std::cout << __FILE__ << "(" << std::dec << __LINE__ << ") " << x
#   endif
#endif

#ifndef LAK_TRANSFORM_MAT4
#include <glm/mat4x4.hpp>
#define LAK_TRANSFORM_MAT4 glm::mat4
#endif

#ifndef LAK_TRANSFORM_VEC3
#include <glm/vec3.hpp>
#define LAK_TRANSFORM_VEC3 glm::vec3
#endif

#ifndef LAK_TRANSFORM_VEC2
#include <glm/vec2.hpp>
#define LAK_TRANSFORM_VEC2 glm::vec2
#endif

#ifndef LAK_TRANSFORM_H
#define LAK_TRANSFORM_H

namespace lak
{
    using mat4_t = LAK_TRANSFORM_MAT4;
    using vec3_t = LAK_TRANSFORM_VEC3;
    using vec2_t = LAK_TRANSFORM_VEC2;

    // template <>
    // mstream &operator>><vec3_t &>(mstream &strm, vec3_t &obj)
    // {
    //     strm >> obj.x >> obj.y >> obj.z;
    //     return strm;
    // }
    // template <>
    // mstream &operator<<<vec3_t &>(mstream &strm, vec3_t &obj)
    // {
    //     strm << obj.x << obj.y << obj.z;
    //     return strm;
    // }
    // template <>
    // mstream &operator>><vec2_t &>(mstream &strm, vec2_t &obj)
    // {
    //     strm >> obj.x >> obj.y;
    //     return strm;
    // }
    // template <>
    // mstream &operator<<<vec2_t &>(mstream &strm, vec2_t &obj)
    // {
    //     strm << obj.x << obj.y;
    //     return strm;
    // }

    #if defined(LAK_TRANSFORM_BIGGER_TYPE_ADD)
    template<typename L, typename R>
    using biggerType = decltype(L(0) + R(0));
    #elif defined(LAK_TRANSFORM_BIGGER_TYPE_SUB)
    template<typename L, typename R>
    using biggerType = decltype(L(0) - R(0));
    #else 
    template<typename L, typename R>
    using biggerType = decltype(L(0) * R(0));
    #endif

    template<typename T> struct quaternion_t;

    template<typename T>
    struct quaternion_t
    {
        typedef typename std::decay<T>::type _value_type;
        typedef quaternion_t<_value_type> _type;
        _value_type x;
        _value_type y;
        _value_type z;
        _value_type w;

        quaternion_t() : x(_value_type(0)), y(_value_type(0)), z(_value_type(0)), w(_value_type(1)) {}
        // quaternion_t(const _value_type& X, const _value_type& Y, const _value_type& Z, const _value_type& W) : x(X), y(Y), z(Z), w(W) {}
        // quaternion_t(_value_type&& X, _value_type&& Y, _value_type&& Z, _value_type&& W) : x(X), y(Y), z(Z), w(W) {}
        quaternion_t(_value_type X, _value_type Y, _value_type Z, _value_type W) : x(X), y(Y), z(Z), w(W) {}
        quaternion_t(const _value_type XYZW[4]) : x(XYZW[0]), y(XYZW[1]), z(XYZW[2]), w(XYZW[3]) {}
        template<typename R>
        quaternion_t(const quaternion_t<R>& other)
        {
            *this = other;
        }
        template<typename R>
        quaternion_t(quaternion_t<R>&& other)
        {
            *this = other;
        }
        template<typename R>
        _type& operator=(const quaternion_t<R>& rhs)
        {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
            w = rhs.w;
            return *this;
        }
        template<typename R>
        _type& operator=(quaternion_t<R>&& rhs)
        {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
            w = rhs.w;
            return *this;
        }
        _type operator*() // abuse the derefernce operator for conjugate operation
        {
            return _type(-x, -y, -z, w);
        }
        _type conjugate()
        {
            return _type(-x, -y, -z, w);
        }
        _value_type operator~() // magnitude
        {
            // sqrt(this->dot(*this)) // overkill
            // sqrt((*this * this->conjugate()).w) // overkill
            return sqrt((x*x)+(y*y)+(z*z)+(w*w));
        }
        _value_type magnitude()
        {
            return sqrt((x*x)+(y*y)+(z*z)+(w*w));
        }
        _type norm()
        {
            return *this * (_value_type(1) / magnitude());
        }
        _type& normalize()
        {
            return *this *= _value_type(1) / magnitude();
        }
        _value_type& operator[](const size_t& index) // useful for &(qrtn[0])
        {
            return *((&x)+index);
        }
        _value_type& operator[](size_t&& index) // useful for &(qrtn[0])
        {
            return *((&x)+index);
        }
        _value_type* operator[](const char& index)
        {
            switch(index)
            {
                case 'x': return &x;
                case 'y': return &y;
                case 'z': return &z;
                case 'w': return &w;
            }
            return nullptr;
        }
        template<typename R>
        bool operator==(const quaternion_t<R>& rhs)
        {
            return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
        }
        template<typename R>
        bool operator==(quaternion_t<R>&& rhs)
        {
            return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
        }
        template<typename R>
        bool operator!=(const quaternion_t<R>& rhs)
        {
            return x != rhs.x && y != rhs.y && z != rhs.z && w != rhs.w;
        }
        template<typename R>
        bool operator!=(quaternion_t<R>&& rhs)
        {
            return x != rhs.x && y != rhs.y && z != rhs.z && w != rhs.w;
        }
        template<typename R>
        biggerType<_value_type, typename std::decay<R>::type> dot(const quaternion_t<R>& rhs)
        {
            return (x * rhs.x) + (y * rhs.y) + (z * rhs.z) + (w * rhs.w);
        }
        template<typename R>
        biggerType<_value_type, typename std::decay<R>::type> dot(quaternion_t<R>&& rhs)
        {
            return (x * rhs.x) + (y * rhs.y) + (z * rhs.z) + (w * rhs.w);
        }
        template<typename R>
        _type& operator+=(const quaternion_t<R>& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            w += rhs.w;
            return *this;
        }
        template<typename R>
        _type& operator+=(quaternion_t<R>&& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            w += rhs.w;
            return *this;
        }
        
        template<typename R>
        quaternion_t<biggerType<_value_type, R>> operator+(const quaternion_t<R>& rhs)
        {
            return quaternion_t<biggerType<_value_type, R>>(x+rhs.x, y+rhs.y, z+rhs.z, z+rhs.z);
        }
        template<typename R>
        quaternion_t<biggerType<_value_type, R>> operator+(quaternion_t<R>&& rhs)
        {
            return quaternion_t<biggerType<_value_type, R>>(x+rhs.x, y+rhs.y, z+rhs.z, z+rhs.z);
        }

        template<typename R>
        _type& operator*=(R rhs) // scalar mult
        {
            x *= rhs;
            y *= rhs;
            z *= rhs;
            w *= rhs;
            return *this;
        }
        template<typename R>
        _type& operator*=(const quaternion_t<R>& rhs)
        {
            _value_type _x = ((y * rhs.z) - (z * rhs.y)) + (w * rhs.x) + (x * rhs.w);
            _value_type _y = ((z * rhs.x) - (x * rhs.z)) + (w * rhs.y) + (y * rhs.w);
            _value_type _z = ((x * rhs.y) - (y * rhs.x)) + (w * rhs.z) + (z * rhs.w);
            _value_type _w = (w * rhs.w) - ((x * rhs.x) + (y * rhs.y) + (z * rhs.z));
            x = _x; y = _y; z = _z; w = _w;
            return *this;
        }
        template<typename R>
        _type& operator*=(quaternion_t<R>&& rhs)
        {
            _value_type _x = ((y * rhs.z) - (z * rhs.y)) + (w * rhs.x) + (x * rhs.w);
            _value_type _y = ((z * rhs.x) - (x * rhs.z)) + (w * rhs.y) + (y * rhs.w);
            _value_type _z = ((x * rhs.y) - (y * rhs.x)) + (w * rhs.z) + (z * rhs.w);
            _value_type _w = (w * rhs.w) - ((x * rhs.x) + (y * rhs.y) + (z * rhs.z));
            x = _x; y = _y; z = _z; w = _w;
            return *this;
        }
        
        template<typename R>
        quaternion_t<biggerType<_value_type, typename std::decay<R>::type>> operator*(R rhs)
        {
            quaternion_t<biggerType<_value_type, typename std::decay<R>::type>> rtn = *this;
            return rtn *= rhs;
        }
        template<typename R>
        quaternion_t<biggerType<_value_type, R>> operator*(const quaternion_t<R>& rhs)
        {
            quaternion_t<biggerType<_value_type, R>> rtn = *this;
            return rtn *= rhs;
        }
        template<typename R>
        quaternion_t<biggerType<_value_type, R>> operator*(quaternion_t<R>&& rhs)
        {
            quaternion_t<biggerType<_value_type, R>> rtn = *this;
            return rtn *= rhs;
        }
        template<typename L>
        friend quaternion_t<biggerType<_value_type, typename std::decay<L>::type>> operator*(L lhs, quaternion_t<_value_type> rhs)
        {
            quaternion_t<biggerType<_value_type, typename std::decay<L>::type>> rtn = rhs;
            return rtn *= lhs;
        }
    };

    // template <typename T>
    // mstream &operator>><quaternion_t<T>>(mstream &strm, quaternion_t<T> &obj)
    // {
    //     strm >> obj.x >> obj.y >> obj.z >> obj.w;
    //     return strm;
    // }
    // template <typename T>
    // mstream &operator<<<quaternion_t<T>>(mstream &strm, quaternion_t<T> &obj)
    // {
    //     strm << obj.x << obj.y << obj.z << obj.w;
    //     return strm;
    // }

    typedef quaternion_t<float> quaternionf_t;
    typedef quaternion_t<double> quaterniond_t;

    template<typename T>
    struct dualquaternion_t
    {
        typedef typename std::decay<T>::type _value_type;
        typedef quaternion_t<_value_type> _qtrn_type;
        typedef dualquaternion_t<_value_type> _type;
        _qtrn_type real;
        _qtrn_type dual;

        dualquaternion_t() : real(_value_type(0), _value_type(0), _value_type(0), _value_type(1)), dual(_value_type(0), _value_type(0), _value_type(0), _value_type(0)) {}
        // dualquaternion_t(const _qtrn_type& R, const _qtrn_type& D) : real(R), dual(D) {}
        // dualquaternion_t(_qtrn_type&& R, _qtrn_type&& D) : real(R), dual(D) {}
        dualquaternion_t(_qtrn_type R, _qtrn_type D) : real(R), dual(D) {}
        dualquaternion_t(const _qtrn_type RD[2]) : real(RD[0]), dual(RD[1]) {}
        // dualquaternion_t(
        //     const _value_type& rX, const _value_type& rY, const _value_type& rZ, const _value_type& rW, 
        //     const _value_type& dX, const _value_type& dY, const _value_type& dZ, const _value_type& dW
        // ) : real(rX, rY, rZ, rW), dual(dX, dY, dZ, dW) {}
        // dualquaternion_t(
        //     _value_type&& rX, _value_type&& rY, _value_type&& rZ, _value_type&& rW, 
        //     _value_type&& dX, _value_type&& dY, _value_type&& dZ, _value_type&& dW
        // ) : real(rX, rY, rZ, rW), dual(dX, dY, dZ, dW) {}
        dualquaternion_t(
            _value_type rX, _value_type rY, _value_type rZ, _value_type rW, 
            _value_type dX, _value_type dY, _value_type dZ, _value_type dW
        ) : real(rX, rY, rZ, rW), dual(dX, dY, dZ, dW) {}
        dualquaternion_t(const _value_type vals[8]) : real(vals[0], vals[1], vals[2], vals[3]), dual(vals[4], vals[5], vals[6], vals[7]) {}
        _type& fromRotTrans(const _qtrn_type& rot, _qtrn_type trans)
        {
            real = rot;
            trans.w = _value_type(0);
            dual = _value_type(0.5) * trans * rot;
            return *this;
        }
        _type& fromRotTrans(_qtrn_type&& rot, _qtrn_type&& trans)
        {
            real = rot;
            trans.w = _value_type(0);
            dual = _value_type(0.5) * trans * rot;
            return *this;
        }
        _type& fromEuler(const _qtrn_type& rot, _qtrn_type trans)
        {
            real.w = cos(rot.w / 2);
            _value_type srw2 = sin(rot.w / 2);// / sqrt((rot.x*rot.x) + (rot.y*rot.y) + (rot.z*rot.z));
            real.x = rot.x * srw2;
            real.y = rot.y * srw2;
            real.z = rot.z * srw2;
            trans.w = _value_type(0);
            dual = _value_type(0.5) * trans * real;
            return *this;
        }
        _type& fromEuler(_qtrn_type&& rot, _qtrn_type&& trans)
        {
            real.w = cos(rot.w / 2);
            _value_type srw2 = sin(rot.w / 2);
            real.x = rot.x * srw2;
            real.y = rot.y * srw2;
            real.z = rot.z * srw2;
            trans.w = _value_type(0);
            dual = _value_type(0.5) * trans * real;
            return *this;
        }
        _qtrn_type rotation()
        {
            return real;
        }
        _qtrn_type eulerRotation()
        {
            _qtrn_type rtn;
            rtn.w = acos(real.w) * 2;
            _value_type srw2 = sin(rtn.w / 2);
            if(srw2 == 0.0f)
            {
                rtn.x = 0.0f;
                rtn.y = 0.0f;
                rtn.z = 0.0f;
            }
            else
            {
                rtn.x = real.x / srw2;
                rtn.y = real.y / srw2;
                rtn.z = real.z / srw2;
            }
            return rtn;
        }
        _qtrn_type translation()
        {
            _qtrn_type& trans = (dual * _value_type(2)) * *real;
            trans.w = _value_type(0);
            return trans;
        }
        mat4_t transform()
        {
            normalize();
            _qtrn_type& trans = translation();
            _value_type &x = real.x, 
                &y = real.y, 
                &z = real.z, 
                &w = real.w;
            const _value_type t = 2;

            return mat4_t {
                (w*w) + (x*x) - (y*y) - (z*z), 
                (t*x*y) + (t*w*z), 
                (t*x*z) - (t*w*y), 
                _value_type(0),

                (t*x*y) - (t*w*z), 
                (w*w) + (y*y) - (x*x) - (z*z), 
                (t*y*z) + (t*w*x), 
                _value_type(0),

                (t*x*z) + (t*w*y), 
                (t*y*z) - (t*w*x), 
                (w*w) + (z*z) - (x*x) - (y*y), 
                _value_type(0),

                trans.x, 
                trans.y, 
                trans.z, 
                _value_type(1)
            };
        }
        _type operator*() // abuse the derefernce operator for conjugate operation
        {
            return _type(*real, *dual);
        }
        _type conjugate()
        {
            return _type(*real, *dual);
        }
        _value_type operator~() // magnitude
        {
            // ||dq|| = sqrt(dq*(*dq))
            // dq1*dq2 = dq1.r*dq2.r + (dq1.r*dq2.d + dq1.d*dq2.r)ε
            // dq*(*dq) = dq.r*(*dq.r) + (dq.r*(*dq.d) + dq.d*(*dq.r))ε
            // dq*(*dq) = dq.r*(*dq.r) + (0)ε
            // ||dq|| = ||dq.r||
            return ~real;
        }
        _value_type magnitude()
        {
            return ~real;
        }
        _type norm()
        {
            return *this * (_value_type(1) / magnitude());
        }
        _type& normalize()
        {
            return *this *= _value_type(1) / magnitude();
        }
        _value_type& operator[](const size_t& index) // useful for &(qrtn[0])
        {
            return *((&real)+index);
        }
        _value_type& operator[](size_t&& index) // useful for &(qrtn[0])
        {
            return *((&real)+index);
        }
        _value_type* operator[](const char& index)
        {
            switch(index)
            {
                case 'r': return &real;
                case 'd': return &dual;
            }
            return nullptr;
        }
        template<typename R>
        bool operator==(const dualquaternion_t<R>& rhs)
        {
            return real == rhs.real && dual == rhs.dual;
        }
        template<typename R>
        bool operator==(dualquaternion_t<R>&& rhs)
        {
            return real == rhs.real && dual == rhs.dual;
        }
        template<typename R>
        bool operator!=(const dualquaternion_t<R>& rhs)
        {
            return real != rhs.real && dual != rhs.dual;
        }
        template<typename R>
        bool operator!=(dualquaternion_t<R>&& rhs)
        {
            return real != rhs.real && dual != rhs.dual;
        }
        template<typename R>
        quaternion_t<biggerType<_value_type, typename std::decay<R>::type>> dot(const dualquaternion_t<R>& rhs)
        {
            return real.dot(rhs.real);
        }
        template<typename R>
        quaternion_t<biggerType<_value_type, typename std::decay<R>::type>> dot(dualquaternion_t<R>&& rhs)
        {
            return real.dot(rhs.real);
        }
        template<typename R>
        _type& operator=(const dualquaternion_t<R>& rhs)
        {
            real = rhs.real;
            dual = rhs.dual;
            return *this;
        }
        template<typename R>
        _type& operator=(dualquaternion_t<R>&& rhs)
        {
            real = rhs.real;
            dual = rhs.dual;
            return *this;
        }
        template<typename R>
        _type& operator+=(const dualquaternion_t<R>& rhs)
        {
            real += rhs.real;
            dual += rhs.dual;
            return *this;
        }
        template<typename R>
        _type& operator+=(dualquaternion_t<R>&& rhs)
        {
            real += rhs.real;
            dual += rhs.dual;
            return *this;
        }
        
        template<typename R>
        dualquaternion_t<biggerType<_value_type, R>> operator+(const dualquaternion_t<R>& rhs)
        {
            return dualquaternion_t<biggerType<_value_type, R>>(real+rhs.real, dual+rhs.dual);
        }
        template<typename R>
        dualquaternion_t<biggerType<_value_type, R>> operator+(dualquaternion_t<R>&& rhs)
        {
            return dualquaternion_t<biggerType<_value_type, R>>(real+rhs.real, dual+rhs.dual);
        }

        template<typename R>
        _type& operator*=(R rhs) // scalar mult
        {
            real *= rhs;
            dual *= rhs;
            return *this;
        }
        template<typename R>
        _type& operator*=(const dualquaternion_t<R>& rhs)
        {
            // NOTE: current real is needed to calculate new dual! beware of order!
            dual = (real * rhs.dual) + (dual * rhs.real);
            real *= rhs.real;
            return *this;
        }
        template<typename R>
        _type& operator*=(dualquaternion_t<R>&& rhs)
        {
            dual = (real * rhs.dual) + (dual * rhs.real);
            real *= rhs.real;
            return *this;
        }
        
        template<typename R>
        dualquaternion_t<biggerType<_value_type, typename std::decay<R>::type>> operator*(R rhs)
        {
            dualquaternion_t<biggerType<_value_type, typename std::decay<R>::type>> rtn = *this;
            return rtn *= rhs;
        }
        template<typename R>
        dualquaternion_t<biggerType<_value_type, R>> operator*(const dualquaternion_t<R>& rhs)
        {
            dualquaternion_t<biggerType<_value_type, R>> rtn = *this;
            return rtn *= rhs;
        }
        template<typename R>
        dualquaternion_t<biggerType<_value_type, R>> operator*(dualquaternion_t<R>&& rhs)
        {
            dualquaternion_t<biggerType<_value_type, R>> rtn = *this;
            return rtn *= rhs;
        }
        template<typename L>
        friend dualquaternion_t<biggerType<_value_type, typename std::decay<L>::type>> operator*(L lhs, dualquaternion_t<_value_type> rhs)
        {
            dualquaternion_t<biggerType<_value_type, typename std::decay<L>::type>> rtn = rhs;
            return rtn *= lhs;
        }
    };

    // template <typename T>
    // mstream &operator>><dualquaternion_t<T>>(mstream &strm, dualquaternion_t<T> &obj)
    // {
    //     strm >> obj.real >> obj.dual;
    //     return strm;
    // }
    // template <typename T>
    // mstream &operator<<<dualquaternion_t<T>>(mstream &strm, dualquaternion_t<T> &obj)
    // {
    //     strm << obj.real << obj.dual;
    //     return strm;
    // }

    typedef dualquaternion_t<float> dualquaternionf_t;
    typedef dualquaternion_t<double> dualquaterniond_t;
}

#endif // LAK_TRANSFORM_H

// #define LAK_TRANSFORM_IMPLEM

#ifdef LAK_TRANSFORM_IMPLEM
#ifndef LAK_TRANSFORM_HAS_IMPLEM
#define LAK_TRANSFORM_HAS_IMPLEM

namespace lak
{
    
}

#endif // LAK_TRANSFORM_HAS_IMPLEM
#endif // LAK_TRANSFORM_IMPLEM