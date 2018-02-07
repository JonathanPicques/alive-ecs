#include <random>
#include <cstdint>
#include <fstream>
#include <gtest/gtest.h>

#include "core/entitymanager.hpp"

#include "test_components/components.hpp"

TEST(EntityManager, AnyAndWith)
{
    auto manager = CreateEntityManager();

    auto entity = manager->CreateEntity();
    auto physics1 = entity.AddComponent<PhysicsComponent>();

    auto entity2 = manager->CreateEntity();
    auto physics2 = entity2.AddComponent<PhysicsComponent>();
    auto transform2 = entity2.AddComponent<TransformComponent>();

    std::uint8_t count = 0;

    entity.With<PhysicsComponent>([&](auto physics)
                                  {
                                      EXPECT_EQ(physics, physics1);
                                      count += 1;
                                  });

    entity.Any<PhysicsComponent, TransformComponent>([&](auto physics, auto transform)
                                  {
                                      EXPECT_EQ(physics, physics1);
                                      EXPECT_EQ(nullptr, transform);
                                      count += 1;
                                  });

    manager->With<PhysicsComponent>([&](auto e, auto physics)
                                    {
                                        if (e == entity)
                                        {
                                            EXPECT_EQ(physics, physics1);
                                        }
                                        else if (e == entity2)
                                        {
                                            EXPECT_EQ(physics, physics2);
                                        }
                                        else
                                        {
                                            FAIL();
                                        }
                                        count += 1;
                                    });

    manager->With<PhysicsComponent, TransformComponent>([&](auto e, auto physics, auto transform)
                                                        {
                                                            EXPECT_EQ(e, entity2);
                                                            EXPECT_EQ(physics, physics);
                                                            EXPECT_EQ(transform2, transform);
                                                            count += 1;
                                                        });

    manager->Any<PhysicsComponent, TransformComponent>([&](auto e, auto physics, auto transform)
                                                       {
                                                           if (e == entity)
                                                           {
                                                               EXPECT_EQ(physics, physics1);
                                                               EXPECT_EQ(transform, nullptr);
                                                           }
                                                           else if (e == entity2)
                                                           {
                                                               EXPECT_EQ(physics, physics2);
                                                               EXPECT_EQ(transform, transform2);
                                                           }
                                                           else
                                                           {
                                                               FAIL();
                                                           }
                                                           count += 1;
                                                       });

    EXPECT_EQ(count, 7);

}

TEST(EntityManager, Clear)
{
    auto manager = CreateEntityManager();
    manager->CreateEntity().Destroy();
    manager->CreateEntity();
    manager->CreateEntity();
    manager->CreateEntity();
    manager->CreateEntity().Destroy();
    manager->CreateEntity().Destroy();
    manager->CreateEntity().Destroy();
    manager->CreateEntity();
    manager->CreateEntity().Destroy();
    manager->CreateEntity().Destroy();
    manager->CreateEntity().Destroy();
    manager->CreateEntity();
    manager->CreateEntity();
    manager->CreateEntity();
    manager->CreateEntity();

    EXPECT_EQ(8, manager->Size());
    manager->Clear();
    EXPECT_EQ(0, manager->Size());
}

TEST(EntityManager, SaveAndLoad)
{
    auto manager = CreateEntityManager();

    auto entity1 = manager->CreateEntity();
    entity1.AddComponent<DummyComponent>();
    entity1.AddComponent<PhysicsComponent>();
    auto transform = entity1.AddComponent<TransformComponent>();
    transform->mData.x = 32.0f;
    transform->mData.y = 64.0f;

    auto entity2 = manager->CreateEntity(); // 1:1
    entity2.AddComponent<DummyComponent>();
    entity2.AddComponent<PhysicsComponent>();
    transform = entity2.AddComponent<TransformComponent>();
    transform->mData.x = 128.0f;
    transform->mData.y = 128.0f;

    auto entity3 = manager->CreateEntity(); // 2:1
    entity3.AddComponent<PhysicsComponent>();
    transform = entity3.AddComponent<TransformComponent>();
    transform->mData.x = 52.0f;
    transform->mData.y = 89.0f;

    manager->CreateEntity(); // 3:1
    manager->CreateEntity().Destroy(); // 4:1
    manager->CreateEntity().Destroy(); // 4:2

    auto entity4 = manager->CreateEntity(); // 4:3
    transform = entity4.AddComponent<TransformComponent>();
    transform->mData.x = 1.0f;
    transform->mData.y = 1.0f;

    manager->CreateEntity();

    {
        std::filebuf f;
        std::ostream os(&f);

        f.open("save.bin", std::ios::out | std::ios::binary);
        manager->Serialize(os);
    }

    {
        entity3.Destroy();

        manager->CreateEntity();
        manager->CreateEntity();

        entity2.Destroy();
        entity4.Destroy();

        manager->CreateEntity();

        std::filebuf f;
        std::istream is(&f);
        f.open("save.bin", std::ios::in | std::ios::binary);
        manager->Deserialize(is);

        auto entities_with_transform = manager->With<TransformComponent>();
        ASSERT_EQ(entities_with_transform.size(), 4);

        ASSERT_EQ(entities_with_transform[0], entity1);
        auto transform1 = entities_with_transform[0].GetComponent<TransformComponent>();
        ASSERT_NE(transform1, nullptr);
        EXPECT_EQ(transform1->GetX(), 32.0f);
        EXPECT_EQ(transform1->GetY(), 64.0f);

        ASSERT_EQ(entities_with_transform[1], entity2);
        auto transform2 = entities_with_transform[1].GetComponent<TransformComponent>();
        ASSERT_NE(transform2, nullptr);
        EXPECT_EQ(transform2->GetX(), 128.0f);
        EXPECT_EQ(transform2->GetY(), 128.0f);

        ASSERT_EQ(entities_with_transform[2], entity3);
        auto transform3 = entities_with_transform[2].GetComponent<TransformComponent>();
        ASSERT_NE(transform3, nullptr);
        EXPECT_EQ(transform3->GetX(), 52.0f);
        EXPECT_EQ(transform3->GetY(), 89.0f);

        ASSERT_EQ(entities_with_transform[3], entity4);
        auto transform4 = entities_with_transform[3].GetComponent<TransformComponent>();
        ASSERT_NE(transform4, nullptr);
        EXPECT_EQ(transform4->GetX(), 1.0f);
        EXPECT_EQ(transform4->GetY(), 1.0f);
    }
}

TEST(EntityManager, SparseSaveAndLoad)
{
    auto manager = CreateEntityManager();
    auto entity0 = manager->CreateEntity();
    auto entity1 = manager->CreateEntity();
    auto entity2 = manager->CreateEntity();
    auto entity3 = manager->CreateEntity();
    auto entity4 = manager->CreateEntity();
    auto entity5 = manager->CreateEntity();
    auto entity6 = manager->CreateEntity();
    auto entity7 = manager->CreateEntity();
    auto entity8 = manager->CreateEntity();

    entity1.Destroy();
    entity2.Destroy();
    entity4.Destroy();
    entity6.Destroy();
    entity8.Destroy();

    auto entity9 = manager->CreateEntity();

    {
        std::filebuf f;
        std::ostream os(&f);

        f.open("sparse_save.bin", std::ios::out | std::ios::binary);
        manager->Serialize(os);
    }

    {
        std::filebuf f;
        std::istream is(&f);
        f.open("sparse_save.bin", std::ios::in | std::ios::binary);
        manager->Deserialize(is);

        std::vector<Entity> entities;
        for (auto entity : *manager)
        {
            entities.emplace_back(entity);
        }
        ASSERT_EQ(5, entities.size());
        EXPECT_EQ(entity0, entities[0]);
        EXPECT_EQ(entity3, entities[1]);
        EXPECT_EQ(entity5, entities[2]);
        EXPECT_EQ(entity7, entities[3]);
        EXPECT_EQ(entity9, entities[4]);

    }
}

TEST(EntityManager, ExtremeSparseSaveAndLoad)
{
    auto manager = CreateEntityManager();
    std::vector<Entity> entities;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, 15);
    for (auto i = 0; i < 1000; i++)
    {
        for (auto j = 0; j < distribution(generator); j++)
        {
            manager->CreateEntity().Destroy();
        }
        entities.emplace_back(manager->CreateEntity());
        manager->CreateEntity().Destroy();
    }
    for (auto i = 0; i < 1000; i += 2)
    {
        entities[i].Destroy();
    }

    {
        std::filebuf f;
        std::ostream os(&f);

        f.open("extreme_sparse_save.bin", std::ios::out | std::ios::binary);
        manager->Serialize(os);
    }

    {
        std::filebuf f;
        std::istream is(&f);
        f.open("extreme_sparse_save.bin", std::ios::in | std::ios::binary);
        manager->Deserialize(is);

        std::vector<Entity> entities2;
        for (auto entity : *manager)
        {
            entities2.emplace_back(entity);
        }
        ASSERT_EQ(500, entities2.size());
        for (auto i = 0; i < 500; i++)
        {
            ASSERT_EQ(entities[i * 2 + 1], entities2[i]);
        }
    }
}