#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModulePlayer.h"
#include <time.h>



ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled) {}

ModuleSceneIntro::~ModuleSceneIntro()
{}

bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = 0;
	App->renderer->camera.y = -2420;

	gameBackground = App->textures->Load("Assets/Background.png");
	title= App->textures->Load("Assets/Title.png");
	objectives = App->textures->Load("Assets/Objectives.png");
	gameOver = App->textures->Load("Assets/GameOver.png");
	gameCompleted = App->textures->Load("Assets/GameComplete.png");
	App->audio->PlayMusic("Assets/StarlaneStroll.ogg");

	srand(time(NULL));

	worldState = WorldState::EARTH;
	

	//Down where earth is 
	land.pos.x = 412;
	land.pos.y = 2878;
	land.pastPos.x = land.pos.x;
	land.pastPos.y = land.pos.y;
	land.mass = 0.0f;
	if (land.collider == nullptr)
	{
		land.collider = new Collider({ land.pos.x,land.pos.y,200,350 }, Collider::Type::SOLID, this);
		App->physics->AddObject(&land);
	}

	water.pos.x = 0;
	water.pos.y = 2878;
	water.mass = 0.0f;
	water.pastPos.x = water.pos.x;
	water.pastPos.y = water.pos.y;
	
	if (water.collider == nullptr)
	{
		water.collider = new Collider({ water.pos.x,water.pos.y,1024,300 }, Collider::Type::WATER, this);
		App->physics->AddObject(&water);
	}

	//Up where teh moon is
	moon.mass = 0.0f;
	moon.pos.x = 0;
	moon.pos.y = 0;
	moon.pastPos.x = moon.pos.x;
	moon.pastPos.y = moon.pos.y;
	if (moon.collider == nullptr)
	{
		moon.collider = new Collider({ moon.pos.x, moon.pos.y, 1024, 100 }, Collider::Type::MOON, this);
		App->physics->AddObject(&moon);
	}

	simulating = false;
	completed = false;
	objectivesCompleted = 0;

	return ret;
}

update_status ModuleSceneIntro::PreUpdate()
{
	App->renderer->camera.y = -(App->player->player.pos.y - 345);
	if (App->renderer->camera.y <= -2420) { App->renderer->camera.y = -2420; }
	if (App->renderer->camera.y >= -0) { App->renderer->camera.y = 0; }

	if (!simulating && App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) { 
        
        simulating = true; 
    }

	//Poner aqui condiciones para cambiar de "worldState"
	
	if (App->player->player.pos.y <= 1965 && worldState == WorldState::EARTH) {
		worldState = WorldState::SPACE;
	}
	 if (App->player->player.pos.y <= 650 && worldState == WorldState::SPACE) {
		worldState = WorldState::MOON;

	}
	 if (App->player->player.pos.y >= 650 && worldState == WorldState::MOON) {
		worldState = WorldState::SPACE;
	}
	 if (App->player->player.pos.y >= 1965 && worldState == WorldState::SPACE) {
		worldState = WorldState::EARTH;
	}

	 deathHeight = -App->renderer->camera.y;
	//LOG("Player poj %d %d", App->player->player.pos.x, App->player->player.pos.y);

	return UPDATE_CONTINUE;
}

update_status ModuleSceneIntro::Update(float dt)
{
	if (App->scene_intro->objectivesCompleted == 0 && App->player->player.pos.y <= 1900) { App->scene_intro->objectivesCompleted = 1; }
	if (App->player->touchedDaMoon && objectivesCompleted == 1) { App->scene_intro->objectivesCompleted = 2; }
	if (App->player->touchingTheWatah && objectivesCompleted == 2)
	{
		objectivesCompleted = 3;
		completed = true;
	}
	return UPDATE_CONTINUE;
}




update_status ModuleSceneIntro::PostUpdate()
{

	App->renderer->Blit(gameBackground, 0, 0, false);
	App->renderer->DrawQuad(water.collider->rect, 0, 26, 218, 100); //Water 
	App->renderer->DrawQuad(land.collider->rect, 190, 190, 190, 200); //platform
	App->renderer->DrawQuad(moon.collider->rect, 159, 156, 141); //Moon



	if (!simulating)
	{
		App->renderer->Blit(title, 0, 2333, false);
	}
	else
	{
		App->physics->Enable();
		App->player->Enable();
		//worldState = WorldState::EARTH;

		//Objectives
		if (!App->player->heDed)
		{
			App->renderer->Blit(objectives, -App->renderer->camera.x, -App->renderer->camera.y, &objectivesRectangle);
			switch (objectivesCompleted)
			{
			case 3:
				App->renderer->Blit(objectives, -App->renderer->camera.x, -App->renderer->camera.y + 134, &objectiveThree);
			case 2:
				App->renderer->Blit(objectives, -App->renderer->camera.x, -App->renderer->camera.y + 86, &objectiveTwo);
			case 1:
				App->renderer->Blit(objectives, -App->renderer->camera.x, -App->renderer->camera.y + 38, &objectiveOne);
				break;
			default:
				break;
			}
		}
	}
	if (App->physics->debug && land.collider != nullptr)
		App->renderer->DrawQuad(land.collider->rect, 255, 0, 0, 100);

	if (App->physics->debug && water.collider != nullptr)
		App->renderer->DrawQuad(water.collider->rect, 0, 0, 255, 100);

	if (App->physics->debug)
		App->renderer->DrawQuad(moon.collider->rect, 255, 0, 0, 100);

	if (App->player->heDed)
	{
		App->renderer->Blit(gameOver, 60, deathHeight + 200, false);
	}
	if (completed)
	{
		App->renderer->Blit(gameCompleted, -App->renderer->camera.x+100, -App->renderer->camera.y + 200, nullptr);
	}

	return UPDATE_CONTINUE;
}

bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	//unload textures
	App->textures->Unload(gameBackground);
	App->textures->Unload(title);
	App->textures->Unload(objectives);
	App->textures->Unload(gameOver);
	App->textures->Unload(gameCompleted);

	//delete Objects


	return true;
}



void ModuleSceneIntro::OnCollision(Collider* c1, Collider* c2) {}