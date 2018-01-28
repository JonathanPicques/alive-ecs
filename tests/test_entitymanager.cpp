#include <cstdint>
#include <fstream>
#include <gtest/gtest.h>

#include <core/entitymanager.hpp>

#include "test_components/components.hpp"

TEST(EntityManager, EntityManager_With)
{
    auto manager = CreateEntityManager();

    auto entity = manager->Create();
    auto physics1 = entity->AddComponent<PhysicsComponent>();

    auto entity2 = manager->Create();
    auto physics2 = entity2->AddComponent<PhysicsComponent>();
    auto transform2 = entity2->AddComponent<TransformComponent>();

    std::uint8_t count = 0;

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

    EXPECT_EQ(count, 5);

}

TEST(EntityManager, EntityManager_Save_And_Load)
{
    auto manager = CreateEntityManager();

    auto entity = manager->Create();
    entity->AddComponent<DummyComponent>();
    entity->AddComponent<PhysicsComponent>();
    auto transform = entity->AddComponent<TransformComponent>();
    transform->mData.x = 32.0f;
    transform->mData.y = 64.0f;

    auto entity2 = manager->Create();
    entity2->AddComponent<DummyComponent>();
    entity2->AddComponent<PhysicsComponent>();
    transform = entity2->AddComponent<TransformComponent>();
    transform->mData.x = 128.0f;
    transform->mData.y = 128.0f;

    auto entity3 = manager->Create();
    entity3->AddComponent<PhysicsComponent>();
    transform = entity3->AddComponent<TransformComponent>();
    transform->mData.x = 52.0f;
    transform->mData.y = 89.0f;

    manager->Create();
    manager->Create();
    manager->Create();

    auto entity4 = manager->Create();
    transform = entity4->AddComponent<TransformComponent>();
    transform->mData.x = 1.0f;
    transform->mData.y = 1.0f;

    {
        std::filebuf f;
        std::ostream os(&f);

        f.open("save.bin", std::ios::out | std::ios::binary);
        manager->Serialize(os);
    }

    {
        entity3->Destroy();

        manager->Create();
        manager->Create();

        entity2->Destroy();
        entity4->Destroy();

        manager->Create();

        manager->DestroyEntities();

        std::filebuf f;
        std::istream is(&f);
        f.open("save.bin", std::ios::in | std::ios::binary);
        manager->Deserialize(is);

        auto entities_with_transform = manager->With<TransformComponent>();
        EXPECT_EQ(entities_with_transform.size(), 4);

        auto transform1 = entities_with_transform[0]->GetComponent<TransformComponent>();
        ASSERT_NE(transform1, nullptr);
        EXPECT_EQ(transform1->GetX(), 32.0f);
        EXPECT_EQ(transform1->GetY(), 64.0f);

        auto transform2 = entities_with_transform[1]->GetComponent<TransformComponent>();
        ASSERT_NE(transform2, nullptr);
        EXPECT_EQ(transform2->GetX(), 128.0f);
        EXPECT_EQ(transform2->GetY(), 128.0f);

        auto transform3 = entities_with_transform[2]->GetComponent<TransformComponent>();
        ASSERT_NE(transform3, nullptr);
        EXPECT_EQ(transform3->GetX(), 52.0f);
        EXPECT_EQ(transform3->GetY(), 89.0f);

        auto transform4 = entities_with_transform[3]->GetComponent<TransformComponent>();
        ASSERT_NE(transform4, nullptr);
        EXPECT_EQ(transform4->GetX(), 1.0f);
        EXPECT_EQ(transform4->GetY(), 1.0f);
    }
}