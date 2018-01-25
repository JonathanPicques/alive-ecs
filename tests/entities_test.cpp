#include <gtest/gtest.h>

#include <core/entitymanager.hpp>

TEST(Entities, Create_Destroy_DestroyEntities)
{
    EntityManager manager;
    auto entity = manager.Create();
    ASSERT_NE(entity, nullptr);
    ASSERT_FALSE(entity->IsDestroyed());
    entity->Destroy();
    ASSERT_TRUE(entity->IsDestroyed());
    manager.DestroyEntities();

    entity = manager.Create();
    ASSERT_NE(entity, nullptr);
    ASSERT_FALSE(entity->IsDestroyed());
    entity->Destroy();
    ASSERT_TRUE(entity->IsDestroyed());
    manager.DestroyEntities();
}