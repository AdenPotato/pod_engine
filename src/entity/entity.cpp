#include "entity.h"

Entity::Entity()
    : m_active(true)
{
}

Entity::~Entity() {
    // Clean up all components
    for (Component* component : m_components) {
        component->onDetach();
        delete component;
    }
    m_components.clear();
    m_componentMap.clear();
}
