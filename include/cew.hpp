#pragma once

#include <cstdarg>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>

// =============================[ Entity ]=====================================

class World;

typedef std::uint_fast32_t Entity;

// =============================[ Component ]==================================

class Component
{
public:
    virtual ~Component() {}
    virtual void update() = 0;
    virtual std::shared_ptr<Component> clone() const = 0;

    World* world;
    Entity entity;
};

// =============================[ World ]======================================

typedef std::map<std::string, std::vector<std::shared_ptr<Component>>> ComponentRegistry;

class World
{
private:
    ComponentRegistry registry;
    size_t slots = 1;
    std::set<int> openSlots;

public:
    Entity addEntity()
    {
        if (openSlots.size() != 0) // Free slots available
        {
            // Remove the open slot
            int index = *(openSlots.begin());
            openSlots.erase(openSlots.begin());

            #ifdef DEBUG
            std::cout << "-- Added new Entity at #" << index << " (reused)" << std::endl;
            #endif

            return index;
        }
        else
        {
            // Resize ComponentRegistry
            for (auto it = registry.begin(); it != registry.end(); it++)
                it->second.resize(slots + 1);

            #ifdef DEBUG
            std::cout << "-- Added new Entity at #" << slots << std::endl;
            #endif

            // Add a slot for the new Entity
            return slots++;
        }
    }

    Entity addEntity(Entity other)
    {
        Entity e = addEntity();

        // Clone all Components of other Entity
        for (auto it = registry.begin(); it != registry.end(); it++)
            if (it->second.at(other) != nullptr)
                it->second.at(e) = it->second.at(other)->clone();

        return e;
    }

    template <class T>
    inline bool validComponent()
    {
        if (!std::is_base_of<Component, T>::value && !std::is_abstract<T>::value)
            return false;
        return true;
    }

    template <class T>
    void registerComponent()
    {
        if (!validComponent<T>())
            throw std::runtime_error("Not a valid Component - class must derive from Component");

        // Register on the ComponentRegistry
        std::string name = typeid(T).name();
        registry.insert(std::make_pair(name, std::vector<std::shared_ptr<Component>>()));

        #ifdef DEBUG
        std::cout << "-- Registered Component Type: " << name << std::endl;
        #endif
    }

    inline bool hasEntity(Entity entity)
    {
        if (openSlots.find(entity) != openSlots.end() || slots <= entity || entity == 0)
            return false;
        return true;
    }

    template <class T>
    inline bool hasComponent()
    {
        if (registry.find(typeid(T).name()) == registry.end())
            return false;
        return true;
    }

    template <class T>
    inline bool hasComponent(Entity entity)
    {
        std::string name = typeid(T).name();
        if (registry.find(name) == registry.end() || registry.at(name).at(entity) == nullptr)
            return false;
        return true;
    }

    template <class T, class ... Args>
    T& addComponent(Entity entity, Args&& ... args)
    {
        if (!validComponent<T>())
            throw std::runtime_error("Not a valid Component - class must derive from Component");

        if (!hasEntity(entity))
            throw std::runtime_error("Entity non-existent - call hasEntity() beforehand");

        if (!hasComponent<T>())
            throw std::runtime_error("Component not registered - call registerComponent() beforehand");

        // Add to ComponentRegistry
        std::string name = typeid(T).name();
        if (registry.at(name).at(entity) == nullptr)
        {
            registry.at(name).at(entity) = std::make_shared<T>(std::forward<Args>(args)...);
            registry.at(name).at(entity)->world = this;
            registry.at(name).at(entity)->entity = entity;

            #ifdef DEBUG
            std::cout << "-- Added Component " << name << " to Entity #" << entity << std::endl;
            #endif
        }
        #ifdef DEBUG
        else
        {
            std::cerr << "-- WARNING: Component " << name << " already present on Entity #" << entity << std::endl;
        }
        #endif

        return *std::dynamic_pointer_cast<T>(registry.at(name).at(entity));
    }

    template <class T>
    T& getComponent(Entity entity)
    {
        std::string name = typeid(T).name();
        if (!hasEntity(entity))
            throw std::runtime_error("Entity non-existent - call hasEntity() beforehand");
        if (!hasComponent<T>(entity))
            throw std::runtime_error("Component non-existent - call hasComponent() beforehand");
        return *std::dynamic_pointer_cast<T>(registry.at(typeid(T).name()).at(entity));
    }

    template <class T>
    void removeComponent(Entity entity)
    {
        std::string name = typeid(T).name();

        #ifdef DEBUG
        if (registry.at(name).at(entity) == nullptr)
            std::cerr << "-- WARNING: Component " << name << " already absent on Entity #" << entity << std::endl;
        #endif

        registry.at(name).at(entity) = nullptr;
    }

    void removeEntity(Entity& entity)
    {
        // Is the entity non-existent?
        if (!hasEntity(entity))
        {
            #ifdef DEBUG
            std::cerr << "-- WARNING: Entity #" << entity << " is already non-existent" << std::endl;
            #endif

            return;
        }

        // Remove from ComponentRegisry
        for (auto it = registry.begin(); it != registry.end(); it++)
            it->second.at(entity) = nullptr;

        // Is top entity?
        if (entity == slots)
            slots--;
        else
            openSlots.insert(entity);

        entity = 0;
    }

    void update()
    {
        // Update all Components
        for (auto it = registry.begin(); it != registry.end(); it++)
            for (int i = 1; i < slots && openSlots.find(i) == openSlots.end(); i++)
                if (it->second.at(i) != nullptr)
                    it->second.at(i)->update();
    }
};

