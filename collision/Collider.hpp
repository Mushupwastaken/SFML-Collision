#pragma once

#include <SFML/Graphics.hpp>
#include <optional>

#include <vector>
#include <memory>

/*
    SOURCES:
    https://blog.hamaluik.ca/posts/building-a-collision-engine-part-1-2d-gjk-collision-detection/
    https://winter.dev/articles/epa-algorithm
*/
    
namespace ne {

class Collider {
private:
    //Member data
    static constexpr int EPA_iterations = 32;
    static constexpr float EPA_epsilon = 1e-5f;
public:
    virtual ~Collider() = default;

    std::optional<sf::Vector2f> intersects(const Collider& other) const;

    virtual sf::Vector2f support(sf::Vector2f directional_vector) const = 0;
};

} //namespace ne
