#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "PhysBody.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePlayer::~ModulePlayer()
{}


bool ModulePlayer::Start()
{
	LOG("Loading player");
	rocket = App->textures->Load("Assets/Rocket.png");
	player = { 478,2711,23,39 };
	accelerationX = accelerationY = speedX = speedY = 0;
	return true;
}

update_status ModulePlayer::PreUpdate()
{
	if (!App->scene_intro->simulating) { return UPDATE_CONTINUE; }

	prePosX = player.x;
	prePosY = player.y;

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) { accelerationY -= 0.01; }
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) { accelerationY += 0.01; }
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) { accelerationX -= 0.01; }
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) { accelerationX += 0.01; }

	if (accelerationX > 0) { accelerationX -= 0.004; }
	if (accelerationX > 0) { accelerationX += 0.004; }
	accelerationY -= App->physics->gravity;

	if (accelerationX > 1) { accelerationX = 1; }
	if (accelerationY > 1) { accelerationY = 1; }
	if (accelerationX < -1) { accelerationX = -1; }
	if (accelerationY < -1) { accelerationY = -1; }

	speedX += accelerationX;
	speedY += accelerationY;

	if (speedX > 2) { speedX = 2; }
	if (speedY > 5) { speedY = 5; }
	if (speedX < -2) { speedX = -2; }
	if (speedY < -5) { speedY = -5; }

	if (speedX > 0) { speedX -= App->physics->drag; }
	if (speedX < 0) { speedX += App->physics->drag; }

	player.x += speedX /*+ 0.5 * accelerationX*/;
	player.y += speedY /*+ 0.5 * accelerationY*/;

	return UPDATE_CONTINUE;
}

update_status ModulePlayer::Update()
{
	App->renderer->Blit(rocket, player.x, player.y, false);
	return UPDATE_CONTINUE;
}



bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}




