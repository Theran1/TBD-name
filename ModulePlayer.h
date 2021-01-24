#pragma once
#include "Module.h"
#include "Animation.h"
#include "Globals.h"
#include "p2Point.h"
#include "ModulePhysics.h"

class Collider;
struct Object;


class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status PreUpdate();
	update_status Update(float dt);
	update_status PostUpdate();
	bool CleanUp();
	void OnCollision(Collider* c1, Collider* c2);
	void UpdateDrag();

public:

	Object player; 
	
	//the width and height of the player is within the object (use player.collider.rect.w for example
	SDL_Texture* explosionTexture = nullptr;
	SDL_Texture* rocket;

	bool godLike;
	bool heDed;
	bool touchedDaMoon = false;
	bool touchingTheWatah = false;
	bool once = true;


	fPoint direction;

	bool isMovingUp;
	bool isMovingLeft;
	bool isMovingRight;
	bool isMovingDown;


	Animation explosion;
	Animation* currentAnimation;

	unsigned int explosionSFX;


};