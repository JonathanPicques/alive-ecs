#include <gtest/gtest.h>

#include <core/entitymanager.hpp>

#include "test_systems/systems.hpp"

TEST(System, AddSystem)
{
    EntityManager manager;
    char inputState = 'i';
    char networkState = 'n';
    auto world = manager.AddSystem<WorldStateSystem>(&inputState, &networkState);
    EXPECT_EQ(world->mInputState, &inputState);
    EXPECT_EQ(world->mNetworkState, &networkState);
    EXPECT_EQ(world, manager.GetSystem<WorldStateSystem>());

    EXPECT_ANY_THROW((manager.AddSystem<WorldStateSystem>(nullptr, nullptr)));
    EXPECT_ANY_THROW((manager.AddSystem<WorldStateSystem>(nullptr, nullptr)));
}

TEST(System, RemoveSystem)
{
    EntityManager manager;
    EXPECT_ANY_THROW((manager.RemoveSystem<WorldStateSystem>()));
    EXPECT_ANY_THROW((manager.RemoveSystem<WorldStateSystem>()));
    manager.AddSystem<WorldStateSystem>(nullptr, nullptr);
    EXPECT_NE(nullptr, manager.GetSystem<WorldStateSystem>());
    manager.RemoveSystem<WorldStateSystem>();
    EXPECT_ANY_THROW((manager.RemoveSystem<WorldStateSystem>()));
    EXPECT_ANY_THROW((manager.RemoveSystem<WorldStateSystem>()));
}

TEST(System, Lifecycle)
{
    EntityManager manager;
    auto gridMapSystem = manager.AddSystem<GridMapSystem>();
    auto worldStateSystem = manager.AddSystem<WorldStateSystem>(nullptr, nullptr);
    EXPECT_EQ(nullptr, gridMapSystem->mWorldStateSystem);
    manager.ResolveSystemDependencies();
    EXPECT_EQ(worldStateSystem, gridMapSystem->mWorldStateSystem);
}