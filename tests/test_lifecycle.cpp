#include <gtest/gtest.h>

#include <core/entitymanager.hpp>

class BodyComponent final : public Component
{
public:
    DECLARE_COMPONENT(BodyComponent);
protected:
    void OnResolveDependencies() final
    {
        mHeartComponent = mEntity->GetComponent<HeartComponent>();
    }
public:
    class HeartComponent *mHeartComponent = nullptr;
};
DEFINE_COMPONENT(BodyComponent)

class HeartComponent final : public Component
{
public:
    DECLARE_COMPONENT(HeartComponent);
protected:
    void OnResolveDependencies() final
    {
        mBodyComponent = mEntity->GetComponent<BodyComponent>();
    }
public:
    BodyComponent *mBodyComponent = nullptr;
};
DEFINE_COMPONENT(HeartComponent);

class LegMuscleComponent final : public Component
{
public:
    DECLARE_COMPONENT(LegMuscleComponent);
protected:
    void OnResolveDependencies() final
    {
        mHeartComponent = mEntity->GetComponent<HeartComponent>();
        mBodyComponent = mEntity->GetComponent<BodyComponent>();
    }
public:
    class HeartComponent *mHeartComponent = nullptr;
    class BodyComponent *mBodyComponent = nullptr;
};
DEFINE_COMPONENT(LegMuscleComponent);

TEST(Lifecycle, Dependency_Manual_Lifecycle)
{
    EntityManager manager;
    manager.RegisterComponent<BodyComponent>();
    manager.RegisterComponent<HeartComponent>();
    manager.RegisterComponent<LegMuscleComponent>();

    auto entity = manager.Create();
    auto leg = entity->AddComponent<LegMuscleComponent>(); // depends on heart and body
    auto heart = entity->AddComponent<HeartComponent>(); // depends on body
    auto body = entity->AddComponent<BodyComponent>(); // depends on heart

    // in this order, leg has nothing resolved, heart has nothing resolved and body has heart resolved
    EXPECT_EQ(nullptr, leg->mBodyComponent);
    EXPECT_EQ(nullptr, leg->mHeartComponent);
    EXPECT_EQ(nullptr, heart->mBodyComponent);
    EXPECT_EQ(heart, body->mHeartComponent);

    entity->ResolveComponentDependencies();

    // all should be resolved
    EXPECT_EQ(body, leg->mBodyComponent);
    EXPECT_EQ(heart, leg->mHeartComponent);
    EXPECT_EQ(body, heart->mBodyComponent);
    EXPECT_EQ(heart, body->mHeartComponent);
}

TEST(Lifecycle, Dependency_Automatic_Lifecycle)
{
    EntityManager manager;
    manager.RegisterComponent<BodyComponent>();
    manager.RegisterComponent<HeartComponent>();
    manager.RegisterComponent<LegMuscleComponent>();

    auto entity = manager.CreateWith<HeartComponent, BodyComponent, LegMuscleComponent>();

    auto leg = entity->GetComponent<LegMuscleComponent>(); // depends on heart and body
    auto heart = entity->GetComponent<HeartComponent>(); // depends on body
    auto body = entity->GetComponent<BodyComponent>(); // depends on heart

    EXPECT_EQ(body, leg->mBodyComponent);
    EXPECT_EQ(heart, leg->mHeartComponent);
    EXPECT_EQ(body, heart->mBodyComponent);
    EXPECT_EQ(heart, body->mHeartComponent);
}

TEST(Lifecycle, Dependency_Mixed_Manual_Automatic_Lifecycle)
{
    EntityManager manager;
    manager.RegisterComponent<BodyComponent>();
    manager.RegisterComponent<HeartComponent>();
    manager.RegisterComponent<LegMuscleComponent>();

    auto entity = manager.CreateWith<HeartComponent, LegMuscleComponent>();

    auto body = entity->AddComponent<BodyComponent>(); // depends on heart
    auto leg = entity->GetComponent<LegMuscleComponent>(); // depends on heart and body
    auto heart = entity->GetComponent<HeartComponent>(); // depends on body

    // body was left out of the resolve
    // heart's reference to body is missing
    // body was loaded after heart, but can only be resolved if handled in OnLoad() method because OnResolveDependencies() would not be called...
    // ...but since OnLoad() defaults to call OnResolveDependencies(), heart dep is resolved
    // leg reference to heart is resolved, but not towards body

    EXPECT_EQ(nullptr, leg->mBodyComponent);
    EXPECT_EQ(heart, leg->mHeartComponent);
    EXPECT_EQ(nullptr, heart->mBodyComponent);
    EXPECT_EQ(heart, body->mHeartComponent);

    entity->ResolveComponentDependencies();

    EXPECT_EQ(body, leg->mBodyComponent);
    EXPECT_EQ(heart, leg->mHeartComponent);
    EXPECT_EQ(body, heart->mBodyComponent);
    EXPECT_EQ(heart, body->mHeartComponent);
}