#include <gtest/gtest.h>

#include <core/entitymanager.hpp>

#include "test_components/components.hpp"

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

TEST(Components, AddComponent)
{
    EntityManager manager;
    auto entity = manager.Create();
    auto component = entity->AddComponent<TransformComponent>(32.0f, 64.0f);
    EXPECT_EQ(component->GetX(), 32.0f);
    EXPECT_EQ(component->GetY(), 64.0f);
}

TEST(Components, GetComponent)
{
    EntityManager manager;
    auto entity = manager.Create();
    ASSERT_EQ(entity->GetComponent<TransformComponent>(), nullptr);

    auto component1 = entity->AddComponent<TransformComponent>(32.0f, 64.0f);
    auto component2 = entity->GetComponent<TransformComponent>();
    ASSERT_EQ(component1, component2);
    EXPECT_EQ(component2->GetX(), 32.0f);
    EXPECT_EQ(component2->GetY(), 64.0f);

    entity->AddComponent<TransformComponent>(1.0f, 0.0f);
    entity->AddComponent<TransformComponent>(2.0f, 0.0f);
    entity->AddComponent<TransformComponent>(3.0f, 0.0f);
    entity->AddComponent<TransformComponent>(4.0f, 0.0f);
    ASSERT_EQ(entity->GetComponent<TransformComponent>()->GetX(), 4.0f);
}

TEST(Components, RemoveComponent)
{
    EntityManager manager;
    auto entity1 = manager.Create();
    auto entity2 = manager.Create();
    auto component = entity1->AddComponent<TransformComponent>(32.0f, 64.0f);
    ASSERT_EQ(entity1->GetComponent<TransformComponent>(), component);
    ASSERT_EQ(entity2->GetComponent<TransformComponent>(), nullptr);
    entity1->RemoveComponent<TransformComponent>();
    ASSERT_EQ(entity1->GetComponent<TransformComponent>(), nullptr);
    entity1->RemoveComponent<TransformComponent>();
    entity1->RemoveComponent<TransformComponent>();
    entity1->RemoveComponent<TransformComponent>();
    entity1->RemoveComponent<TransformComponent>();
    entity1->RemoveComponent<TransformComponent>();
    ASSERT_EQ(entity1->GetComponent<TransformComponent>(), nullptr);
    entity1->AddComponent<TransformComponent>(32.0f, 64.0f);
    ASSERT_NE(entity1->GetComponent<TransformComponent>(), nullptr);
    entity1->RemoveComponent<DummyComponent>();
    ASSERT_NE(entity1->GetComponent<TransformComponent>(), nullptr);
    entity1->RemoveComponent<TransformComponent>();
    ASSERT_EQ(entity1->GetComponent<TransformComponent>(), nullptr);
}

TEST(Components, Has_Component)
{
    EntityManager manager;
    auto entity = manager.Create();
    EXPECT_FALSE(entity->HasComponent<DummyComponent>());
    EXPECT_FALSE(entity->HasComponent<TransformComponent>());
    EXPECT_FALSE((entity->HasComponent<DummyComponent, TransformComponent>()));
    EXPECT_FALSE((entity->HasAnyComponent<DummyComponent, TransformComponent>()));

    entity->AddComponent<DummyComponent>();
    EXPECT_TRUE(entity->HasComponent<DummyComponent>());
    EXPECT_FALSE((entity->HasComponent<DummyComponent, TransformComponent>()));
    EXPECT_TRUE((entity->HasAnyComponent<DummyComponent, TransformComponent>()));

    entity->AddComponent<TransformComponent>(32.0f, 64.0f);
    EXPECT_TRUE((entity->HasComponent<DummyComponent, TransformComponent>()));
    EXPECT_TRUE((entity->HasComponent<TransformComponent, DummyComponent>()));
    EXPECT_TRUE((entity->HasAnyComponent<DummyComponent, TransformComponent>()));
    EXPECT_TRUE((entity->HasAnyComponent<TransformComponent, DummyComponent>()));
    EXPECT_TRUE((entity->HasComponent<TransformComponent, TransformComponent, TransformComponent>()));
    EXPECT_TRUE((entity->HasComponent<TransformComponent, DummyComponent, TransformComponent>()));
    EXPECT_TRUE((entity->HasAnyComponent<TransformComponent, TransformComponent, TransformComponent>()));

    entity->RemoveComponent<DummyComponent>();
    entity->RemoveComponent<TransformComponent>();
    EXPECT_FALSE((entity->HasAnyComponent<TransformComponent, DummyComponent>()));
    EXPECT_FALSE((entity->HasAnyComponent<DummyComponent, TransformComponent>()));
}