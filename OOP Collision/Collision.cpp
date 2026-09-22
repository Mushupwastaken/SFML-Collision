#include "Collision.hpp"


namespace ne {

namespace detail {

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

} //namespace detail

sf::FloatRect Collider::getBounds() const 
{
    //QOL fallback for quick testing purposes - should be overloaded
    sf::Vector2f minBound{getSupport({-1.0f, 0.0f}).x, getSupport({0.0f, 1.0f}).y};
    sf::Vector2f maxBound{getSupport({1.0f, 0.0f}).x, getSupport({0.0f, -1.0f}).y};

    return sf::FloatRect{minBound, maxBound - minBound};
}


CircleCollider::CircleCollider(float radius) : 
    m_radius(radius)
{}

float CircleCollider::getRadius() const
{
    return m_radius;
}

sf::Vector2f CircleCollider::getSupport(sf::Vector2f dirVec) const
{
    sf::Vector2f halfSize{m_radius, m_radius};

    if(dirVec.lengthSquared() == 0.0f)
    {
        return halfSize;
    }

    return halfSize + (dirVec.normalized() * m_radius);
}

sf::FloatRect CircleCollider::getBounds() const
{
    sf::Vector2f halfSize{m_radius, m_radius};
    
    return {sf::Vector2f{}, halfSize * 2.0f};
}


RectangleCollider::RectangleCollider(sf::Vector2f size) :
    m_size(size)
{}

sf::Vector2f RectangleCollider::getSize() const
{
    return m_size;
}

sf::Vector2f RectangleCollider::getSupport(sf::Vector2f dirVec) const
{
    sf::Vector2f halfSize = m_size / 2.0f;

    return halfSize + sf::Vector2f{std::copysign(halfSize.x, dirVec.x), std::copysign(halfSize.y, dirVec.y)};
}

sf::FloatRect RectangleCollider::getBounds() const
{
    return {sf::Vector2f{}, m_size};
}


ConvexCollider::ConvexCollider(std::initializer_list<sf::Vector2f> points) :
    m_points(std::move(points))
{
    if(m_points.size() < 3) 
    {
        throw std::invalid_argument("A ConvexCollider must have at minimum 3 points");
    }

    updateBounds();
}

std::span<const sf::Vector2f> ConvexCollider::getPoints() const
{
    return m_points;
}

sf::Vector2f ConvexCollider::getSupport(sf::Vector2f dirVec) const
{
    sf::Vector2f furthestPoint{};
    float furthestDist = std::numeric_limits<float>::lowest();

    for(const sf::Vector2f& point : m_points)
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

sf::FloatRect ConvexCollider::getBounds() const 
{
    return m_bounds;
}

void ConvexCollider::updateBounds() 
{
    sf::Vector2f minPoint{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
    sf::Vector2f maxPoint{std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()};

    for(const sf::Vector2f& point : m_points) 
    {
        if(point.x < minPoint.x)
        {
            minPoint.x = point.x;
        }
        if(point.x > maxPoint.x) 
        {
            maxPoint.x = point.x;
        }

        if(point.y < minPoint.y)
        {
            minPoint.y = point.y;
        }
        if(point.y > maxPoint.y) 
        {
            maxPoint.y = point.y;
        }
    }
    
    m_bounds.position = minPoint;
    m_bounds.size = maxPoint - minPoint;
}


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
        sf::Vector2f support = detail::getMinkowskiDifference(colliderA, transformA, colliderB, transformB, dirVec);

        if(support.dot(dirVec) < 0.0f)
        {
            break;
        }

        m_vertices.push_back(support);

        if(detail::evolveSimplex(m_vertices, dirVec))
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
            
        sf::Vector2f support = detail::getMinkowskiDifference(colliderA, transformA, colliderB, transformB, closestNormal);
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