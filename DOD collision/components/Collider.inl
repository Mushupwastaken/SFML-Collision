namespace ne {

constexpr sf::Vector2f Collider::getSupport(sf::Vector2f directionalVector) const {
    auto calculateSupport = [&](const auto& shape) -> sf::Vector2f {
        using T = std::decay_t<decltype(shape)>;

        if constexpr(std::is_same_v<T, Convex>)
        {
            sf::Vector2f furthestPoint{};
            float furthestDistance = std::numeric_limits<float>::lowest();

            for(sf::Vector2f point : shape.points)
            {
                float distance = point.dot(directionalVector);

                if(distance > furthestDistance)
                {
                    furthestDistance = distance;
                    furthestPoint = point;
                }
            }

            return furthestPoint;
        }
        else if constexpr(std::is_same_v<T, Rectangle>)
        {
            //Match SFML -- Top left corner
            sf::Vector2f halfSize = shape.size / 2.f;
            
            return sf::Vector2f(
                (directionalVector.x >= 0.0f) ? halfSize.x : -halfSize.x,
                (directionalVector.y >= 0.0f) ? halfSize.y : -halfSize.y
            );
        }
        else if constexpr(std::is_same_v<T, Circle>)
        {
            //Match SFML -- Top left corner   
            sf::Vector2f center{shape.radius, shape.radius};

            return center + shape.radius * directionalVector.normalized();
        }
        else
        {
            throw std::runtime_error("findlocalSupport variant unsupported!");
        }
    };

    return std::visit(calculateSupport, shape);
}

constexpr sf::FloatRect Collider::getBounds() const {
    auto calculateBounds = [&](const auto& shape) -> sf::FloatRect {
        using T = std::decay_t<decltype(shape)>;

        if constexpr(std::is_same_v<T, Convex>)
        {
            sf::Vector2f minPoint = std::numeric_limits<sf::Vector2f>::max();
            sf::Vector2f maxPoint = std::numeric_limits<sf::Vector2f>::lowest();

            for(auto point : shape.points) 
            {
                if(point.x < minPoint.x)
                {
                    minPoint.x = point.x;
                }
                else if(point.x > maxPoint.x) 
                {
                    maxPoint.x = point.x;
                }

                if(point.y < minPoint.y)
                {
                    minPoint.y = point.y;
                }
                else if(point.y > maxPoint.y) 
                {
                    maxPoint.y = point.y;
                }
            }
        
            return sf::FloatRect(minPoint, maxPoint - minPoint);
        }
        else if constexpr(std::is_same_v<T, Rectangle>)
        {
            return sf::FloatRect({0, 0}, shape.size);
        }
        else if constexpr(std::is_same_v<T, Circle>)
        {
            const float diameter = shape.radius * 2;
            return sf::FloatRect({0, 0}, {diameter, diameter});
        }
        else
        {
            throw std::runtime_error("getBounds variant unsupported!");
        }
    };

    return std::visit(calculateBounds, shape);
}

} //namespace ne