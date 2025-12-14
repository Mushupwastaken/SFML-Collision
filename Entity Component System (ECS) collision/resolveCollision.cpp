#include "resolveCollision.hpp"

#include <type_traits>
#include <limits>
#include <cmath>
#include <stdexcept>



namespace ne {

using Simplex = std::vector<sf::Vector2f>;
static constexpr int EPA_iterations = 32;
static constexpr float EPA_epsilon = 1e-5f;

namespace { 

constexpr sf::Vector2f tripleProduct(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c) {
    return (b * a.dot(c)) - (a * b.dot(c));
};

bool evolveSimplex(Simplex& vertices, sf::Vector2f& directional_vector) {
    enum : std::size_t 
    {
        Point       =   1,
        Line        =   2,
        Triangle    =   3,
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

            if(acPerp.dot(ao) > 0) 
            {
                vertices = {c, a};
                directional_vector = acPerp;
                
                return false;
            }
            else if(abPerp.dot(ao) > 0)
            {
                vertices = {b, a};
                directional_vector = abPerp;
                
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

            directional_vector = tripleProduct(cb, co, cb);
            return false;
        }
        case Point:
        {
            directional_vector = -directional_vector;
            return false;
        }
        default:
            throw std::runtime_error("Invalid evolveSimplex switch case!");
    }
}

sf::Vector2f getSupportPoint(ColliderComponent collider, TransformableComponent transformable, sf::Vector2f directional_vector) {
    directional_vector = directional_vector.rotatedBy(-transformable.getRotation()).componentWiseMul(transformable.getScale());
    
    auto find_local_support = [&](const auto& shape) -> sf::Vector2f 
    {
        using T = std::decay_t<decltype(shape)>;

        if constexpr(std::is_same_v<T, Convex>)
        {
            sf::Vector2f farthest_point{};
            float farthest_dist = std::numeric_limits<float>::lowest();

            for(auto point : shape.points)
            {
                float dist = point.dot(directional_vector);

                if(dist > farthest_dist)
                {
                    farthest_dist = dist;
                    farthest_point = point;
                }
            }

            return farthest_point;
        }
        else if constexpr(std::is_same_v<T, Circle>)
        {
            sf::Vector2f center{shape.radius/2.f, shape.radius/2.f};
            
            return center + shape.radius * directional_vector.normalized();
        }
        else
        {
            throw std::runtime_error("ColliderComponent missing getSupportPoint variant impl!");
        }
    };
    
    sf::Vector2f local_support = std::visit(find_local_support, collider);
    return transformable.getTransform().transformPoint(local_support);
}

} //unnamed namespace

std::optional<sf::Vector2f> resolveCollision(ColliderComponent collider_a, TransformableComponent transformable_a, ColliderComponent collider_b, TransformableComponent transformable_b) {
    //Helper function
    auto minkowskiSupport = [&](sf::Vector2f directional_vector) -> sf::Vector2f 
    {
        return getSupportPoint(collider_a, transformable_a, directional_vector) - getSupportPoint(collider_b, transformable_b, -directional_vector);
    };
    
    //GJK phase
    Simplex         vertices{};
    sf::Vector2f    directional_vector{1.f, 0.f};

    do
    {
        sf::Vector2f support = minkowskiSupport(directional_vector);

        if(support.dot(directional_vector) <= 0)
        {
            return std::nullopt;
        }

        vertices.push_back(support);
    }
    while(!evolveSimplex(vertices, directional_vector));


    //EPA phase
    float           closest_distance{};
    sf::Vector2f    closest_normal{};
    std::size_t     closest_index{};

    //Find closest edge
    for(int iteration = 0; iteration < EPA_iterations; iteration++)
    {
        //Find closest edge
        closest_distance = std::numeric_limits<float>::infinity();

        for(std::size_t i=0; i<vertices.size(); i++)
        {
            std::size_t j = (i + 1) % vertices.size();
            
            sf::Vector2f edge       =   vertices[j] - vertices[i];
            sf::Vector2f normal     =   edge.perpendicular().normalized();
            
            float distance = normal.dot(vertices[i]);

            if(distance < 0)
            {
                distance    =   -distance;
                normal      =   -normal;
            }

            if(distance < closest_distance) 
            {
                closest_distance    =   distance;
                closest_normal      =   normal;
                closest_index       =   j;
            }
        }

        //Check if distance to origin is below tolerances, allows escape
        sf::Vector2f support        =   minkowskiSupport(closest_normal);
        float distance_from_origin  =   closest_normal.dot(support);

        if(std::abs(distance_from_origin - closest_distance) <= EPA_epsilon)
        {
            break;
        }

        vertices.insert(std::begin(vertices) + closest_index, support);
    }

    //returns penetration vector
    return closest_distance * closest_normal;
}

} //namespace ne