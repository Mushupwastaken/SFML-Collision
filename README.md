##SFML Collision
#Derived from sf::Shape, the inherited class, **ColliderShape** adds _intersects_ to a derived sf::Shape

The _intersects_ function returns a penetration vector, allowing us to properly resolve collision with the following C++ code:
```
std::optional penetration_vector = colliderA.intersects(colliderB);
if(penetration_vector != std::nullopt)
{
    colliderB.onCollision(colliderA, penetration_vector.value());
}
```
