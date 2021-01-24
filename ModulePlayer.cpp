#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModuleSceneIntro.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePlayer::~ModulePlayer()
{}


bool ModulePlayer::Start()
{
	LOG("Loading player");
	rocket = App->textures->Load("Assets/Rocket.png");
	//player.collider->rect = { 478,2711,23,39 }; //x y w h
	player.force.SetToZero();
	player.mass = 4.0f;
	player.pos.x = 478;
	player.pos.y = 2878 - 39;
	player.pastPos.x = player.pos.x;
	player.pastPos.y = player.pos.y;
	
	heDed = false;

	explosionTexture = App->textures->Load("Assets/Explosion.png");
	explosionSFX = App->audio->LoadFx("Assets/Explosion.wav");

	for (int i = 0; i < 8; i++)
	{
		explosion.PushBack({ i * 39,0,39,39 });
	}
	explosion.SetSpeed(0.2f);
	explosion.SetLoop(false);

	currentAnimation = &explosion;

	if (player.collider == nullptr)
	{
		player.collider = new Collider({ 478,2711,23,39 }, Collider::Type::PLAYER, this);
		App->physics->AddObject(&player);
	}


	isMovingUp = false;
	isMovingLeft = false;
	isMovingRight = false;
	isMovingDown = false;

	return true;
}

update_status ModulePlayer::PreUpdate()
{

	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
	{
		heDed = true;
		
	}
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)  //GodMode Debug stuff
	{
		godLike = !godLike;
	}

	if (!heDed && !App->scene_intro->completed)
	{

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			player.force.y = -500.0f * player.mass;
			/*isMovingUp = true;
			isMovingDown = false;*/
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			player.force.x = -500.0f * player.mass;
			/*isMovingLeft = true;
			isMovingRight = false;*/
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			player.force.y = 500.0f * player.mass;
			/*isMovingDown = true;
			isMovingUp = false;*/
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			player.force.x = 500.0f * player.mass;
			/*isMovingRight = true;
			isMovingLeft = false;*/
		}



		UpdateDrag();
		if (App->scene_intro->worldState == WorldState::EARTH) {
			player.force.x = player.force.x + App->physics->dragDirection.x * DRAG_FORCE;
			player.force.y = player.force.y + App->physics->dragDirection.y * DRAG_FORCE;
		}
	}
	else
	{
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		{
			player.pos.x = 478;
			player.pos.y = 2878 - 39;
			player.pastPos.x = player.pos.x;
			player.pastPos.y = player.pos.y;
			player.force.SetToZero();
			explosion.Reset();
			heDed = false;
			once = true;
			touchedDaMoon = false;
			App->scene_intro->completed = false;
			App->scene_intro->objectivesCompleted = 0;

			if (player.collider == nullptr)
			{
				player.collider = new Collider({ 478,2711,23,39 }, Collider::Type::PLAYER, this);
				App->physics->AddObject(&player);
			}
			App->scene_intro->worldState = WorldState::EARTH;
			//App->scene_intro->CreateEarth();
		}
	}


	return UPDATE_CONTINUE;
}


update_status ModulePlayer::Update(float dt)
{

	/* if you want animations to go with framerate: dtAnim = dt;*/
	if (!heDed)
	{
		//Player boundries
		if ((player.pos.x) <= 0) //Left bound
		{
			player.pos.x = 1;
			player.speed.x = 0;
		
		}
		if ((player.pos.x + player.collider->rect.w) > (App->renderer->camera.w)) //Right bound
		{
			player.pos.x = App->renderer->camera.w - player.collider->rect.w - 1;
			player.speed.x = 0;
			
		}
		if ((player.pos.y) <= 0) //Up bound
		{
			player.pos.y = 1;
			player.speed.y = 0;
		}
		if ((player.pos.y + player.collider->rect.h) > (3179)) //Bottom bound
		{
			player.pos.y = 3179 - player.collider->rect.h - 1;
			player.speed.y = 0;
		}

		//Speed limits
		if (player.speed.y > 400.0f)
			player.speed.y = 400.0f;
		else if (player.speed.y < -400.0f)
			player.speed.y = -400.0f;
		if (player.speed.x > 400.0f)
			player.speed.x = 400.0f;
		else if (player.speed.x < -400.0f)
			player.speed.x = -400.0f;
		//LOG("%f,%f", player.speed.x, player.speed.y);

		//if (touchingTheWatah && touchedDaMoon && abs((int)player.speed.y) <= 1.0f && abs((int)player.pastSpeed.y) <= 1.0f)                 // Victory
		//{
		//	//win or smt idk
		//	LOG("YOUWISJKAFGHJGAHSJFGASDHVGFJJHSVADHJVSEHIJKFSDKHJFFBDKBHJKDHBFJHMSDB FNJMBSDFJHSDFUHJG");
		//}

	}
	else  //he is F*^**** dead
	{
		if (once)
		{
			App->audio->PlayFx(explosionSFX, 0);
			once = false;
		}
		if (player.collider != nullptr)
		{
			App->physics->RemoveObject(&player);
		}
		player.speed.x = 0.0f;
		player.speed.y = 0.0f;
	}


	return UPDATE_CONTINUE;
}



update_status ModulePlayer::PostUpdate()
{
	if (heDed)
	{
		currentAnimation->Update();
		App->renderer->Blit(explosionTexture, player.pos.x - 8, player.pos.y, &currentAnimation->GetCurrentFrame());
	}
	else { App->renderer->Blit(rocket, player.pos.x, player.pos.y, false); }

	if (App->physics->debug && player.collider != nullptr) { App->renderer->DrawQuad(player.collider->rect, 0, 225, 100, 100); }
	return UPDATE_CONTINUE;
}

void ModulePlayer::OnCollision(Collider* c1, Collider* c2)
{
	//LOG("player state %d", App->scene_intro->worldState);
	if (!godLike)
	{
		if (App->scene_intro->worldState == WorldState::EARTH)
		{
			if (c2->type == Collider::Type::SOLID)
			{
				if (abs((int)player.pastSpeed.y) >= 300.0f)
				{
					heDed = true;
				}
				//LOG("Solid!");
			}
		}
		if (App->scene_intro->worldState == WorldState::SPACE)
		{
			/*if (c2->type == Collider::Type::OBSTACLE)
			{
				heDed = true;
			}*/
		}
		if (App->scene_intro->worldState == WorldState::MOON)
		{
			if (c2->type == Collider::Type::MOON)
			{
				if (abs((int)player.pastSpeed.y) >= 300.0f)
				{
					heDed = true;
				}
				touchedDaMoon = true;
				//LOG("Da Moon");
			}
		}
	}



}


bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");
	App->textures->Unload(rocket);
	App->textures->Unload(explosionTexture);

	return true;
}



void ModulePlayer::UpdateDrag()
{
	if (isMovingRight == true && isMovingUp == true) {//TopRight Direction +,-
		//direction = { 1.0f,-1.0f };
		direction.x = 1.0f;
		direction.y = -1.0f;
	}
	else if (isMovingRight == true && isMovingDown == true) { //BottomRight Direction +,+
		//direction = { -1.0f,-1.0f };
		direction.x = -1.0f;
		direction.y = -1.0f;
	}
	else if (isMovingLeft == true && isMovingDown == true) {//BottomLeft Direction -,+
		//direction = { -1.0f,1.0f };
		direction.x = -1.0f;
		direction.y = 1.0f;
	}
	else if (isMovingLeft == true && isMovingUp == true) {//TopLeft Direction -,-
		direction.x = 1.0f;
		direction.y = 1.0f;
	}
	else if (isMovingDown == true) {
		direction.x = 0.0f;
		direction.y = 1.0f;
	}
	else if (isMovingUp == true) {
		direction.x = 0.0f;
		direction.y = -1.0f;
	}
	else if (isMovingRight == true) {		
		direction.x = 1.0f;
		direction.y = 0.0f;
	}
	else if (isMovingLeft == true) {
		direction.x = -1.0f;
		direction.y = 0.0f;
	}

	App->physics->UpdateDrag(direction);
}