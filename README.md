# Simple SFML GJK & EPA Collision Support
__No credit required!__

# Sample C++ Code (OOP ver.):
```cpp
ne::CollisionHandler handler{};

ne::CircleCollider colliderA(50.0f);
sf::Transformable transformableA{};
ne::CircleCollider colliderB(50.0f);
sf::Transformable transformableB{};

if(const std::optional resolutionVec = handler.findIntersection(colliderA, transformableA.getTransform(), colliderB, transformableB.getTransform()))
{
    transformableA.move(-*resolutionVec);
}
```
***
# Sample C++ Code (DOD ver.):
```cpp
ne::CollisionHandler handler{};

ne::Collider colliderA{ne::CollisionBody::Circle{50.0f}};
sf::Transformable transformableA{};
ne::Collider colliderB{ne::CollisionBody::Circle{50.0f};
sf::Transformable transformableB{};

if(const std::optional resolutionVec = handler.findIntersection(colliderA, transformableA.getTransform(), colliderB, transformableB.getTransform()))
{
    transformableA.move(-*resolutionVec);
}
```
