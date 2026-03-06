#pragma once

#include <variant>
#include <vector>


namespace ne {

inline namespace shapes {

struct Convex {
    std::vector<sf::Vector2f> points{{0.0f, 0.0f}, {100.0f, 0.0f}, {100.0f, 100.0f}};
};

struct Rectangle {
    sf::Vector2f size{100.0f, 100.0f};
};

struct Circle {
    float radius{50.0f};
};

} //inline namespace shapes

struct Collider {
    std::variant<Convex, Rectangle, Circle> shape{};

    [[nodiscard]] constexpr sf::Vector2f getSupport(sf::Vector2f directionalVector) const;
    [[nodiscard]] constexpr sf::FloatRect getBounds() const;
};

} //namespace ne



#include "Collider.inl"
