#pragma once

#include "Components.hpp"

#include <SFML/System/Vector2.hpp>
#include <optional>


namespace ne {

std::optional<sf::Vector2f> resolveCollision(ColliderComponent collider_a, TransformableComponent transformable_a, ColliderComponent collider_b, TransformableComponent transformable_b);

} //namespace ne
