#pragma once

#include <SFML/Graphics.hpp>
#include <variant>
#include <vector>

namespace ne {

struct Convex {
    std::vector<sf::Vector2f> points;
};

struct Circle {
    float radius;
};

//I know this is considered bad practice, but it makes coding with an ECS so much more tolerable
using ColliderComponent         =   std::variant<Convex, Circle>;
using TransformableComponent    =   sf::Transformable;

} //namespace ne
