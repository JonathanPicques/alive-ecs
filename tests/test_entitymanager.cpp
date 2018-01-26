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
                                            ASSERT_EQ(physics, physics1);
                                        }
                                        else if (e == entity2)
                                        {
                                            ASSERT_EQ(physics, physics2);
                                        }
                                        else
                                        {
                                            FAIL();
                                        }
                                        count += 1;
                                    });

    manager->With<PhysicsComponent, TransformComponent>([&](auto e, auto physics, auto transform)
                                                        {
                                                            ASSERT_EQ(e, entity2);
                                                            ASSERT_EQ(physics, physics);
                                                            ASSERT_EQ(transform2, transform);
                                                            count += 1;
                                                        });

    manager->Any<PhysicsComponent, TransformComponent>([&](auto e, auto physics, auto transform)
                                                       {
                                                           if (e == entity)
                                                           {
                                                               ASSERT_EQ(physics, physics1);
                                                               ASSERT_EQ(transform, nullptr);
                                                           }
                                                           else if (e == entity2)
                                                           {
                                                               ASSERT_EQ(physics, physics2);
                                                               ASSERT_EQ(transform, transform2);
                                                           }
                                                           else
                                                           {
                                                               FAIL();
                                                           }
                                                           count += 1;
                                                       });

    ASSERT_EQ(count, 5);

}

TEST(EntityManager, EntityManager_Save_And_Load)
{
    auto manager = CreateEntityManager();

    auto entity = manager->Create();
    entity->AddComponent<DummyComponent>();
    entity->AddComponent<PhysicsComponent>();
    entity->AddComponent<TransformComponent>(32.0f, 64.0f);

    auto entity2 = manager->Create();
    entity2->AddComponent<DummyComponent>();
    entity2->AddComponent<PhysicsComponent>();
    entity2->AddComponent<TransformComponent>(128.0f, 128.0f);

    auto entity3 = manager->Create();
    entity3->AddComponent<PhysicsComponent>();
    entity3->AddComponent<TransformComponent>(52.0f, 89.0f);

    manager->Create();
    manager->Create();
    manager->Create();

    auto entity4 = manager->Create();
    entity4->AddComponent<TransformComponent>(1.0f, 1.0f);

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

        auto transform = entities_with_transform[0]->GetComponent<TransformComponent>();
        EXPECT_NE(transform, nullptr);
        EXPECT_EQ(transform->GetX(), 32.0f);
        EXPECT_EQ(transform->GetY(), 64.0f);

        auto transform2 = entities_with_transform[1]->GetComponent<TransformComponent>();
        EXPECT_NE(transform2, nullptr);
        EXPECT_EQ(transform2->GetX(), 128.0f);
        EXPECT_EQ(transform2->GetY(), 128.0f);

        auto transform3 = entities_with_transform[2]->GetComponent<TransformComponent>();
        EXPECT_NE(transform3, nullptr);
        EXPECT_EQ(transform3->GetX(), 52.0f);
        EXPECT_EQ(transform3->GetY(), 89.0f);

        auto transform4 = entities_with_transform[3]->GetComponent<TransformComponent>();
        EXPECT_NE(transform4, nullptr);
        EXPECT_EQ(transform4->GetX(), 1.0f);
        EXPECT_EQ(transform4->GetY(), 1.0f);
    }
}