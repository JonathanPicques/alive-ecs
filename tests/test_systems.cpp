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

    const auto* const world2 =  manager.GetSystem<WorldStateSystem>();
    // world2->Update();
}