#include "systems.hpp"

DEFINE_SYSTEM(WorldStateSystem);

WorldStateSystem::WorldStateSystem(char* inputState, char* networkState) : mInputState(inputState), mNetworkState(networkState)
{

}

void WorldStateSystem::Update()
{

}