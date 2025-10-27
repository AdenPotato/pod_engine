#pragma once

// Forward declaration
class Entity;

// Base class for all components
class Component {
public:
    Component() : m_owner(nullptr) {}
    virtual ~Component() = default;

    // Get the entity that owns this component
    Entity* getOwner() const { return m_owner; }

    // Called when component is added to entity
    virtual void onAttach() {}

    // Called when component is removed from entity
    virtual void onDetach() {}

protected:
    Entity* m_owner;

    friend class Entity;  // Allow Entity to set owner
};
