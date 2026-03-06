#pragma once

#include "components/Collider.hpp"
#include "components/Manifold.hpp"

#include <optional>


namespace ne {

[[nodiscard]] bool intersects(const Collider& colliderA, const sf::Transformable& transformableA, const Collider& colliderB, const sf::Transformable& transformableB);

[[nodiscard]] std::optional<Manifold> findPenetration(const Collider& colliderA, const sf::Transformable& transformableA, const Collider& colliderB, const sf::Transformable& transformableB);

} //namespace ne