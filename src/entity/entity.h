#pragma once

#include "transform.h"
#include "component.h"
#include <vector>
#include <typeindex>
#include <unordered_map>
#include <algorithm>

class Entity {
public:
    Entity();
    ~Entity();

    // Transform is always present
    Transform transform;

    // Component management
    template<typename T, typename... Args>
    T* addComponent(Args&&... args);

    template<typename T>
    T* getComponent();

    template<typename T>
    const T* getComponent() const;

    template<typename T>
    bool hasComponent() const;

    template<typename T>
    void removeComponent();

    // Entity active state
    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }

private:
    std::vector<Component*> m_components;
    std::unordered_map<std::type_index, Component*> m_componentMap;
    bool m_active;
};

// Template implementations
template<typename T, typename... Args>
T* Entity::addComponent(Args&&... args) {
    static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

    // Check if component already exists
    std::type_index typeIdx(typeid(T));
    if (m_componentMap.find(typeIdx) != m_componentMap.end()) {
        return static_cast<T*>(m_componentMap[typeIdx]);
    }

    // Create new component
    T* component = new T(std::forward<Args>(args)...);
    component->m_owner = this;

    m_components.push_back(component);
    m_componentMap[typeIdx] = component;

    component->onAttach();

    return component;
}

template<typename T>
T* Entity::getComponent() {
    static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

    std::type_index typeIdx(typeid(T));
    auto it = m_componentMap.find(typeIdx);
    if (it != m_componentMap.end()) {
        return static_cast<T*>(it->second);
    }
    return nullptr;
}

template<typename T>
const T* Entity::getComponent() const {
    static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

    std::type_index typeIdx(typeid(T));
    auto it = m_componentMap.find(typeIdx);
    if (it != m_componentMap.end()) {
        return static_cast<const T*>(it->second);
    }
    return nullptr;
}

template<typename T>
bool Entity::hasComponent() const {
    static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

    std::type_index typeIdx(typeid(T));
    return m_componentMap.find(typeIdx) != m_componentMap.end();
}

template<typename T>
void Entity::removeComponent() {
    static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

    std::type_index typeIdx(typeid(T));
    auto it = m_componentMap.find(typeIdx);
    if (it != m_componentMap.end()) {
        Component* component = it->second;
        component->onDetach();

        // Remove from vector
        m_components.erase(
            std::remove(m_components.begin(), m_components.end(), component),
            m_components.end()
        );

        // Remove from map
        m_componentMap.erase(it);

        delete component;
    }
}
