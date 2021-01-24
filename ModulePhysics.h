#pragma once
#include "Module.h"
#include "Globals.h"


#define WATER_DENSITY_LEVEL 0.018
#define DAMPENING_COEFICIENT 0.98
#define DRAG_FORCE 500.0f


class Collider
{
public:
	// Collision Type enum
	enum Type
	{
		NONE = -1,
		SOLID,
		WATER,
		MOON,
		PLAYER,
		//OBSTACLE,  //if we have time to add it
 		MAX
	};

	// Constructor
	Collider(SDL_Rect rect, Type type, Module* listener = nullptr) : rect(rect), type(type), listener(listener)
	{}

	// Sets the position of the construct
	void SetPos(int x, int y, int w, int h);

	SDL_Rect rect;
	bool pendingToDelete = false;
	Type type;
	Module* listener = nullptr;
};


struct Object
{
	Object()
	{}

	Object(iPoint pos, fPoint speed, fPoint force, float mass = 0.0f, Collider* c = nullptr/*, float angle = 0.0f*/) : pos(pos), speed(speed), force(force), mass(mass), collider(c) /*,  angle(angle)*/ 
	{}


	iPoint pos;
	iPoint pastPos;
	fPoint speed;
	fPoint pastSpeed;
	fPoint force;
	float mass;
	Collider* collider;
	//float angle;

};


class ModulePhysics : public Module
{
public:


	ModulePhysics(Application* app, bool start_enabled = false);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status Update(float dt);
	update_status PostUpdate();
	bool CleanUp();

	void UpdateDrag(fPoint direction);


	void UpdatePhysics(Object* object, float dt);


	void AddObject(Object* object);
	void RemoveObject(Object* object);


private:
	fPoint Force2Accel(fPoint force, int mass);
	void UpdateGravity();

	// Checks if two rects are intersecting
	//bool Intersects(const SDL_Rect& r) const;
	bool Intersects(Object* A, Object* B);
	void ResolveCollisions(Object* A, Object* B); //without the ""Raycasting"" Method
	void ResetSpeed(Object* A);

public:
	fPoint gravity;
	bool debug;
	fPoint dragDirection;

private:
	// All existing objects in the scene
	p2List<Object*> objects;

	// The collision matrix. Defines the interaction for two collider types
	// If set to false, collider 1 will ignore collider 2
	bool matrix[Collider::Type::MAX][Collider::Type::MAX];

};