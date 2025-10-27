#pragma once

#include "entity.h"
#include <vector>
#include <algorithm>

class EntityManager {
public:
    EntityManager();
    ~EntityManager();

    // Create a new entity
    Entity* createEntity();

    // Destroy an entity
    void destroyEntity(Entity* entity);

    // Get all entities
    const std::vector<Entity*>& getAllEntities() const { return m_entities; }

    // Get entities with specific component
    template<typename T>
    std::vector<Entity*> getEntitiesWithComponent();

    // Get entities with multiple components
    template<typename T1, typename T2>
    std::vector<Entity*> getEntitiesWithComponents();

    // Update - removes dead entities
    void update();

    // Clear all entities
    void clear();

private:
    std::vector<Entity*> m_entities;
    std::vector<Entity*> m_entitiesToDestroy;
};

// Template implementations
template<typename T>
std::vector<Entity*> EntityManager::getEntitiesWithComponent() {
    std::vector<Entity*> result;
    for (Entity* entity : m_entities) {
        if (entity->isActive() && entity->hasComponent<T>()) {
            result.push_back(entity);
        }
    }
    return result;
}

template<typename T1, typename T2>
std::vector<Entity*> EntityManager::getEntitiesWithComponents() {
    std::vector<Entity*> result;
    for (Entity* entity : m_entities) {
        if (entity->isActive() &&
            entity->hasComponent<T1>() &&
            entity->hasComponent<T2>()) {
            result.push_back(entity);
        }
    }
    return result;
}
