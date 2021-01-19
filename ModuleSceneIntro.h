#pragma once
#include "Module.h"
#include "Animation.h"
#include "p2DynArray.h"
#include "Globals.h"

enum class WorldState
{
	EARTH,
	SPACE,
	MOON
};

class ModuleSceneIntro;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	bool CleanUp();

public:

	SDL_Texture* gameBackground;
	SDL_Texture* title;
	SDL_Texture* objectives;

	WorldState worldState;

	bool simulating;
};
