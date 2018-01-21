#include <iostream>

#include "Entity.hpp"

struct Transform
{
    explicit Transform(int x, int y) : x(x), y(y)
    {}
    int x, y;
};

struct Physics
{
    explicit Physics(int x, int y) : xSpeed(x), ySpeed(y)
    {}
    int xSpeed, ySpeed;
};

int main()
{
    EntityManager manager;
    Entity entity = manager.Create();

    auto component = entity.AddComponent<Physics>(199, 199);
    auto component2 = entity.AddComponent<Transform>(10, 20);

    std::cout << component.get()->xSpeed << std::endl;
    std::cout << component2.get()->x << std::endl;
    return 0;
}