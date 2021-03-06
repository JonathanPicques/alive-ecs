#pragma once

#include <core/system.hpp>

class WorldStateSystem final : public System
{
public:
    DECLARE_SYSTEM(WorldStateSystem);

public:
    WorldStateSystem(char* inputState, char* networkState);

public:
    char* mInputState = nullptr;
    char* mNetworkState = nullptr;
};

class GridMapSystem final : public System
{
public:
    DECLARE_SYSTEM(GridMapSystem);

public:
    void OnLoad() final;
    void OnResolveDependencies() final;

public:
    WorldStateSystem *mWorldStateSystem;
};