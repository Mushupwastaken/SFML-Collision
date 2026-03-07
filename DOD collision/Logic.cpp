#include "Logic.hpp"
#include "Config.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>


namespace ne {

namespace detail {

using Simplex = std::vector<sf::Vector2f>;

[[nodiscard]] sf::Vector2f getMinkowskiDifference(const Collider& colliderA, const sf::Transformable& transformableA, const Collider& colliderB, const sf::Transformable& transformableB, sf::Vector2f directionalVector) {
    const sf::Vector2f localDirectionalVectorA = directionalVector.rotatedBy(-transformableA.getRotation());
    const sf::Vector2f localDirectionalVectorB = (-directionalVector).rotatedBy(-transformableB.getRotation());
    
    const sf::Vector2f globalSupportA = transformableA.getTransform().transformPoint(colliderA.getSupport(localDirectionalVectorA));
    const sf::Vector2f globalSupportB = transformableB.getTransform().transformPoint(colliderA.getSupport(localDirectionalVectorB));

    return globalSupportA - globalSupportB;
}

[[nodiscard]] bool evolveSimplex(Simplex& vertices, sf::Vector2f& directionalVector) {
    constexpr auto tripleProduct = [](sf::Vector2f a, sf::Vector2f b, sf::Vector2f c) -> sf::Vector2f {
        return (b * a.dot(c)) - (a * b.dot(c));
    };
    
    enum : std::size_t {
        Triangle    =   3,
        Line        =   2,
        Point       =   1,
    };
    
    switch(vertices.size()) 
    {
        case Triangle:
        { 
            sf::Vector2f a = vertices[2];
            sf::Vector2f b = vertices[1];
            sf::Vector2f c = vertices[0];
            
            sf::Vector2f ao =   - a;
            sf::Vector2f ab = b - a;
            sf::Vector2f ac = c - a;

            sf::Vector2f abPerp = tripleProduct(ac, ab, ab);
            sf::Vector2f acPerp = tripleProduct(ab, ac, ac);

            if(acPerp.dot(ao) > 0.0f) 
            {
                vertices = {c, a};
                directionalVector = acPerp;
                
                return false;
            }
            else if(abPerp.dot(ao) > 0.0f)
            {
                vertices = {b, a};
                directionalVector = abPerp;
                
                return false;
            }
            else
            {
                return true;
            }
        }
        case Line:
        {
            sf::Vector2f b = vertices[1];
            sf::Vector2f c = vertices[0];

            sf::Vector2f cb = b - c;
            sf::Vector2f co =   - c;

            directionalVector = tripleProduct(cb, co, cb);
            return false;
        }
        case Point:
        {
            directionalVector = -directionalVector;
            return false;
        }
        default:
            throw std::runtime_error("Invalid evolveSimplex switch case!");
    }
}

[[nodiscard]] std::optional<Simplex> GJK(const Collider& colliderA, const sf::Transformable& transformableA, const Collider& colliderB, const sf::Transformable& transformableB) {
    Simplex vertices{};
    sf::Vector2f directionalVector{1.0f, 0.0f};

    do
    {
        sf::Vector2f support = detail::getMinkowskiDifference(colliderA, transformableA, colliderB, transformableB, directionalVector);

        if(support.dot(directionalVector) <= 0.0f)
        {
            return std::nullopt;
        }

        vertices.push_back(support);
    }
    while(!detail::evolveSimplex(vertices, directionalVector));

    //GJK resolved with true
    return vertices;
}

[[nodiscard]] Manifold EPA(const Collider& colliderA, const sf::Transformable& transformableA, const Collider& colliderB, const sf::Transformable& transformableB, Simplex polytope) {    
    float closestDistance{};
    sf::Vector2f closestNormal{};
    std::size_t closestIndex{};

    //Sort polytope CCW
    if(polytope.size() == 3)
    {
        sf::Vector2f edge1 = polytope[1] - polytope[0];
        sf::Vector2f edge2 = polytope[2] - polytope[0];

        //Positive = CCW, Negative = CW
        if(edge1.cross(edge2) < 0.0f) 
        {
            std::swap(polytope[0], polytope[1]);
        }
    }

    //Find closest edge
    for(int iteration = 0; iteration < constants::epaIterations; iteration++)
    {
        //Find closest edge
        closestDistance = std::numeric_limits<float>::infinity();
        std::size_t n = polytope.size();

        for(std::size_t i=0; i<n; i++)
        {
            std::size_t j = (i + 1) % n;
            
            sf::Vector2f pointA = polytope[i];
            sf::Vector2f pointB = polytope[j];

            sf::Vector2f edge = pointB - pointA;
            sf::Vector2f normal = edge.perpendicular().normalized();
            
            float distance = normal.dot(pointA);
            
            if(distance < 0.0f)
            {
                distance    = -distance;
                normal      = -normal;
            }
            
            if(distance < closestDistance)
            {
                closestDistance    =   distance;
                closestNormal      =   normal;
                closestIndex       =   j;
            }
        }

        //Check if distance to origin is below tolerances, allows escape
        sf::Vector2f support = detail::getMinkowskiDifference(colliderA, transformableA, colliderB, transformableB, closestNormal);
        float distanceFromOrigin = closestNormal.dot(support);

        if(std::abs(distanceFromOrigin - closestDistance) <= constants::epaEpslion)
        {
            break;
        }

        polytope.insert(polytope.begin() + closestIndex, support);
    }

    // Penetration vector
    return {closestNormal, closestDistance + constants::epaEpslion};
}

} //namespace detail

//Public function API
bool intersects(const Collider& colliderA, const sf::Transformable& transformableA, const Collider& colliderB, const sf::Transformable& transformableB) {
    return detail::GJK(colliderA, transformableA, colliderB, transformableB).has_value();
}

std::optional<Manifold> findPenetration(const Collider& colliderA, const sf::Transformable& transformableA, const Collider& colliderB, const sf::Transformable& transformableB) {
    if(const auto& simplex = detail::GJK(colliderA, transformableA, colliderB, transformableB))
    {
        return detail::EPA(colliderA, transformableA, colliderB, transformableB, *simplex);
    }

    return std::nullopt;
}


} //namespace ne


