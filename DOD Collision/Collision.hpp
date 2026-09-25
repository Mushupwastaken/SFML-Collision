#pragma once


/*
    SOURCES:
    - https://dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/
    - https://winter.dev/articles/gjk-algorithm/
    - https://winter.dev/articles/epa-algorithm/
    - https://github.com/hamaluik/headbutt
*/


namespace ne {

namespace CollisionBody {
    
struct Circle {
    float radius{};
};

struct Rectangle {
    sf::Vector2f size{};
};

struct Convex {
    std::vector<sf::Vector2f> points{};
};

} //namespace CollisionBody

struct Collider {
    std::variant<CollisionBody::Circle, CollisionBody::Rectangle, CollisionBody::Convex> body;

    [[nodiscard]] constexpr sf::Vector2f getSupport(sf::Vector2f dirVec) const;
    
    [[nodiscard]] constexpr sf::FloatRect getBounds() const;
};

struct CollisionDetails {
    unsigned int gjkIterations{32};
    unsigned int epaIterations{64};
    float epaTolerance{1e-3};
};

class CollisionHandler {
public:
    explicit CollisionHandler(const CollisionDetails& details = {});

    void setDetails(const CollisionDetails& details);

    [[nodiscard]] const CollisionDetails& getDetails() const;

    [[nodiscard]] bool intersects(const Collider& colliderA, const sf::Transform& transformA, const Collider& colliderB, const sf::Transform& transformB);

    [[nodiscard]] std::optional<sf::Vector2f> findIntersection(const Collider& colliderA, const sf::Transform& transformA, const Collider& colliderB, const sf::Transform& transformB);
private:
    CollisionDetails m_details{};
    std::vector<sf::Vector2f> m_vertices{};
};

} //namespace ne


#include "Collision.inl"