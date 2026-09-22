# Simple SFML GJK & EPA Collision Support
__No credit required!__\
_Note:_  I'd recommended you use the OOP ver.  DOD ver. is ~6 months out of date.

# Sample C++ Code (class ver.):
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
//Initalizing
entt::registry registry;

auto entityA = registry.create();
collider.emplace<ne::Collider>(entityA, ne::shapes::Circle{50.0f});
collider.emplace<sf::Transformable>(entityA, sf::Transformable::Identity);

auto entityB = registry.create();
collider.emplace<ne::Collider>(entityB, ne::shapes::Circle{50.0f});
collider.emplace<sf::Transformable>(entityB, sf::Transformable::Identity);

//Collision code (NOTE: using a view is recommeneded)
auto* colliderA			=   registry.try_get<ne::Collider>(entityA);
auto* transformableA    =   registry.try_get<sf::Transformable>(entityA);
auto* colliderB         =   registry.try_get<ne::Collider>(entityB);
auto* transformableB    =   registry.try_get<sf::Transformable>(entityB);

if(!colliderA || !transformableA || !colliderB || !transformableB)
{
    std::cerr << "Failed to gather necessary Components!\n";
}
else
{
    if(const std::optional manifold = ne::findPenetration(*colliderA, *transformableA, *colliderB, *transformableB))
    {
        transformableA->move(manifold->normal * -manifold->depth);
    }
}
```
