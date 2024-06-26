#include "Game.h"
#include <windows.h>
#include <Xinput.h>
#include "Platform.h"
#include "Stage.h"
#include "Inputs.h"
using namespace std;

/*
Urgent:
	Fix speed changing after colliding while in stun

General:
	Allow controllers to navigate the start menu and pause
	Refactor Character.cpp with the new Inputs.h
	Give AI functionality with multiple stages by making them go towards the nearest platform when desperate and jump between platforms to get to the player
	
Fix 'parameters' in game.h:
	Make a moveTemplate class
	Make a CharacterStats class
	Clean UpdateCharacter's move creation with the new move templates
	

Tutorial:
	Put 1 player on screen as Circle
	Introduce different moves
	Add second player (uncontrollable) and output player 2's damage, vx and vy
	After that, make player 2 controllable and output player 2's controls
	Demonstrate different character stats
	After it's over, automatically set the menu to p2=ai as A and p1=human as circle. The player can decide if they wanna do that or not

Power ups:
	Power ups isn't bad, make additionalAcceleration, additionalKnockbackX etc, grab a few different sprites, make an item class (which stats it buffs, duration and colour) and add a line of the respective colour above a player with a power up

Items:
	Bind throw to dodge
	For each item make a light and heavy attack that works similarly to easy mode attacks
	Also add numUses
	Item is held directly infront of the character
*/

Game::Game(Graphics* gfx)
	:
	randomDist(0, 39),
	rng(rd())
{
	this->gfx = gfx;

	menuText = gfx->CreateTextFormat(L"Gabriola", 32.0f);

	//Load menu sprites
	easyModeWarning = new SpriteSheet(L"Easy Mode Warning.bmp", gfx);
	aiWarning = new SpriteSheet(L"AI Warning.bmp", gfx);
	creditsVisual = new SpriteSheet(L"Credits Menu.bmp", gfx);
	startVisual = new SpriteSheet(L"Start Menu.bmp", gfx);
	player1Win = new SpriteSheet(L"Player1Win.bmp", gfx);
	player2Win = new SpriteSheet(L"Player2Win.bmp", gfx);
	pauseVisual = new SpriteSheet(L"Pause Menu.bmp", gfx);
	startThree = new SpriteSheet(L"StartThree.bmp", gfx);
	startTwo = new SpriteSheet(L"StartTwo.bmp", gfx);
	startOne = new SpriteSheet(L"StartOne.bmp", gfx);
	go = new SpriteSheet(L"Go.bmp", gfx);

	WIPStageSelectVisual = new SpriteSheet(L"WIP STAGE SELECT.bmp", gfx);

	//Initialise idle sprites
	idleParameters[0] = L"CircleIdle.bmp";
	idleParameters[1] = L"aIdle.bmp";
	idleParameters[2] = L"SigmaMonkeyOfDoomIdle.bmp";
	idleParameters[3] = L"DogIdle.bmp";
	idleParameters[4] = L"ChickenIdle.bmp";
	idleParameters[5] = L"RockIdle.bmp";
	idleParameters[6] = L"TrollIdle.bmp";
	idleParameters[7] = L"BigBIdle.bmp";
	idleParameters[8] = L"Random.bmp";

	//Initialise move sprites
	moveParameters[0] = L"Circlemove.bmp";
	moveParameters[1] = L"aMove.bmp";
	moveParameters[2] = L"SigmaMonkeyOfDoomMove.bmp";
	moveParameters[3] = L"DogMove.bmp";
	moveParameters[4] = L"ChickenMove.bmp";
	moveParameters[5] = L"RockMove.bmp";
	moveParameters[6] = L"TrollMove.bmp";
	moveParameters[7] = L"BigBMove.bmp";

	//Initialise hit sprites
	hitParameters[0] = L"CircleHit.bmp";
	hitParameters[1] = L"aHit.bmp";
	hitParameters[2] = L"SigmaMonkeyOfDoomHit.bmp";
	hitParameters[3] = L"DogHit.bmp";
	hitParameters[4] = L"ChickenHit.bmp";
	hitParameters[5] = L"RockHit.bmp";
	hitParameters[6] = L"TrollHit.bmp";
	hitParameters[7] = L"BigBHit.bmp";

	//Initialise lives sprites
	livesIconParameters[0] = L"CircleLivesIcon.bmp";
	livesIconParameters[1] = L"aLivesIcon.bmp";
	livesIconParameters[2] = L"SigmaMonkeyOfDoomLivesIcon.bmp";
	livesIconParameters[3] = L"DogLivesIcon.bmp";
	livesIconParameters[4] = L"ChickenLivesIcon.bmp";
	livesIconParameters[5] = L"RockLivesIcon.bmp";
	livesIconParameters[6] = L"TrollLivesIcon.bmp";
	livesIconParameters[7] = L"BigBLivesIcon.bmp";

	//Initialise description sprites
	descParameters[0] = L"CircleDesc.bmp";
	descParameters[1] = L"aDesc.bmp";
	descParameters[2] = L"SigmaMonkeyOfDoomDesc.bmp";
	descParameters[3] = L"DogDesc.bmp";
	descParameters[4] = L"ChickenDesc.bmp";
	descParameters[5] = L"RockDesc.bmp";
	descParameters[6] = L"TrollDesc.bmp";
	descParameters[7] = L"BigBDesc.bmp";
	descParameters[8] = L"RandomDesc.bmp";

	//Initialise sounds
	mciSendStringA("open \"hit.avi\" type mpegvideo alias hit", NULL, 0, NULL);
	mciSendStringA("open \"Credits.wav\" type mpegvideo alias Credits", NULL, 0, NULL);
	mciSendStringA("open \"MainMenuTheme.wav\" type mpegvideo alias MainMenu", NULL, 0, NULL);

	//Play starting music
	mciSendStringA("play MainMenu repeat", NULL, 0, NULL);

	//Initialise menu idle sprites
	player1Idle = new SpriteSheet(idleParameters[player1CharacterID], gfx); //Set player 1s idle animation
	player2Idle = new SpriteSheet(idleParameters[player2CharacterID], gfx); //Set player 1s idle animation

	//Initialise menu description sprites
	player1Desc = new SpriteSheet(descParameters[player1CharacterID], gfx); //Set player 1s idle animation
	player2Desc = new SpriteSheet(descParameters[player2CharacterID], gfx); //Set player 1s idle animation
}

void Game::Go()
{
	GameState();
	ComposeFrame();
}

void Game::GameState()
{
	//Controller initialisation code
	ZeroMemory(&p1ControllerState, sizeof(XINPUT_STATE));
	if (p1Controller == -1) {
		DWORD dwResult;
		for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
		{
			// Simply get the state of the controller from XInput.
			dwResult = XInputGetState(i, &p1ControllerState);

			if (dwResult == ERROR_SUCCESS && i != p2Controller)
			{
				p1Controller = i;
			}
			else{
				ZeroMemory(&p2ControllerState, sizeof(XINPUT_STATE));
			}
		}
	}
	else {
		DWORD dwResult;
		dwResult = XInputGetState(p1Controller, &p1ControllerState);

		if (dwResult != ERROR_SUCCESS)
		{
			p1Controller = -1;
		}
	}

	ZeroMemory(&p2ControllerState, sizeof(XINPUT_STATE));
	if (p2Controller == -1) {
		DWORD dwResult;
		for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
		{
			// Simply get the state of the controller from XInput.
			dwResult = XInputGetState(i, &p2ControllerState);

			if (dwResult == ERROR_SUCCESS && i != p1Controller)
			{
				p2Controller = i;
			}
			else {
				ZeroMemory(&p2ControllerState, sizeof(XINPUT_STATE));
			}
		}
	}
	else {
		DWORD dwResult;
		dwResult = XInputGetState(p2Controller, &p2ControllerState);

		if (dwResult != ERROR_SUCCESS)
		{
			p2Controller = -1;
		}
	}

	clickPosition.x = -1;
	if (gfx->ifFocus() && GetKeyState(0x01) != clickHeld && 0x800) {
		clickHeld = GetKeyState(0x01);
		gfx->GetClickPosition(&clickPosition);
	}

	switch (menuState) {
		case (credits):
			if ((gfx->ifFocus() && GetKeyState(0x20) & 0x8000) ||
				(clickPosition.x >= 0 && clickPosition.x <= 205 && clickPosition.y >= 0 && clickPosition.y <= 65)) {
				if (!spaceHeld) {
					menuState = mainMenu;
					mciSendStringA("stop Credits", NULL, 0, NULL);
					mciSendStringA("play MainMenu from 0 repeat", NULL, 0, NULL);
					spaceHeld = true;
				}
			}
			else {
				spaceHeld = false;
			}
			break;
		case (mainMenu):
			StartMenu();
			break;
		case(stageSelect):
			StageSelectionMenu();
			break;
		case(game):
			if (!paused && timeUntilStart > 0) { //If the game is starting
				timeUntilStart--; //Reduce time until the game starts
			}
			else {
				GameLoop();
			}
			break;
	}
	enterOrEscapeHeld = gfx->ifFocus() && GetKeyState(0x0D) & 0x8000 || gfx->ifFocus() && GetKeyState(0x1B) & 0x8000; //If enter or escape is held set it to true, otherwise set it to false
	//spaceHeld = gfx->ifFocus() && GetKeyState(0x20) & 0x8000;
}

void Game::menuTransition()
{
	enterOrEscapeHeld = true;
	qHeld = true;
	eHeld = true;
	iHeld = true;
	pHeld = true;
	spaceHeld = true;
	tabHeld = true;
	backspaceHeld = true;
	clickHeld = true;
	shiftHeld = true;
}

void Game::StartMenu()
{
	if ((gfx->ifFocus() && GetKeyState(0xA2) & 0x8000 && gfx->ifFocus() && GetKeyState(0x20) & 0x8000 && !spaceHeld) ||
		(clickPosition.x >= 0 && clickPosition.x <= 400 && clickPosition.y >= 980 && clickPosition.y <= 1080)) { //If Fullscreen pressed
		spaceHeld = true;
		gfx->Fullscreen();
	}
	else if ((gfx->ifFocus() && GetKeyState(0x20) & 0x8000) ||
	(clickPosition.x >= 1520 && clickPosition.x <= 1920 && clickPosition.y >= 980 && clickPosition.y <= 1080)) { //If credits pressed
		if (!spaceHeld) {
			spaceHeld = true;
			menuState = credits;
			mciSendStringA("stop MainMenu", NULL, 0, NULL);
			mciSendStringA("play Credits from 0 repeat", NULL, 0, NULL);
			menuTransition();
		}
	}
	else {
		spaceHeld = false;
		if ((gfx->ifFocus() && GetKeyState(0x45) & 0x8000) ||
			(clickPosition.x >= 655 && clickPosition.x <= 745 && clickPosition.y >= 115 && clickPosition.y <= 210)) { //If Player 1 wants to increase their character ID
			if (player1CharacterID < 8 && !eHeld) {
				player1CharacterID++; //Increase it
				player1Idle -> ~SpriteSheet();
				player1Idle = new SpriteSheet(idleParameters[player1CharacterID], gfx); //Set player 1s idle animation
				player1Desc -> ~SpriteSheet();
				player1Desc = new SpriteSheet(descParameters[player1CharacterID], gfx); //Set player 1s description
				eHeld = true;
			}
		}
		else {
			eHeld = false;
		}

		if ((gfx->ifFocus() && GetKeyState(0x51) & 0x8000) ||
			(clickPosition.x >= 350 && clickPosition.x <= 440 && clickPosition.y >= 115 && clickPosition.y <= 210)) { //If Player 1 wants to decrease their character ID
			if (player1CharacterID > 0 && !qHeld) {
				player1CharacterID--; //Decrease it
				player1Idle -> ~SpriteSheet();
				player1Idle = new SpriteSheet(idleParameters[player1CharacterID], gfx); //Set player 1s idle animation
				player1Desc -> ~SpriteSheet();
				player1Desc = new SpriteSheet(descParameters[player1CharacterID], gfx); //Set player 1s description
				qHeld = true;
			}
		}
		else {
			qHeld = false;
		}

		if ((gfx->ifFocus() && GetKeyState(0x50) & 0x8000) ||
			(clickPosition.x >= 1485 && clickPosition.x <= 1570 && clickPosition.y >= 115 && clickPosition.y <= 210)) { //If Player 2 wants to increase their character ID
			if (player2CharacterID < 8 && !pHeld) {
				player2CharacterID++; //Incerase it
				player2Idle -> ~SpriteSheet();
				player2Idle = new SpriteSheet(idleParameters[player2CharacterID], gfx); //Set player 1s idle animation
				player2Desc -> ~SpriteSheet();
				player2Desc = new SpriteSheet(descParameters[player2CharacterID], gfx); //Set player 1s description
				pHeld = true;
			}
		}
		else {
			pHeld = false;
		}

		if ((gfx->ifFocus() && GetKeyState(0x49) & 0x8000) ||
			(clickPosition.x >= 1175 && clickPosition.x <= 1265 && clickPosition.y >= 115 && clickPosition.y <= 210)) { //If Player 2 wants to decrease their character ID
			if (player2CharacterID > 0 && !iHeld) {
				player2CharacterID--; //Decrease it
				player2Idle -> ~SpriteSheet();
				player2Idle = new SpriteSheet(idleParameters[player2CharacterID], gfx); //Set player 1s idle animation
				player2Desc -> ~SpriteSheet();
				player2Desc = new SpriteSheet(descParameters[player2CharacterID], gfx); //Set player 1s description
				iHeld = true;
			}
		}
		else {
			iHeld = false;
		}

		if ((gfx->ifFocus() && GetKeyState(0x09) & 0x8000) || //Tab
			(clickPosition.x >= 70 && clickPosition.x <= 170 && clickPosition.y >= 100 && clickPosition.y <= 130)) {
			if (!tabHeld) {
				p1AISelected = !p1AISelected;
				tabHeld = true;
			}
		}
		else {
			tabHeld = false;
		}

		if ((gfx->ifFocus() && GetKeyState(0x08) & 0x8000) || //Backsapce
			(clickPosition.x >= 1750 && clickPosition.x <= 1850 && clickPosition.y >= 100 && clickPosition.y <= 130)) {
			if (!backspaceHeld) {
				p2AISelected = !p2AISelected;
				backspaceHeld = true;
			}
		}
		else {
			backspaceHeld = false;
		}

		if ((gfx->ifFocus() && GetKeyState(0x10) & 0x8000) || //Shift
			(clickPosition.x >= 860 && clickPosition.x <= 1060 && clickPosition.y >= 0 && clickPosition.y <= 70)) {
			if (!shiftHeld) {
				easyMode = !easyMode;
				/* easy mode features:
					+1 double jump
					Walk acceleration / speed is the same as air acceleration / speed
					Free fall duration = 5 frames instead of 30 frames when touching the ground
					No invincibility
					No dodging
					No fast falling
					No directional attacks(pressing light always does forward light etc)
					Moves deal 2 / 3rds of their original stun
					All knockback is doubled
					Grounded jump height = aerial jump height

					Potential future plan:
					Slow all character speed (falling, vertical, horizontal)
				*/
				shiftHeld = true;
			}
		}
		else {
			shiftHeld = false;
		}

		if (((gfx->ifFocus() && GetKeyState(0x0D) & 0x8000 || gfx->ifFocus() && GetKeyState(0x1B) & 0x8000) && !enterOrEscapeHeld) ||
			(clickPosition.x >= 760 && clickPosition.x <= 1160 && clickPosition.y >= 490 && clickPosition.y <= 660)) { //If the game is starting
			menuState = stageSelect;
			if (player1CharacterID == 8) {
				player1CharacterID = 1 + (float)randomDist(rng) * 6.9f / 39;
				player1Idle = new SpriteSheet(idleParameters[player1CharacterID], gfx); //Set player 1s idle animation
				player1Desc -> ~SpriteSheet();
				player1Desc = new SpriteSheet(descParameters[player1CharacterID], gfx); //Set player 1s description
			}
			player1Move = new SpriteSheet(moveParameters[player1CharacterID], gfx); //Set player 1s move animation
			player1Hit = new SpriteSheet(hitParameters[player1CharacterID], gfx); //Set player 1s hit animation
			player1LivesIcon = new SpriteSheet(livesIconParameters[player1CharacterID], gfx); //Set player 2s lives icon
			Player1.Initialise(parameters[player1CharacterID]); //Set player 1s variables

			if (player2CharacterID == 8) {
				player2CharacterID = 1 + (float)randomDist(rng) * 6.9f / 39;
				player2Idle = new SpriteSheet(idleParameters[player2CharacterID], gfx); //Set player 1s idle animation
				player2Desc -> ~SpriteSheet();
				player2Desc = new SpriteSheet(descParameters[player2CharacterID], gfx); //Set player 1s description
			}
			player2Move = new SpriteSheet(moveParameters[player2CharacterID], gfx); //Set player 1s move animation
			player2Hit = new SpriteSheet(hitParameters[player2CharacterID], gfx); //Set player 1s hit animation
			player2LivesIcon = new SpriteSheet(livesIconParameters[player2CharacterID], gfx); //Set player 2s lives icon
			Player2.Initialise(parameters[player2CharacterID]); //Set player 1s variables
		}
	}
}

void Game::StageSelectionMenu()
{
	if (gfx->ifFocus() && GetKeyState(0x31) & 0x8000) {//1 key
		stageSelected = 0;
	}
	if (gfx->ifFocus() && GetKeyState(0x32) & 0x8000) {//2 key
		stageSelected = 1;
	}
	if (gfx->ifFocus() && GetKeyState(0x33) & 0x8000) {//3 key
		stageSelected = 2;
	}
	if (gfx->ifFocus() && GetKeyState(0x34) & 0x8000) {//4 key
		stageSelected = 3;
	}
	if (gfx->ifFocus() && GetKeyState(0x35) & 0x8000) {//5 key
		stageSelected = 4;
	}
	if (gfx->ifFocus() && GetKeyState(0x36) & 0x8000) {//6 key
		stageSelected = 5;
	}
	if (gfx->ifFocus() && GetKeyState(0x37) & 0x8000) {//7 key
		stageSelected = 6;
	}
	if (gfx->ifFocus() && GetKeyState(0x38) & 0x8000) {//8 key
		stageSelected = 7;
	}
	if (gfx->ifFocus() && GetKeyState(0x39) & 0x8000) {//9 key
		stageSelected = 8;
	}
	if (stageSelected != -1) {
		GameStart();
		menuTransition();
		menuState = game;
	}
}

void Game::StartBattleTheme()
{
	mciSendStringA("stop MainMenu", NULL, 0, NULL);
	mciSendStringA("stop BattleTheme", NULL, 0, NULL);
	mciSendStringA("close BattleTheme", NULL, 0, NULL);
	mciSendStringA(Stages[stageSelected].loadMusic, NULL, 0, NULL);
	mciSendStringA("play BattleTheme from 0 repeat", NULL, 0, NULL);
}

void Game::GameLoop()
{
	if (paused) {
		if ((gfx->ifFocus() && GetKeyState(0xA2) & 0x8000 && !enterOrEscapeHeld && (gfx->ifFocus() && GetKeyState(0x0D) & 0x8000 || gfx->ifFocus() && GetKeyState(0x1B) & 0x8000)) ||
			(clickPosition.x >= 1620 && clickPosition.x <= 1920 && clickPosition.y >= 0 && clickPosition.y <= 200)) {
			paused = false;
			GameEnd();
			previousWinner = 0;
			mciSendStringA("stop BattleTheme0", NULL, 0, NULL);
			mciSendStringA("stop BattleTheme1", NULL, 0, NULL);
			mciSendStringA("stop BattleTheme2", NULL, 0, NULL);
			mciSendStringA("stop BattleTheme3", NULL, 0, NULL);
			mciSendStringA("play MainMenu from 0 repeat", NULL, 0, NULL);
		}
		else if ((gfx->ifFocus() && GetKeyState(0xA2) & 0x8000 && gfx->ifFocus() && GetKeyState(0x52) & 0x8000) ||
			(clickPosition.x >= 0 && clickPosition.x <= 300 && clickPosition.y >= 0 && clickPosition.y <= 200)) { //If the game should be restarted
			GameStart();
		}
		else if (!enterOrEscapeHeld && (gfx->ifFocus() && GetKeyState(0x0D) & 0x8000 || gfx->ifFocus() && GetKeyState(0x1B) & 0x8000)) { //If the pause button is pressd
			paused = false; //Unpause the game
		}
	}
	else if (timer == 0) {
		GameEnd();
	}
	else if (Player1.IsAlive(1920, 1080, leniancy) && Player2.IsAlive(1920, 1080, leniancy)) {//If both players are alive
		timer--;
		if (enterOrEscapeHeld == false && (gfx->ifFocus() && GetKeyState(0x0D) & 0x8000 || gfx->ifFocus() && GetKeyState(0x1B) & 0x8000)) { //If the pause button is pressed
			timeUntilStart = 180; //Initialise the countdown
			timeGoIsDisplayed = 60; //Initialise go
			paused = true; //Pause the game
		}

		if (timeGoIsDisplayed > 0) { //If go should be displayed
			timeGoIsDisplayed--; //Reduce the time go should be displayed
		}

		//Update models
		if (p1AISelected) {
			ArtifialFriend.Update(Player2.x, Player2.y, Player2.width, Player2.height, Player1.x, Player1.y, Player1.vx, Player1.vy, Player1.width, Player1.height, Player1.invincibilityCooldown == 0, Player1.doubleJump, Stages[stageSelected].Platforms, randomDist(rng), Player2.playerPercentage);
			Player1.UpdateCharacter(Inputs(
				ArtifialFriend.left, //Left
				ArtifialFriend.right, //Right
				ArtifialFriend.up, //Up
				ArtifialFriend.down, //Down
				ArtifialFriend.jump, //Jump
				ArtifialFriend.light, //Light
				ArtifialFriend.heavy, //Heavy
				ArtifialFriend.special, //Special
				ArtifialFriend.dodge //Dodge
				),
				Stages[stageSelected].Platforms);
		}
		else {
			p1StopOtherInputs = false;
			float RX = p1ControllerState.Gamepad.sThumbRX / controllerStickMaxInput;
			float RY = p1ControllerState.Gamepad.sThumbRY / controllerStickMaxInput;
			if (!p1RightStickPressed && (RX > 0.2 || RX < -0.2 || RY > 0.2 || RY < -0.2) && Player1.stun == 0 && Player1.moveDuration == 0) {
				p1StopOtherInputs = true;
			}
			p1RightStickPressed = (RX > 0.2 || RX < -0.2 || RY > 0.2 || RY < -0.2) && ((Player1.stun == 0 && Player1.moveDuration == 0) || p1RightStickPressed);
			Player1.UpdateCharacter(Inputs(
				gfx->ifFocus() && (GetKeyState(0x41) & 0x8000) || ((((p1ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) || (p1ControllerState.Gamepad.sThumbLX / controllerStickMaxInput < -0.2)) && !p1StopOtherInputs) || (p1StopOtherInputs && RX < -0.2)), //Left
				gfx->ifFocus() && (GetKeyState(0x44) & 0x8000) || ((((p1ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) || (p1ControllerState.Gamepad.sThumbLX / controllerStickMaxInput > 0.2)) && !p1StopOtherInputs) || (p1StopOtherInputs && RX > 0.2)), //Right
				gfx->ifFocus() && (GetKeyState(0x57) & 0x8000) || ((((p1ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) || (p1ControllerState.Gamepad.sThumbLY / controllerStickMaxInput > 0.2)) && !p1StopOtherInputs) || (p1StopOtherInputs && RY > 0.2)), //Up
				gfx->ifFocus() && (GetKeyState(0x53) & 0x8000) || ((((p1ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) || (p1ControllerState.Gamepad.sThumbLY / controllerStickMaxInput < -0.2)) && !p1StopOtherInputs) || (p1StopOtherInputs && RY < -0.2)), //Down
				gfx->ifFocus() && (GetKeyState(0x47) & 0x8000) || ((p1ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !p1StopOtherInputs), //Jump
				gfx->ifFocus() && (GetKeyState(0x46) & 0x8000) || ((p1ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_X) || p1StopOtherInputs), //Light
				gfx->ifFocus() && (GetKeyState(0x54) & 0x8000) || ((p1ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_B || p1ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) && !p1StopOtherInputs), //Heavy
				gfx->ifFocus() && (GetKeyState(0x48) & 0x8000) || ((p1ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_Y || p1ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) && !p1StopOtherInputs), //Special
				gfx->ifFocus() && (GetKeyState(0xA0) & 0x8000) || ((p1ControllerState.Gamepad.bLeftTrigger / 255 > 0.1 || p1ControllerState.Gamepad.bRightTrigger / 255 > 0.1) && !p1StopOtherInputs) //Dodge
				),
				Stages[stageSelected].Platforms);
		}
		
		if (p2AISelected) {
			ArtifialFriend.Update(Player1.x, Player1.y, Player1.width, Player1.height, Player2.x, Player2.y, Player2.vx, Player2.vy, Player2.width, Player2.height, Player2.invincibilityCooldown == 0, Player2.doubleJump, Stages[stageSelected].Platforms, randomDist(rng), Player1.playerPercentage);
			Player2.UpdateCharacter(Inputs(
				ArtifialFriend.left, //Left
				ArtifialFriend.right, //Right
				ArtifialFriend.up, //Up
				ArtifialFriend.down, //Down
				ArtifialFriend.jump, //Jump
				ArtifialFriend.light, //Light
				ArtifialFriend.heavy, //Heavy
				ArtifialFriend.special, //Special
				ArtifialFriend.dodge //Dodge
				),
				Stages[stageSelected].Platforms);
		}
		else {
			p2StopOtherInputs = false;
			float RX = p2ControllerState.Gamepad.sThumbRX / controllerStickMaxInput;
			float RY = p2ControllerState.Gamepad.sThumbRY / controllerStickMaxInput;
			if (!p2RightStickPressed && (RX > 0.2 || RX < -0.2 || RY > 0.2 || RY < -0.2) && Player2.stun == 0 && Player2.moveDuration == 0) {
				p2StopOtherInputs = true;
			}
			p2RightStickPressed = (RX > 0.2 || RX < -0.2 || RY > 0.2 || RY < -0.2) && ((Player2.stun == 0 && Player2.moveDuration == 0) || p2RightStickPressed);
			
			Player2.UpdateCharacter(Inputs(
				gfx->ifFocus() && (GetKeyState(0x25) & 0x8000) || ((((p2ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) || (p2ControllerState.Gamepad.sThumbLX / controllerStickMaxInput < -0.2)) && !p2StopOtherInputs) || (p2StopOtherInputs && RX < -0.2)), //Left
				gfx->ifFocus() && (GetKeyState(0x27) & 0x8000) || ((((p2ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) || (p2ControllerState.Gamepad.sThumbLX / controllerStickMaxInput > 0.2)) && !p2StopOtherInputs) || (p2StopOtherInputs && RX > 0.2)), //Right
				gfx->ifFocus() && (GetKeyState(0x26) & 0x8000) || ((((p2ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) || (p2ControllerState.Gamepad.sThumbLY / controllerStickMaxInput > 0.2)) && !p2StopOtherInputs) || (p2StopOtherInputs && RY > 0.2)), //Up
				gfx->ifFocus() && (GetKeyState(0x28) & 0x8000) || ((((p2ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) || (p2ControllerState.Gamepad.sThumbLY / controllerStickMaxInput < -0.2)) && !p2StopOtherInputs) || (p2StopOtherInputs && RY < -0.2)), //Down
				gfx->ifFocus() && (GetKeyState(0x4C) & 0x8000) || ((p2ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !p2StopOtherInputs), //Jump
				gfx->ifFocus() && (GetKeyState(0x4B) & 0x8000) || ((p2ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_X) || p2StopOtherInputs), //Light
				gfx->ifFocus() && (GetKeyState(0x4F) & 0x8000) || ((p2ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_B || p2ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) && !p2StopOtherInputs), //Heavy
				gfx->ifFocus() && (GetKeyState(0xBA) & 0x8000) || ((p2ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_Y || p2ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) && !p2StopOtherInputs), //Special
				gfx->ifFocus() && (GetKeyState(0x4E) & 0x8000) || ((p2ControllerState.Gamepad.bLeftTrigger / 255 > 0.1 || p2ControllerState.Gamepad.bRightTrigger / 255 > 0.1) && !p2StopOtherInputs) //Dodge
				),
				Stages[stageSelected].Platforms);
		}

		if (Player1.IsMoveColliding(Player2.x, Player2.y, Player2.width, Player2.height)) { //Is player 1 hitting any move
			std::string soundString = "set hit speed " + std::to_string(2100 - 100 * (int)Player1.GetMove(Player2.moveThatHit).damage);
			mciSendStringA(soundString.c_str(), NULL, 0, NULL);
			mciSendStringA("play hit from 0", NULL, 0, NULL);
			Player2.IsHit(Player1.GetMove(Player1.moveThatHit)); //Register that player 2 has been hit
		}
		if (Player2.IsMoveColliding(Player1.x, Player1.y, Player1.width, Player1.height)) { //Is player 2 hitting any move
			std::string soundString = "set hit speed " + std::to_string(2100 - 100 * (int)Player2.GetMove(Player2.moveThatHit).damage);
			mciSendStringA(soundString.c_str(), NULL, 0, NULL);
			Player1.IsHit(Player2.GetMove(Player2.moveThatHit)); //Register that player 1 has been hit
		}
	}
	else {
		GameEnd();
	}
}

void Game::GameStart()
{
	menuTransition();
	timer = 10 * 60 * 60 - 1;
	StartBattleTheme();
	Player1.easyMode = easyMode;
	Player1.x = Stages[stageSelected].spawnx; //Set player 1s starting position
	Player1.y = Stages[stageSelected].spawny - Player1.height; //Set player 1s starting position
	Player1.facingRight = true;
	Player1.Restart();
	Player2.easyMode = easyMode;
	Player2.x = 1920 - Stages[stageSelected].spawnx - Player2.width; //Set player 2s starting position
	Player2.y = Stages[stageSelected].spawny - Player2.height; //Set player 2s starting position
	Player2.facingRight = false;
	Player2.Restart();
	timeUntilStart = 180;
	timeGoIsDisplayed = 60;
	paused = false;
}

void Game::GameEnd()
{
	menuTransition();
	menuState = mainMenu;
	stageSelected = -1; //Delects the current stage
	mciSendStringA("pause BattleTheme", NULL, 0, NULL);
	mciSendStringA("close BattleTheme", NULL, 0, NULL);
	mciSendStringA("play MainMenu from 0 repeat", NULL, 0, NULL);
	player1Hit -> ~SpriteSheet();
	player2Hit -> ~SpriteSheet();
	player1Move -> ~SpriteSheet();
	player2Move -> ~SpriteSheet();
	player1LivesIcon -> ~SpriteSheet();
	player2LivesIcon -> ~SpriteSheet();
	if (Player2.lives < Player1.lives || (Player1.lives == Player2.lives && Player1.playerPercentage < Player2.playerPercentage)) { //Set the previous winner
		previousWinner = 1;
	}
	else if (Player1.lives < Player2.lives || (Player1.lives == Player2.lives && Player2.playerPercentage < Player1.playerPercentage)) {
		previousWinner = 2;
	}
	else {
		previousWinner = 0;
	}
}

int* Game::ShrinkToCoordinates(float x0, float y0, float x1, float y1, int screenx0, int screeny0, int screenx1, int screeny1) {
	float refactorx = (screenx1 - screenx0) / 1920.0f;
	float refactory = (screeny1 - screeny0) / 1080.0f;

	int a[4];

	a[0] = screenx0 + x0 * refactorx;
	a[1] = screeny0 + y0 * refactory;
	a[2] = screenx0 + x1 * refactorx;
	a[3] = screeny0 + y1 * refactory;

	return a;
}

wstring Game::DamageToWString(float damage)
{
	string intToString = to_string((int)(damage * 10));
	while (intToString.length() < 3) {
		intToString = "0" + intToString;
	}
	intToString = intToString.substr(0, 2) + "." + intToString.substr(2, 1);
	return wstring(intToString.begin(), intToString.end());
}

wstring Game::TimeToWString(int time)
{
	string intToString = to_string((int)(timer / 60 / 60)) + ":" + to_string((int)(timer / 60 % 60));
	return wstring(intToString.begin(), intToString.end());
}

void Game::ComposeFrame()
{
	gfx->BeginDraw();
	gfx->ClearScreen(0, 0, 0);
	switch (menuState) {
		case(credits):
			/*
			Song credits:
			Post-Adventure Tea Party, Zane Little, Free Music Archive, CC BY-NC
			Terrible song in 100 bpm, Lundstroem, Free Music Archive ,CC BY-NC
			All Star, Beat Mekanik, Free Music Archive, CC BY-NC
			Sinister Abode, Zane Little, Free Music Archive, CC BY-NC
			The March Of The Dragon Hunters, Kathrin Klimek, Free Music Archive, CC BY-NC
			Regeneration, Audioezout, Free Music Archive, CC BY-NC

			Character Credits:
			Slot 1 (Circle), Original
			Slot 2 (a), Sprites designed and drawn by Henry Exworthy
			Slot 3 (Sigma Monkey of Doom), Sprites designed and drawn by anonymous
			Slot 4 (Dog), Visual concept by Max Sullivan
			Slot 5 (Chicken), Visual concept by Elliot Ody, Moveset design by anonymous
			Slot 6 (Rock), Concept by Joshua Thorpe
			Slot 7 (Troll), Sprites designed and drawn by Arthur
			Slot 8 (Big B), Sprites designed and drawn by Elliot Ody, Moveset design by Elliot Ody

			Coding:
			Original framework - https://github.com/planetchili/chili_framework
			GDI+ model - https://learn.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-gdi-start
			DirectX model - https://youtube.com/playlist?list=PLKK11Ligqitij8r6hd6tfqqesh3T_xWJA&si=0ODR_FpQVGcQTN7k
			*/
			creditsVisual->Draw(0, 0, false);
			break;
		case(mainMenu):
			//startVisual->Draw(0, 0, false); //Displays the visual for the start menu
			gfx->DrawText(L"Player 1 \n Move with WASD \n Jump with G \n Light attack with F \n Heavy attack with T \n Special attack with H \n Dodge with Left Shift \n Pause with ESC", menuText, 140, 500, 0, 0, 255, 1);
			gfx->DrawText(L"Player 2 \n Move with arrow keys \n Jump with L \n Light attack with K \n Heavy attack with O \n Special attack with ; \n Dodge with N \n Pause with Enter", menuText, 1400, 500, 255, 0, 0, 1);

			if (p1AISelected) {
				aiWarning->Draw(70, 100, false);
			}
			if (p2AISelected) {
				aiWarning->Draw(1750, 100, false);
			}

			if (easyMode) {
				easyModeWarning->Draw(860, 0, false);
				gfx->DrawRectFill(130, 730, 410, 760, 0, 0, 0, 1);
				gfx->DrawRectFill(1400, 730, 1571, 762, 0, 0, 0, 1);
			}

			if (previousWinner == 1) { //If Player 1 won the last match
				player1Win->Draw(1920 / 2 - 250, 100, false); //Dislpay a sprite that shows this
			}
			else if (previousWinner == 2) { //If player 2 won the last match
				player2Win->Draw(1920 / 2 - 250, 100, false); //Dislpay a sprite that shows this
			}

			player1Idle->Draw(550 - parameters[player1CharacterID][0] / 2, 100, false); //Draws an appropriate sprite based on character ID in the start menu
			player2Idle->Draw(1370 - parameters[player2CharacterID][0] / 2, 100, player2CharacterID != 8); //Draws an appropriate sprite based on character ID in the start menu

			player1Desc->Draw(345, 250, false); //Draws an appropriate sprite based on character ID in the start menu
			player2Desc->Draw(1175, 250, false); //Draws an appropriate sprite based on character ID in the start menu
			break;
		case (stageSelect):
			WIPStageSelectVisual->Draw(0, 0, false);
			for (int i = 0; i < 9; i++) {
				int boxx0 = 1920 / 4 * (1 + i % 3) - 192 / 2;
				int boxy0 = 1080 / 4 * (1 + i / 3 - i % 3 / 3) - 108 / 2;
				int boxx1 = 1920 / 4 * (1 + i % 3) + 192 / 2;
				int boxy1 = 1080 / 4 * (1 + i / 3 - i % 3 / 3) + 108 / 2;
				gfx->DrawRectThin(boxx0, boxy0, boxx1, boxy1, 255, 255, 255, 1);
				for (int j = 0; j < 10; j++) {
					//Assume 192 * 108 boxes
					int* shrunkCoordinates;

					shrunkCoordinates = ShrinkToCoordinates(Stages[i].Platforms[j].x0, Stages[i].Platforms[j].y0, Stages[i].Platforms[j].x1, Stages[i].Platforms[j].y1, boxx0, boxy0, boxx1, boxy1);

					if (Stages[i].Platforms[j].isFilled) {
						gfx->DrawRectFill(shrunkCoordinates[0], shrunkCoordinates[1], shrunkCoordinates[2], shrunkCoordinates[3], Stages[i].Platforms[j].r, Stages[i].Platforms[j].g, Stages[i].Platforms[j].b, 1); //Draw the platform
					}
					else {
						gfx->DrawRectThin(shrunkCoordinates[0], shrunkCoordinates[1], shrunkCoordinates[2], shrunkCoordinates[3], Stages[i].Platforms[j].r, Stages[i].Platforms[j].g, Stages[i].Platforms[j].b, 1); //Draw the platform
					}
				}
			}
			break;
		case(game):
			if (easyMode) {
				easyModeWarning->Draw(0, 0, false);
			}
			if (paused) {
				pauseVisual->Draw(0, 0, false); //Display the pause menu

				if (easyMode) {
					gfx->DrawRectFill(0, 1050, 270, 1080, 0, 0, 0, 1);
					gfx->DrawRectFill(1630, 1050, 1920, 1080, 0, 0, 0, 1);
				}

				if (Player1.lives == Player2.lives && Player1.playerPercentage == Player2.playerPercentage) { //If both players are even
					gfx->DrawRectFill(1920 / 2 - Player1.width - 1, 99, 1920 / 2 + 1, 100 + Player1.height + 1, 255, 199, 0, 1); //Player 1 border
					player1Idle->Draw(1920 / 2 - Player1.width, 100, false); //Display player 1 at the top of the screen
					gfx->DrawRectFill(1920 / 2, 99, 1920 / 2 + Player2.width + 1, 100 + Player2.height + 1, 255, 199, 0, 1); //Player 2 border
					player2Idle->Draw(1920 / 2, 100, false); //Display player 2 at the top of the screen
				}
				else if (Player1.lives > Player2.lives || (Player1.lives == Player2.lives && Player1.playerPercentage < Player2.playerPercentage)) {
					gfx->DrawRectFill(1920 / 4 - 1, 349, 1920 / 4 + Player1.width + 1, 351 + Player1.height, 255, 199, 0, 1); //Player 1 border
					player1Idle->Draw(1920 / 4, 350, false); //Display player 1 on the left side
					gfx->DrawRectFill(1920 / 4 * 3 - 1, 349, 1920 / 4 * 3 + Player2.width + 1, 351 + Player2.height, 255, 199, 0, 1); //Player 2 border
					player2Hit->Draw(1920 / 4 * 3, 350, false); //Display player 2 on the right side
				}
				else {
					gfx->DrawRectFill(1920 / 4 - 1, 349, 1920 / 4 + Player2.width + 1, 351 + Player2.height, 255, 199, 0, 1); //Player 1 border
					player2Idle->Draw(1920 / 4, 350, false); //Display player 2 on the left side
					gfx->DrawRectFill(1920 / 4 * 3 - 1, 349, 1920 / 4 * 3 + Player1.width + 1, 351 + Player1.height, 255, 199, 0, 1); //Player 2 border
					player1Hit->Draw(1920 / 4 * 3, 350, false); //Display player 1 on the right side
				}
			}

			else if (timeUntilStart > 120) { //If you should display 3
				startThree->Draw(1920 / 2 - 150, 1080 / 4 - 150, false); //Display 3
			}
			else if (timeUntilStart > 60) { //If you should display 2
				startTwo->Draw(1920 / 2 - 150, 1080 / 4 - 150, false); //Display 2
			}
			else if (timeUntilStart > 0) { //If you should display 1
				startOne->Draw(1920 / 2 - 150, 1080 / 4 - 150, false); //Display 1
			}
			else if (timeGoIsDisplayed > 0) { //If you should display go
				go->Draw(1920 / 2 - 150, 1080 / 4 - 150, false); //Display go
			}

			for (int i = 0; i < 9; i++) { //For each platform
				Platform platformToDraw = Stages[stageSelected].Platforms[i];
				if (platformToDraw.isFilled) {
					gfx->DrawRectFill(platformToDraw.x0, platformToDraw.y0, platformToDraw.x1, platformToDraw.y1, platformToDraw.r, platformToDraw.g, platformToDraw.b, 1); //Draw the platform
				}
				else {
					gfx->DrawRectThin(platformToDraw.x0, platformToDraw.y0, platformToDraw.x1, platformToDraw.y1, platformToDraw.r, platformToDraw.g, platformToDraw.b, 1); //Draw the platform
				}
			}

			gfx->DrawTextW(DamageToWString(Player1.playerPercentage).c_str(), menuText, 1920 / 4, 900, 0, 0, 255, 1);

			for (int i = 0; i < Player1.lives; i++) {
				player1LivesIcon->Draw(1920 / 4 + i * 30, 950, false); //Player 1 lives icon
			}
			for (int i = 0; i < Player2.lives; i++) {
				player2LivesIcon->Draw(1920 / 4 * 3 + i * 30, 950, false); //Player 2 lives icon
			}

			gfx->DrawTextW(DamageToWString(Player2.playerPercentage).c_str(), menuText, 1920 / 4 * 3, 900, 0, 0, 255, 1);

			if (p1AISelected) {
				aiWarning->Draw(1920 / 4, 980, false);
			}
			if (p2AISelected) {
				aiWarning->Draw(1920 / 4 * 3, 980, false);
			}

			if (Player1.invincibility > 0) { //If player 1 has invincibility
				gfx->DrawRectThin(Player1.x - 1, Player1.y - 1, Player1.x + Player1.width + 1, Player1.y + Player1.height + 1, 0, 237, 255, 1); //Give them a border
			}
			if (Player2.invincibility > 0) { //If player 2 has invincibility
				gfx->DrawRectThin(Player2.x - 1, Player2.y - 1, Player2.x + Player2.width + 1, Player2.y + Player2.height + 1, 0, 237, 255, 1); //Give them a border
			}

			if (Player1.moveDuration > 0 || Player1.freeFallDuration > 0) { //If Player 1 is using a move
				player1Move->Draw(Player1.x, Player1.y, !Player1.facingRight); //Display normal animation
			}
			else if (Player1.stun > 0) { //If player 1 is in stun
				player1Hit->Draw(Player1.x, Player1.y, !Player1.facingRight); //Display normal animation
			}
			else {
				player1Idle->Draw(Player1.x, Player1.y, !Player1.facingRight); //Display normal animation
			}

			if (Player2.moveDuration > 0 || Player2.freeFallDuration > 0) { //If Player 2 is using a move
				player2Move->Draw(Player2.x, Player2.y, !Player2.facingRight); //Display normal animation
			}
			else if (Player2.stun > 0) { //If player 2 is in stun
				player2Hit->Draw(Player2.x, Player2.y, !Player2.facingRight); //Display normal animation
			}
			else {
				player2Idle->Draw(Player2.x, Player2.y, !Player2.facingRight); //Display normal animation
			}

			Move moveToDraw;
			for (int i = 0; i < Player1.moveArrayLength; i++) { //For every move
				moveToDraw = Player1.GetMove(i);
				if (moveToDraw.activeDuration >= 0 && moveToDraw.startUpDuration < 0) {
					gfx->DrawRectFill(moveToDraw.x + moveToDraw.additionalX, moveToDraw.y + moveToDraw.additionalY, moveToDraw.x + moveToDraw.additionalX + moveToDraw.width, moveToDraw.y + moveToDraw.additionalY + moveToDraw.height, moveToDraw.r, moveToDraw.g, moveToDraw.b, 1); //Draw it
				}
				moveToDraw = Player2.GetMove(i);
				if (moveToDraw.activeDuration >= 0 && moveToDraw.startUpDuration < 0) {
					gfx->DrawRectFill(moveToDraw.x + moveToDraw.additionalX, moveToDraw.y + moveToDraw.additionalY, moveToDraw.x + moveToDraw.additionalX + moveToDraw.width, moveToDraw.y + moveToDraw.additionalY + moveToDraw.height, moveToDraw.r, moveToDraw.g, moveToDraw.b, 1); //Draw it
				}
			}

			gfx->DrawTextW(TimeToWString(timer).c_str(), menuText, 1920 / 2 - 50, 50, 255, 255, 255, 1);
			break;
	}

	gfx->EndDraw();
}