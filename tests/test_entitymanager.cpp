#include <cstdint>
#include <gtest/gtest.h>

#include <core/entitymanager.hpp>

#include "test_components/components.hpp"

TEST(EntityManager, EntityManager_With)
{
    EntityManager manager;
    auto entity = manager.Create();
    auto physics1 = entity->AddComponent<PhysicsComponent>();

    auto entity2 = manager.Create();
    auto physics2 = entity2->AddComponent<PhysicsComponent>();
    auto transform2 = entity2->AddComponent<TransformComponent>();

    std::uint8_t count = 0;

    manager.With<PhysicsComponent>([&](auto e, auto physics)
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

    manager.With<PhysicsComponent, TransformComponent>([&](auto e, auto physics, auto transform)
                                                       {
                                                           ASSERT_EQ(e, entity2);
                                                           ASSERT_EQ(physics, physics);
                                                           ASSERT_EQ(transform2, transform);
                                                           count += 1;
                                                       });

    manager.Any<PhysicsComponent, TransformComponent>([&](auto e, auto physics, auto transform)
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

TEST(EntityManager, EntityManager_Save)
{
    EntityManager manager;
    auto entity = manager.Create();
    entity->AddComponent<DummyComponent>();
    entity->AddComponent<PhysicsComponent>();
    entity->AddComponent<TransformComponent>(32.0f, 64.0f);

    auto entity2 = manager.Create();
    entity2->AddComponent<DummyComponent>();
    entity2->AddComponent<PhysicsComponent>();
    entity2->AddComponent<TransformComponent>(128.0f, 128.0f);

    {
        std::filebuf f;
        std::ostream os(&f);

        f.open("save.bin", std::ios::out | std::ios::binary);
        manager.Save(os);
    }

    /*
    {
        std::filebuf f;
        std::istream is(&f);
        f.open("save.bin", std::ios::in | std::ios::binary);
        manager.Load(is);

        auto entities = manager.With<TransformComponent>();
        EXPECT_EQ(entities.size(), 2);
        for (auto i = 0; i < entities.size(); i++)
        {
            if (i == 0)
            {
                auto transform = entities[i]->GetComponent<TransformComponent>();
                EXPECT_NE(transform, nullptr);
                EXPECT_EQ(transform->GetX(), 32.0f);
                EXPECT_EQ(transform->GetY(), 64.0f);
            }
            else
            {
                auto transform = entities[i]->GetComponent<TransformComponent>();
                EXPECT_NE(transform, nullptr);
                EXPECT_EQ(transform->GetX(), 32.0f);
                EXPECT_EQ(transform->GetY(), 64.0f);
            }
        }
    }
    */
}