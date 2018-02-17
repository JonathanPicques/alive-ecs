#include <core/entitymanager.hpp>

#include "systems.hpp"

DEFINE_SYSTEM(WorldStateSystem);

WorldStateSystem::WorldStateSystem(char* inputState, char* networkState) : mInputState(inputState), mNetworkState(networkState)
{

}

DEFINE_SYSTEM(GridMapSystem);

void GridMapSystem::OnLoad()
{
    System::OnLoad();
}

void GridMapSystem::OnResolveDependencies()
{
    mWorldStateSystem = mManager->GetSystem<WorldStateSystem>();
}