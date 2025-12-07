##SFML Collision
#Derived from sf::Shape, this adds _intersects_ to sf::Shapes, 

The _intersects_ function returns a penetration vector, allowing us to properly resolve collision with the following:

```
std::optional penetration_vector = colliderA.intersects(colliderB);
if(penetration_vector != std::nullopt)
{
    colliderB.onCollision(colliderA, penetration_vector.value());
}
```
