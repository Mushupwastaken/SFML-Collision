#include "Collider.hpp"

#include <vector>
#include <cmath>
#include <stdexcept>


namespace ne {

using Simplex = std::vector<sf::Vector2f>;

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

            if(acPerp.dot(ao) >= 0) {
                vertices = {c, a};
                directional_vector = acPerp;
                
                return false;
            }
            else if(abPerp.dot(ao) >= 0)
            {
                vertices = {b, a};
                directional_vector = abPerp;
                
                return false;
            }
            
            return true;
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

} //unnamed namespace


//Public
std::optional<sf::Vector2f> Collider::intersects(const Collider& other) const {
    //GJK
    Simplex vertices{};
    sf::Vector2f directional_vector{1.f, 0.f}; //(CenterA - CenterB) would be most effecient

    do
    {
        sf::Vector2f support = this->support(directional_vector) - other.support(-directional_vector);

        if(support.dot(directional_vector) <= 0)
        {
            return std::nullopt;
        }

        vertices.push_back(support);
    }
    while(!evolveSimplex(vertices, directional_vector));

    //EPA
    float closest_dist{}, dist{};
    sf::Vector2f closest_normal{};
    std::size_t closest_index{};

    //Find closest edge
    for(int iteration = 0; iteration < EPA_iterations; iteration++)
    {
        closest_dist = std::numeric_limits<float>::infinity();

        for(std::size_t i=0; i<vertices.size(); i++)
        {
            std::size_t j = (i + 1) % vertices.size();
            
            sf::Vector2f edge = vertices[j] - vertices[i];
            sf::Vector2f normal = edge.perpendicular().normalized();
            
            dist = normal.dot(vertices[i]);

            if(dist < 0)
            {
                dist = -dist;
                normal = -normal;
            }

            if(dist < closest_dist) 
            {
                closest_dist = dist;
                closest_normal = normal;
                closest_index = j;
            }
        }

        sf::Vector2f support = this->support(closest_normal) - other.support(-closest_normal);
        dist = closest_normal.dot(support);

        if(std::abs(dist - closest_dist) <= EPA_epsilon)
        {
            break;
        }

        vertices.insert(std::begin(vertices) + closest_index, support);
    }

    //returns penetration vector
    return closest_dist * closest_normal;
}


} //namespace ne
