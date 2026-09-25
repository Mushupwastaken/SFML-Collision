#include "Collision.hpp"


namespace ne {

constexpr sf::Vector2f Collider::getSupport(sf::Vector2f dirVec) const 
{
    return std::visit([&](const auto& body) -> sf::Vector2f
    {
        using T = std::decay_t<decltype(body)>;

        if constexpr(std::is_same_v<T, CollisionBody::Circle>) 
        {
            sf::Vector2f halfSize{body.radius, body.radius};

            if(dirVec.lengthSquared() == 0.0f)
            {
                return halfSize;
            }

            return halfSize + (dirVec.normalized() * body.radius);
        } 
        else if constexpr(std::is_same_v<T, CollisionBody::Rectangle>) 
        {
            sf::Vector2f halfSize = body.size / 2.0f;

            return halfSize + sf::Vector2f{std::copysign(halfSize.x, dirVec.x), std::copysign(halfSize.y, dirVec.y)};
        }
        else if constexpr(std::is_same_v<T, CollisionBody::Convex>)
        {
            sf::Vector2f furthestPoint{};
            float furthestDist = std::numeric_limits<float>::lowest();

            for(const sf::Vector2f& point : body.points)
            {
                float dist = point.dot(dirVec);
            
                if(dist > furthestDist)
                {
                    furthestDist = dist;
                    furthestPoint = point;
                }
            }
        
            return furthestPoint;
        }
        else
        {
            static_assert(false, "Collider missing method for getSupport");
        }
    }, body);
}

constexpr sf::FloatRect Collider::getBounds() const 
{
    return std::visit([&](const auto& body) -> sf::FloatRect
    {
        using T = std::decay_t<decltype(body)>;

        if constexpr(std::is_same_v<T, CollisionBody::Circle>) 
        {
            sf::Vector2f halfSize{body.radius, body.radius};
    
            return {sf::Vector2f{}, halfSize * 2.0f};
        } 
        else if constexpr(std::is_same_v<T, CollisionBody::Rectangle>) 
        {
            return {sf::Vector2f{}, body.size};
        }
        else if constexpr(std::is_same_v<T, CollisionBody::Convex>)
        {
            sf::Vector2f minPoint{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
            sf::Vector2f maxPoint{std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()};

            for(const sf::Vector2f& point : body.points) 
            {
                minPoint.x = std::min(minPoint.x, point.x);
                maxPoint.x = std::max(maxPoint.x, point.x);
            
                minPoint.y = std::min(minPoint.y, point.y);
                maxPoint.y = std::max(maxPoint.y, point.y);
            }

            return {minPoint, maxPoint - minPoint};
        }
        else
        {
            static_assert(false, "Collider missing method for getBounds");
        }
    }, body);
}

} //namespace ne