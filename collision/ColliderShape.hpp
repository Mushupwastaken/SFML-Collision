#pragma once

#include "Collider.hpp"
#include <type_traits>

namespace ne {

template <typename T>
    requires std::is_base_of_v<sf::Shape, T>
class ColliderShape : public virtual Collider, public T { 
public:
    using T::T;

    sf::Vector2f support(sf::Vector2f directional_vector) const override {
        directional_vector = directional_vector.rotatedBy(-T::getRotation()).componentWiseMul(T::getScale());
        
        //Circle case
        if constexpr(std::is_same_v<T, sf::CircleShape>)
        {
            //30 is the default point count for sf::CircleShape
            if(T::getPointCount() >= 30)
            {
                sf::Vector2f unit_vector = directional_vector.normalized(); 
                return T::getTransform().transformPoint(T::getGeometricCenter() + T::getRadius() * unit_vector);
            }
        }

        //Convex set case
        sf::Vector2f farthest_point{};
        float farthest_dist = std::numeric_limits<float>::lowest();

        for(std::size_t i=0; i<T::getPointCount(); i++)
        {
            sf::Vector2f point = T::getPoint(i);
            float dist = point.dot(directional_vector);

            if(dist > farthest_dist)
            {
                farthest_dist = dist;
                farthest_point = point;
            }
        }

        return T::getTransform().transformPoint(farthest_point);
    }
};

//Shorthand for concrete SFML sf::Shape implementations
using RectangleColliderShape    =   ColliderShape<sf::RectangleShape>;
using CircleColliderShape       =   ColliderShape<sf::CircleShape>;
using ConvexColliderShape       =   ColliderShape<sf::ConvexShape>;


} //namespace ne
