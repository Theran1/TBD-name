#pragma once
#include "Module.h"
#include "Animation.h"
#include "p2DynArray.h"
#include "Globals.h"

class Collider;
struct Object;



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
	update_status Update(float dt);
	update_status PostUpdate();
	bool CleanUp();
	void OnCollision(Collider* c1, Collider* c2);

public:

	SDL_Texture* gameBackground;
	SDL_Texture* title;
	SDL_Texture* objectives;
	SDL_Texture* gameOver;
	SDL_Texture* gameCompleted;

	WorldState worldState;

	Object moon;
	Object land;
	Object water;

	int deathHeight;

	bool simulating;
	bool completed;
	int objectivesCompleted;

	SDL_Rect objectivesRectangle = { 0,0,493,148 };
	SDL_Rect objectiveOne = { 0,171,224,16 };
	SDL_Rect objectiveTwo = { 0,219,367,16 };
	SDL_Rect objectiveThree = { 0,267,493,14 };
};
