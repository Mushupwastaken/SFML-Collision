#pragma once

#include <SFML/Graphics.hpp>
#include <optional>

//https://blog.hamaluik.ca/posts/building-a-collision-engine-part-1-2d-gjk-collision-detection/


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

    virtual void resolveCollision(Collider& collided, sf::Vector2f penetration_vector) = 0;
};


#include <memory>
inline std::vector<std::unique_ptr<Collider>> Colliders;


} //namespace ne
