#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModulePlayer.h"
#include "math.h"



ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	matrix[Collider::Type::SOLID][Collider::Type::SOLID] = false;
	matrix[Collider::Type::SOLID][Collider::Type::WATER] = false;
	matrix[Collider::Type::SOLID][Collider::Type::MOON] = false;
	matrix[Collider::Type::SOLID][Collider::Type::PLAYER] = true;
	//matrix[Collider::Type::SOLID][Collider::Type::OBSTACLE] = false;

	matrix[Collider::Type::WATER][Collider::Type::SOLID] = false;
	matrix[Collider::Type::WATER][Collider::Type::WATER] = false;
	matrix[Collider::Type::WATER][Collider::Type::MOON] = false;
	matrix[Collider::Type::WATER][Collider::Type::PLAYER] = true;
	//matrix[Collider::Type::WATER][Collider::Type::OBSTACLE] = false;

	matrix[Collider::Type::MOON][Collider::Type::SOLID] = false;
	matrix[Collider::Type::MOON][Collider::Type::WATER] = false;
	matrix[Collider::Type::MOON][Collider::Type::MOON] = false;
	matrix[Collider::Type::MOON][Collider::Type::PLAYER] = true;
	//matrix[Collider::Type::MOON][Collider::Type::OBSTACLE] = false;

	matrix[Collider::Type::PLAYER][Collider::Type::SOLID] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::WATER] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::MOON] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::PLAYER] = false;
	//matrix[Collider::Type::PLAYER][Collider::Type::OBSTACLE] = true;

	//matrix[Collider::Type::OBSTACLE][Collider::Type::SOLID] = false;
	//matrix[Collider::Type::OBSTACLE][Collider::Type::WATER] = false;
	//matrix[Collider::Type::OBSTACLE][Collider::Type::MOON] = false;
	//matrix[Collider::Type::OBSTACLE][Collider::Type::PLAYER] = true;
	//matrix[Collider::Type::OBSTACLE][Collider::Type::OBSTACLE] = false;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics");
	gravity.x = 0.0f;
	gravity.y = 0.0f;

	debug = false;
	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	
	// Delete colliders scheduled for deletion
	p2List_item<Object*>* tmp = objects.getFirst();
	while (tmp != nullptr)
	{
		if (tmp->data->collider != nullptr && tmp->data->collider->pendingToDelete == true)
		{
			delete tmp->data->collider;
			tmp->data->collider = nullptr;
		}

		tmp = tmp->next;
	}




	Collider* c1;
	Collider* c2;

	// Check collisions
	tmp = objects.getFirst();
	while (tmp != nullptr)
	{
		// skip empty colliders
		if (tmp->data->collider == nullptr)
		{
			tmp = tmp->next;
			continue;
		}

		c1 = tmp->data->collider;

		// avoid checking collisions already checked
		p2List_item<Object*>* tmp2 = tmp->next;
		while (tmp2 != nullptr)
		{
			// skip empty colliders
			if (tmp2->data->collider == nullptr)
			{
				tmp2 = tmp2->next;
				continue;
			}

			c2 = tmp2->data->collider;

			if (Intersects(tmp->data, tmp2->data) && matrix[c1->type][c2->type] && App->player->godLike == false)
			{
				ResolveCollisions(tmp->data, tmp2->data);
				if (c1->listener)
				{
					c1->listener->OnCollision(c1, c2);
				}
				if (c2->listener)
				{
					c2->listener->OnCollision(c2, c1);
				}
			}
			tmp2 = tmp2->next;
		}
		tmp = tmp->next;
	}

	return UPDATE_CONTINUE;
}





update_status ModulePhysics::Update(float dt)
{
	UpdateGravity();

	//LOG("grav %f", gravity.y);

	p2List_item<Object*>* temporal = objects.getFirst();
	while (temporal != nullptr)
	{
		if (temporal->data->mass != 0.0f)
		{
			UpdatePhysics(temporal->data, dt);
		}
		temporal = temporal->next;
	}
	return UPDATE_CONTINUE;
}



// 
update_status ModulePhysics::PostUpdate()
{

	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	objects.clear();

	return true;
}



fPoint ModulePhysics::Force2Accel(fPoint force, int mass)
{
	fPoint a;
	a.x = force.x / mass;
	a.y = force.y / mass;
	return a;
}



void ModulePhysics::UpdateGravity()
{
    switch (App->scene_intro->worldState)
    {
    case WorldState::EARTH:
    {
        gravity.x = 0.0f;
        gravity.y = 400.0f;
        break;
    }
    case WorldState::SPACE:
    {
        gravity.x = 0.0f;
        gravity.y = 0.0f;
        break;
    }
    case WorldState::MOON:
    {

		gravity.x = 0.0f;
		gravity.y = -300.0f;
        break;
    }
    default:
    {
        break;
    }
    }
}



void ModulePhysics::UpdatePhysics(Object* object, float dt)
{
	// Update the "Past" Variables for the corrections
	object->pastPos = object->pos;
	object->pastSpeed = object->speed;

	fPoint a = Force2Accel(object->force, object->mass);
	// Da Verlet Method
	object->pos.x = object->pos.x + object->speed.x * dt + ((gravity.x + a.x) * dt * dt * 0.5);
	object->speed.x = object->speed.x + ((gravity.x + a.x) * dt);
	object->pos.y = object->pos.y + object->speed.y * dt + ((gravity.y + a.y) * dt * dt * 0.5);
	object->speed.y = object->speed.y + ((gravity.y + a.y) * dt);

	// Reset the sum of forces
	object->force.x = 0.0f;
	object->force.y = 0.0f;

	// update the collider position
	if (object->collider != nullptr)
	{
		object->collider->SetPos(object->pos.x, object->pos.y, object->collider->rect.w, object->collider->rect.h);

	}
	
}


void ModulePhysics::ResolveCollisions(Object* A, Object* B)
{
	App->player->touchingTheWatah = false;
	if (A->collider->type == Collider::Type::WATER || B->collider->type == Collider::Type::WATER)
    {
        //BUOYANCY
        if (A->collider->type == Collider::Type::WATER)
        {
			App->player->touchingTheWatah = true;
			A->speed.SetToZero();
            float volume;
            if (A->collider->rect.y <= B->collider->rect.y)
            {
                volume = B->collider->rect.h * B->collider->rect.w;
            }
            else
            {
                volume = (float)abs((A->collider->rect.y - B->collider->rect.y) - B->collider->rect.h) * B->collider->rect.w;
            }
            float tmpForce = B->force.y + (B->mass * gravity.y - (WATER_DENSITY_LEVEL)*volume * gravity.y);
            B->speed.y *= DAMPENING_COEFICIENT;
            B->force.y = tmpForce;
            //LOG("Force: %f, volum: %f", B->force.y, volume);
        }
        else
        {

            ResolveCollisions(B, A);
        }
    }
    else
    {

		//Correction and intersections
        fPoint frameDifB;
        frameDifB.x = B->pos.x - B->pastPos.x;
        frameDifB.y = B->pos.y - B->pastPos.y;
        iPoint tmpB;
        tmpB.x = B->pos.x;
        tmpB.y = B->pos.y;
        fPoint frameDifA;
        frameDifA.x = A->pos.x - A->pastPos.x;
        frameDifA.y = A->pos.y - A->pastPos.y;
        iPoint tmpA;
        tmpA.x = A->pos.x;
        tmpA.y = A->pos.y;
        while (Intersects(A, B))
        {
            if (frameDifB.x > 0)
            {
                tmpB.x--;
                frameDifB.x--;
            }
            else if (frameDifB.x < 0)
            {
                tmpB.x++;
                frameDifB.x++;
            }
            if (frameDifB.y > 0)
            {
                tmpB.y--;
                frameDifB.y--;
            }
            else if (frameDifB.y < 0)
            {
                tmpB.y++;
                frameDifB.y++;
            }

            if (frameDifA.x > 0)
            {
                tmpA.x--;
                frameDifA.x--;
            }
            else if (frameDifA.x < 0)
            {
                tmpA.x++;
                frameDifA.x++;
            }
            if (frameDifA.y > 0)
            {
                tmpA.y--;
                frameDifA.y--;
            }
            else if (frameDifA.y < 0)
            {
                tmpA.y++;
                frameDifA.y++;
            }

            //dist = (float)abs(tmpB.y - tmpA.y);
            //LOG("%d,%d - %d,%d = %f", tmpB.x, tmpB.y, tmpA.x, tmpA.y, dist);
            LOG("%d,%d - %d,%d", tmpB.x, tmpB.y, tmpA.x, tmpA.y);
            A->collider->rect.x = tmpA.x;
            A->collider->rect.y = tmpA.y;
            B->collider->rect.x = tmpB.x;
            B->collider->rect.y = tmpB.y;
        }
        B->pos.x = tmpB.x;
        B->pos.y = tmpB.y;
        A->pos.x = tmpA.x;
        A->pos.y = tmpA.y;

        ResetSpeed(A);
        ResetSpeed(B);
    }
}



void ModulePhysics::ResetSpeed(Object* A)
{
	A->speed.x = 0.0f;
	A->speed.y = 0.0f;
}

void ModulePhysics::AddObject(Object* object)
{
	objects.add(object);
}

void ModulePhysics::RemoveObject(Object* object)
{
	p2List_item<Object*>* tmp = objects.findNode(object);
	object->collider = nullptr;
	objects.del(tmp);
}




void Collider::SetPos(int x, int y, int w, int h)
{
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
}

bool ModulePhysics::Intersects(Object* A, Object* B)
{

	return (A->collider->rect.x < B->collider->rect.x + B->collider->rect.w
		&& A->collider->rect.x + A->collider->rect.w > B->collider->rect.x
		&& A->collider->rect.y < B->collider->rect.y + B->collider->rect.h
		&& A->collider->rect.h + A->collider->rect.y > B->collider->rect.y);

}
 