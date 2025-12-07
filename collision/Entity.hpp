#pragma once

#include "Collider.hpp"
#include <memory>

namespace ne {

//Implementation for non-static colliders
//EX: "class Player : public Entity, public CircleColliderShape {...}"

class Entity : public virtual Collider {
public:
    virtual ~Entity() = default;

    using Collider::Collider;
    
    virtual void tick() = 0;
};

inline std::vector<std::unique_ptr<Entity>> Entities{};

} //namespace ne
