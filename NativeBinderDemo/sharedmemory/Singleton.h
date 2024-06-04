//
//  Singleton.h
//

#pragma once

#include <utility>

template<typename T>
class Singleton
{
    static T* s_Instance;

public:

    template<typename ...Args>
    static void Construct(Args&&... args)
    {
        if (s_Instance != nullptr)
            return;

        s_Instance = new T(std::forward<Args>(args)...);
    }

    static void Destroy()
    {
        delete s_Instance;
        s_Instance = nullptr;
    }

    static T* GetInstance() { return s_Instance; }
};

template<typename T> T* Singleton<T>::s_Instance = nullptr;
