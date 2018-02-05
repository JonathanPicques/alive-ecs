#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <type_traits>

#include "component.hpp"

class EntityManager;

class Entity final
{
public:
    friend EntityManager;

public:
    using PointerSize = std::uint16_t;

public:
    class Pointer final
    {
    public:
        friend EntityManager;

    public:
        Pointer(PointerSize index, PointerSize version);

    public:
        Pointer(Pointer&&) = default;
        Pointer(Pointer const&) = default;
        Pointer& operator=(Pointer const&) = default;

    public:
        friend bool operator==(const Pointer& a, const Pointer& b);
        friend bool operator!=(const Pointer& a, const Pointer& b);

    private:
        PointerSize mIndex = {};
        PointerSize mVersion = {};
    };

public:
    explicit Entity(EntityManager* manager, Pointer pointer);

public:
    Entity(Entity&&) = default;
    Entity(Entity const&) = default;
    Entity& operator=(Entity const&) = default;

public:
    friend bool operator==(const Entity& a, const Entity& b);

public:
    template<typename C>
    C* GetComponent();
    template<typename C>
    const C* GetComponent() const;
    template<typename C>
    C* AddComponent();
    template<typename C>
    void RemoveComponent();
    template<typename C>
    bool HasComponent() const;
    template<typename C1, typename C2, typename ...C>
    bool HasComponent() const;
    template<typename C>
    bool HasAnyComponent() const;
    template<typename C1, typename C2, typename ...C>
    bool HasAnyComponent() const;

public:
    template<typename ...C>
    bool Any(typename std::common_type<std::function<void(C* ...)>>::type view);
    template<typename ...C>
    bool With(typename std::common_type<std::function<void(C* ...)>>::type view);

public:
    void ResolveComponentDependencies();

public:
    void Destroy();
    bool IsDestroyed() const;

private:
    EntityManager* mManager = nullptr;
    Pointer mPointer;
};