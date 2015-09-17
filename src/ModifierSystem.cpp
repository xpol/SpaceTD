
#include "systems/ModifierSystem.hpp"
#include "Game.hpp"
#include "ResourceManager.hpp"

#include "components/Creep.hpp"
#include "components/Modifier.hpp"

#include <SDL2/SDL_ttf.h>

ModifierSystem::ModifierSystem() :
	game_(Game::getInstance()),
	rm_(ResourceManager::getInstance())
{
}
ModifierSystem::~ModifierSystem()
{
}

void ModifierSystem::update(entityx::EntityManager &es, entityx::EventManager &events, double dt)
{

}