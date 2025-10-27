#include "entity_manager.h"
#include <iostream>

EntityManager::EntityManager() {
}

EntityManager::~EntityManager() {
    clear();
}

Entity* EntityManager::createEntity() {
    Entity* entity = new Entity();
    m_entities.push_back(entity);
    return entity;
}

void EntityManager::destroyEntity(Entity* entity) {
    if (entity) {
        entity->~Entity();
        m_entitiesToDestroy.push_back(entity);
    }
}

void EntityManager::update() {
    // Remove entities marked for destruction
    for (Entity* entity : m_entitiesToDestroy) {
        // Remove from main entities vector
        auto it = std::find(m_entities.begin(), m_entities.end(), entity);
        if (it != m_entities.end()) {
            m_entities.erase(it);
        }

        // Delete the entity
        delete entity;
    }

    m_entitiesToDestroy.clear();
}

void EntityManager::clear() {
    // Delete all entities
    for (Entity* entity : m_entities) {
        delete entity;
    }
    m_entities.clear();

    // Clear destruction queue
    m_entitiesToDestroy.clear();
}
