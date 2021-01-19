#pragma once
#include "Module.h"
#include "Animation.h"
#include "Globals.h"
#include "p2Point.h"

class PhysBody;

struct Object
{
	SDL_Texture* graphic;
	PhysBody* body;
	uint fx;

	Object() : graphic(NULL), body(NULL)
	{}
};

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	bool CleanUp();

public:
	int prePosX = 0;
	int prePosY = 0;
	SDL_Texture* rocket;
	SDL_Rect player;
	float accelerationX;
	float accelerationY;
	float speedX;
	float speedY;
};