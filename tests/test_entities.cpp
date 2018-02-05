#include <gtest/gtest.h>

#include <core/entitymanager.hpp>

#include "test_components/components.hpp"

TEST(Entities, CreateDestroyEntities)
{
    auto manager = CreateEntityManager();
    auto entity = manager->CreateEntity();
    EXPECT_FALSE(entity.IsDestroyed());
    entity.Destroy();
    EXPECT_TRUE(entity.IsDestroyed());
    EXPECT_ANY_THROW(entity.Destroy());

    auto entity2 = manager->CreateEntity();
    EXPECT_FALSE(entity2.IsDestroyed());
    entity2.Destroy();
    EXPECT_TRUE(entity2.IsDestroyed());

    auto entity3 = manager->CreateEntity();
    auto entity4 = manager->CreateEntity();
    auto entity5 = manager->CreateEntity();
    EXPECT_FALSE(entity3.IsDestroyed());
    EXPECT_FALSE(entity4.IsDestroyed());
    EXPECT_FALSE(entity5.IsDestroyed());
    EXPECT_TRUE(entity2.IsDestroyed());

    EXPECT_ANY_THROW(entity.Destroy());
}

TEST(Components, AddComponent)
{
    auto manager = CreateEntityManager();
    auto entity = manager->CreateEntity();
    auto component = entity.AddComponent<TransformComponent>();
    EXPECT_EQ(component->GetX(), 0.0f);
    EXPECT_EQ(component->GetY(), 0.0f);
    entity.Destroy();
    EXPECT_ANY_THROW(entity.GetComponent<TransformComponent>());
}

TEST(Components, GetComponent)
{
    auto manager = CreateEntityManager();
    auto entity = manager->CreateEntity();
    EXPECT_EQ(entity.GetComponent<TransformComponent>(), nullptr);

    auto component1 = entity.AddComponent<TransformComponent>();
    auto component2 = entity.GetComponent<TransformComponent>();

    component1->mData.x = 32.0f;
    component1->mData.y = 64.0f;

    EXPECT_EQ(component1, component2);
    EXPECT_EQ(component2->GetX(), 32.0f);
    EXPECT_EQ(component2->GetY(), 64.0f);

    EXPECT_ANY_THROW((entity.AddComponent<TransformComponent>()));
    EXPECT_ANY_THROW((entity.AddComponent<TransformComponent>()));
    EXPECT_ANY_THROW((entity.AddComponent<TransformComponent>()));
    EXPECT_ANY_THROW((entity.AddComponent<TransformComponent>()));

    EXPECT_EQ(entity.GetComponent<TransformComponent>()->GetX(), 32.0f);
    EXPECT_EQ(entity.GetComponent<TransformComponent>()->GetY(), 64.0f);
}

TEST(Components, RemoveComponent)
{
    auto manager = CreateEntityManager();
    auto entity1 = manager->CreateEntity();
    auto entity2 = manager->CreateEntity();
    auto component = entity1.AddComponent<TransformComponent>();
    EXPECT_EQ(entity1.GetComponent<TransformComponent>(), component);
    EXPECT_EQ(entity2.GetComponent<TransformComponent>(), nullptr);

    entity1.RemoveComponent<TransformComponent>();
    EXPECT_EQ(entity1.GetComponent<TransformComponent>(), nullptr);

    EXPECT_ANY_THROW(entity1.RemoveComponent<TransformComponent>());
    EXPECT_EQ(entity1.GetComponent<TransformComponent>(), nullptr);

    entity1.AddComponent<TransformComponent>();
    ASSERT_NE(entity1.GetComponent<TransformComponent>(), nullptr);

    EXPECT_ANY_THROW(entity1.RemoveComponent<DummyComponent>());
    EXPECT_EQ(entity1.GetComponent<DummyComponent>(), nullptr);

    entity1.RemoveComponent<TransformComponent>();
    EXPECT_EQ(entity1.GetComponent<TransformComponent>(), nullptr);
}

TEST(Components, Has_Component)
{
    auto manager = CreateEntityManager();
    auto entity = manager->CreateEntity();
    EXPECT_FALSE(entity.HasComponent<DummyComponent>());
    EXPECT_FALSE(entity.HasComponent<TransformComponent>());
    EXPECT_FALSE((entity.HasComponent<DummyComponent, TransformComponent>()));
    EXPECT_FALSE((entity.HasAnyComponent<DummyComponent, TransformComponent>()));

    entity.AddComponent<DummyComponent>();
    EXPECT_TRUE(entity.HasComponent<DummyComponent>());
    EXPECT_FALSE((entity.HasComponent<DummyComponent, TransformComponent>()));
    EXPECT_TRUE((entity.HasAnyComponent<DummyComponent, TransformComponent>()));

    entity.AddComponent<TransformComponent>();
    EXPECT_TRUE((entity.HasComponent<DummyComponent, TransformComponent>()));
    EXPECT_TRUE((entity.HasComponent<TransformComponent, DummyComponent>()));
    EXPECT_TRUE((entity.HasAnyComponent<DummyComponent, TransformComponent>()));
    EXPECT_TRUE((entity.HasAnyComponent<TransformComponent, DummyComponent>()));
    EXPECT_TRUE((entity.HasComponent<TransformComponent, TransformComponent, TransformComponent>()));
    EXPECT_TRUE((entity.HasComponent<TransformComponent, DummyComponent, TransformComponent>()));
    EXPECT_TRUE((entity.HasAnyComponent<TransformComponent, TransformComponent, TransformComponent>()));

    entity.RemoveComponent<DummyComponent>();
    entity.RemoveComponent<TransformComponent>();
    EXPECT_FALSE((entity.HasAnyComponent<TransformComponent, DummyComponent>()));
    EXPECT_FALSE((entity.HasAnyComponent<DummyComponent, TransformComponent>()));
}

TEST(Entity, CreateWith)
{
    auto manager = CreateEntityManager();
    auto entity = manager->CreateEntityWith<DummyComponent, TransformComponent>();

    auto component1 = entity.GetComponent<DummyComponent>();
    auto component2 = entity.GetComponent<TransformComponent>();

    ASSERT_NE(component1, nullptr);
    ASSERT_NE(component2, nullptr);

    entity.Destroy();
    entity = manager->CreateEntity();
    component1 = entity.GetComponent<DummyComponent>();
    component2 = entity.GetComponent<TransformComponent>();
    EXPECT_EQ(nullptr, component1);
    EXPECT_EQ(nullptr, component2);
}