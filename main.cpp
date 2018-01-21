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

    std::cout << entity.AddComponent<Physics>(200, 100).get()->xSpeed << std::endl;
    std::cout << entity.AddComponent<Transform>(10, 20).get()->x << std::endl;

    std::cout << entity.GetComponent<Physics>().get()->ySpeed << std::endl;
    std::cout << entity.GetComponent<Transform>().get()->y << std::endl;

    return 0;
}