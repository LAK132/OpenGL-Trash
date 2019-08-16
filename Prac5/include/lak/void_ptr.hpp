#include <type_traits>

#ifndef LAK_VOID_PTR_H
#define LAK_VOID_PTR_H

namespace lak
{
    using std::remove_reference;
    struct void_ptr
    {
        void(*deleter)(void*) = [](void*){};
        void* ptr = nullptr;
        void_ptr(){}
        ~void_ptr() { deleter(ptr); }
        void_ptr& operator=(void_ptr& other) // transfers ownership
        {
            deleter(ptr);
            deleter = other.deleter;
            ptr = other.ptr;
            other.deleter = [](void*){};
            return *this;
        }
        void_ptr& operator=(void_ptr&& other) { return *this = other; }
        template <typename T>
        void init()
        {
            deleter = [](void* ptr){
                if(ptr == nullptr) return;
                T* t = static_cast<T*>(ptr);
                delete t;
            };
            ptr = new (std::nothrow) T;
        }
        template <typename T>
        void init(void* _ptr, void(*_deleter)(void*) = [](void*)
            { if(ptr == nullptr) return; T* t = static_cast<T*>(ptr); delete t; })
        {
            ptr = _ptr;
            deletor = _deleter;
        }
        template <typename T>
        void init(T* _ptr, void(*_deleter)(void*) = [](void*)
            { if(ptr == nullptr) return; T* t = static_cast<T*>(ptr); delete t; })
        {
            ptr = _ptr;
            deletor = _deleter;
        }
        template <typename T>
        void_ptr& operator=(const typename remove_reference<T>::type& other)
        {
            init<T>();
            if(ptr != nullptr) *static_cast<T*>(ptr) = other;
            return *this;
        }
        template <typename T>
        void_ptr& operator=(typename remove_reference<T>::type&& other)
        {
            init<T>();
            if(ptr != nullptr) *static_cast<T*>(ptr) = other;
            return *this;
        }
        template <typename T>
        T* getPtr()
        {
            return static_cast<T*>(ptr);
        }
        template <typename T>
        T& getRef()
        {
            return *static_cast<T*>(ptr);
        }
    };
}

#endif // LAK_VOID_PTR_H