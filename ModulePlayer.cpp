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

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) { accelerationY -= 0.06; }
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) { accelerationY += 0.06; }
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) { accelerationX -= 0.03; }
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) { accelerationX += 0.03; }

	accelerationY -= App->physics->gravity;

	if (accelerationX !=  0) { accelerationX = accelerationX * 0.95; }

	if (accelerationX > 2) { accelerationX = 2; }
	if (accelerationY > 2) { accelerationY = 2; }
	if (accelerationX < -2) { accelerationX = -2; }
	if (accelerationY < -2) { accelerationY = -2; }

	speedX += accelerationX;
	speedY += accelerationY;

	if (speedX > 4) { speedX = 4; }
	if (speedY > 8) { speedY = 8; }
	if (speedX < -4) { speedX = -4; }
	if (speedY < -8) { speedY = -8; }

	if (speedX != 0) { speedX = speedX * 0.95; }

	player.x += speedX;
	player.y += speedY;

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
