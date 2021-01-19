#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"



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

	worldState = WorldState::EARTH;
	return ret;
}

update_status ModuleSceneIntro::PreUpdate()
{
	App->renderer->camera.y = -(App->player->player.y - 345);
	if (App->renderer->camera.y <= -2420) { App->renderer->camera.y = -2420; }
	if (App->renderer->camera.y >= -0) { App->renderer->camera.y = 0; }

	if (!simulating && App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) { simulating = true; }



	switch (worldState)
	{
	case WorldState::EARTH:
		//if (App->physics->gravity != -0.004) { App->physics->gravity = -0, 004; }
		break;
	case WorldState::SPACE:
		App->physics->gravity = 0;
		break;
	case WorldState::MOON:
		App->physics->gravity = 0, 002;
		break;
	default:
		break;
	}

	return UPDATE_CONTINUE;
}

update_status ModuleSceneIntro::Update()
{
	App->renderer->Blit(gameBackground, 0, 0, false);
	App->renderer->DrawQuad({ 0,2878,1024,300 }, 0, 26, 218,100);
	App->renderer->DrawQuad({ 412,2878,200,350 }, 190, 190, 190,200);
	App->renderer->DrawQuad({ 412,2828,200,50 }, 190, 190, 190);
	App->renderer->DrawQuad({ 0,0,1024,200 }, 159, 156, 141);

	if (!simulating)
	{
		App->renderer->Blit(title, 0, 2333, false);
	}
	else { App->renderer->Blit(objectives, -App->renderer->camera.x, -App->renderer->camera.y, false); }

	return UPDATE_CONTINUE;
}

bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}
