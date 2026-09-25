#include "Collision.hpp"


namespace ne {

namespace priv {

[[nodiscard]] constexpr sf::Vector2f tripleProduct(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c) 
{
    return b * a.dot(c) - a * b.dot(c);
}

[[nodiscard]] bool evolveSimplex(std::vector<sf::Vector2f>& vertices, sf::Vector2f& dirVec) 
{
    switch(vertices.size()) 
    {
        case 3:
        { 
            sf::Vector2f a = vertices[2];
            sf::Vector2f b = vertices[1];
            sf::Vector2f c = vertices[0];
            
            sf::Vector2f ao = -a;
            sf::Vector2f ab = b - a;
            sf::Vector2f ac = c - a;

            sf::Vector2f abPerp = tripleProduct(ac, ab, ab);
            sf::Vector2f acPerp = tripleProduct(ab, ac, ac);

            if(acPerp.dot(ao) > 0.0f) 
            {
                vertices = {c, a};
                dirVec = acPerp;
            }
            else if(abPerp.dot(ao) > 0.0f)
            {
                vertices = {b, a};
                dirVec = abPerp;
            }
            else
            {
                return true;
            }

            return false;
        }
        case 2:
        {
            sf::Vector2f a = vertices[1];
            sf::Vector2f b = vertices[0];

            sf::Vector2f ab = b - a;
            sf::Vector2f ao = -a;

            dirVec = tripleProduct(ab, ao, ab);
            
            if(dirVec.lengthSquared() == 0.0f) 
            {
                dirVec = ab.perpendicular();
            }

            return false;
        }
        case 1:
        {
            dirVec = -vertices[0];
            
            return false;
        }
        default:
            throw std::runtime_error("Invalid evolveSimplex case!");
    }
}

[[nodiscard]] sf::Vector2f getMinkowskiDifference(const Collider& colliderA, const sf::Transform& transformA, const Collider& colliderB, const sf::Transform& transformB, sf::Vector2f dirVec) 
{
    //Ensure dirVec is normalized before calculating
    if(dirVec.lengthSquared() > 1.0f)
    {
        dirVec = dirVec.normalized();
    }
   
    const float* matA = transformA.getMatrix();
    const float* matB = transformB.getMatrix();
    
    //Orient dirVec to the dir & scale of  inverse transform
    sf::Vector2f localDirA{dirVec.x * matA[0] + dirVec.y * matA[1], dirVec.x * matA[4] + dirVec.y * matA[5]};
    sf::Vector2f localDirB{-dirVec.x * matB[0] - dirVec.y * matB[1], -dirVec.x * matB[4] - dirVec.y * matB[5]};
    
    sf::Vector2f supportA = transformA.transformPoint(colliderA.getSupport(localDirA));
    sf::Vector2f supportB = transformB.transformPoint(colliderB.getSupport(localDirB));

    return supportA - supportB;
}

} //namespace priv

CollisionHandler::CollisionHandler(const CollisionDetails& details)
{
    //Func called because it invokes m_vertices.reserve(...) internally
    setDetails(details);
}

void CollisionHandler::setDetails(const CollisionDetails& details)
{
    m_details = details;
    m_vertices.reserve(3 + m_details.epaIterations);
}

const CollisionDetails& CollisionHandler::getDetails() const
{
    return m_details;
}
    
bool CollisionHandler::intersects(const Collider& colliderA, const sf::Transform& transformA, const Collider& colliderB, const sf::Transform& transformB)
{
    m_vertices.clear();
    
    sf::FloatRect boundsA = transformA.transformRect(colliderA.getBounds());
    sf::FloatRect boundsB = transformB.transformRect(colliderB.getBounds());
    
    if(!boundsA.findIntersection(boundsB))
    {
        return false;
    }

    //Check for collision using GJK, filling m_vertices for EPA
    sf::Vector2f dirVec{1.0f, 0.0f};

    for(unsigned int i = 0; i < m_details.gjkIterations; i++)
    {
        sf::Vector2f support = priv::getMinkowskiDifference(colliderA, transformA, colliderB, transformB, dirVec);

        if(support.dot(dirVec) < 0.0f)
        {
            break;
        }

        m_vertices.push_back(support);

        if(priv::evolveSimplex(m_vertices, dirVec))
        {
            return true;
        }
    }

    return false;
}

std::optional<sf::Vector2f> CollisionHandler::findIntersection(const Collider& colliderA, const sf::Transform& transformA, const Collider& colliderB, const sf::Transform& transformB)
{
    if(!intersects(colliderA, transformA, colliderB, transformB))
    {
        return std::nullopt;
    }

    //Derive resolution vec using EPA
    sf::Vector2f edgeA = m_vertices[1] - m_vertices[0];
    sf::Vector2f edgeB = m_vertices[2] - m_vertices[0];

    //Sort polytope CCW
    if(edgeA.cross(edgeB) < 0.0f) 
    {
        std::swap(m_vertices[1], m_vertices[2]);
    }

    float closestDist{};
    sf::Vector2f closestNormal{};
    std::size_t closestIdx{};

    for(unsigned int i = 0; i < m_details.epaIterations; i++)
    {
        std::size_t vertexCount = m_vertices.size();
        closestDist = std::numeric_limits<float>::infinity();
        
        for(std::size_t j = 0; j < vertexCount; j++)
        {
            std::size_t k = (j + 1) % vertexCount;

            const sf::Vector2f& pointA = m_vertices[j];
            const sf::Vector2f& pointB = m_vertices[k];
            
            sf::Vector2f edge = pointA - pointB;
            sf::Vector2f normal{1.0f, 0.0f};
            
            if(edge.lengthSquared() > 0.0f)  
            {
                normal = edge.perpendicular().normalized();
            }

            float dist = normal.dot(pointA);
            
            if(dist < closestDist)
            {
                closestDist = dist;
                closestNormal = normal;
                closestIdx = k;
            }
        }
            
        sf::Vector2f support = priv::getMinkowskiDifference(colliderA, transformA, colliderB, transformB, closestNormal);
        float dist = closestNormal.dot(support);

        if(std::abs(dist - closestDist) <= m_details.epaTolerance)
        {
            break;
        }

        //Insert CCW to maintain sorted state
        m_vertices.insert(m_vertices.begin() + closestIdx, support);
    }

    return closestNormal * closestDist;
}

} //namespace ne