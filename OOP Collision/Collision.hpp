#pragma once


/*
    SOURCES:
    - https://dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/
    - https://winter.dev/articles/gjk-algorithm/
    - https://winter.dev/articles/epa-algorithm/
    - https://github.com/hamaluik/headbutt
*/


namespace ne {

struct CollisionDetails {
    unsigned int gjkIterations{32};
    unsigned int epaIterations{64};
    float epaTolerance{1e-3};
};

class Collider {
public:
    virtual ~Collider() = default;

    [[nodiscard]] virtual sf::Vector2f getSupport(sf::Vector2f dirVec) const = 0;

    [[nodiscard]] virtual sf::FloatRect getBounds() const;
};

class CircleCollider final : public Collider {
public:
    explicit CircleCollider(float radius);

    [[nodiscard]] float getRadius() const;

    [[nodiscard]] sf::Vector2f getSupport(sf::Vector2f dirVec) const override;

    [[nodiscard]] sf::FloatRect getBounds() const override;
private:
    float m_radius{};
};

class RectangleCollider final : public Collider {
public:
    explicit RectangleCollider(sf::Vector2f size);

    [[nodiscard]] sf::Vector2f getSize() const;

    [[nodiscard]] sf::Vector2f getSupport(sf::Vector2f dirVec) const override;

    [[nodiscard]] sf::FloatRect getBounds() const override;
private:
    sf::Vector2f m_size{};
};

class ConvexCollider final : public Collider {
public:
    explicit ConvexCollider(std::initializer_list<sf::Vector2f> points);

    [[nodiscard]] std::span<const sf::Vector2f> getPoints() const;

    [[nodiscard]] sf::Vector2f getSupport(sf::Vector2f dirVec) const override;

    [[nodiscard]] sf::FloatRect getBounds() const override;
private:
    void updateBounds();

    std::vector<sf::Vector2f> m_points{};
    sf::FloatRect m_bounds{};
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