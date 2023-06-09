/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include <random>
#include "SpriteEffect.h"


Game::Game(MainWindow& mainWindow)
	:
	wnd(mainWindow),
	gfx(mainWindow),
	MainMenuTheme(L"MainMenuTheme.wav", 0.0f, 200.0f),
	HitSound(L"hit.wav"),
	CreditsTheme(L"Credits.wav", 0.0f, 225.0f),
	BattleTheme0(L"Battle Theme 0.wav", 0.0f, 236.0f),
	BattleTheme1(L"Battle Theme 1.wav", 0.0f, 232.0f),
	BattleTheme2(L"Battle Theme 2.wav", 0.0f, 147.0f),
	BattleTheme3(L"Battle Theme 3.wav", 0.0f, 159.0f),
	musicDist(0,3.9),
	aiMoveDist(0,3.9)
{
	//Initialise the numbers used for the percentage display
	numbers[0] = { "Images\\0.bmp" }; 
	numbers[1] = { "Images\\1.bmp" };
	numbers[2] = { "Images\\2.bmp" };
	numbers[3] = { "Images\\3.bmp" };
	numbers[4] = { "Images\\4.bmp" };
	numbers[5] = { "Images\\5.bmp" };
	numbers[6] = { "Images\\6.bmp" };
	numbers[7] = { "Images\\7.bmp" };
	numbers[8] = { "Images\\8.bmp" };
	numbers[9] = { "Images\\9.bmp" };

	//Initialise idle sprites
	idle[0] = { "Images\\CircleIdle.bmp" };
	idle[1] = { "Images\\aIdle.bmp" };
	idle[2] = { "Images\\SigmaMonkeyOfDoomIdle.bmp" };
	idle[3] = { "Images\\DogIdle.bmp" };
	idle[4] = { "Images\\ChickenIdle.bmp" };
	idle[5] = { "Images\\RockIdle.bmp" };
	idle[6] = { "Images\\TrollIdle.bmp" };
	idle[7] = { "Images\\BigBIdle.bmp" };

	//Initialise move sprites
	move[0] = { "Images\\Circlemove.bmp" };
	move[1] = { "Images\\aMove.bmp" };
	move[2] = { "Images\\SigmaMonkeyOfDoomMove.bmp" };
	move[3] = { "Images\\DogMove.bmp" };
	move[4] = { "Images\\ChickenMove.bmp" };
	move[5] = { "Images\\RockMove.bmp" };
	move[6] = { "Images\\TrollMove.bmp" };
	move[7] = { "Images\\BigBMove.bmp" };

	//Initialise hit sprites
	hit[0] = {"Images\\CircleHit.bmp"};
	hit[1] = { "Images\\aHit.bmp" };
	hit[2] = { "Images\\SigmaMonkeyOfDoomHit.bmp" };
	hit[3] = { "Images\\DogHit.bmp" };
	hit[4] = { "Images\\ChickenHit.bmp" };
	hit[5] = { "Images\\RockHit.bmp" };
	hit[6] = { "Images\\TrollHit.bmp" };
	hit[7] = { "Images\\BigBHit.bmp" };

	//Initialise lives sprites
	lives[0] = {"Images\\CircleLivesIcon.bmp"};
	lives[1] = { "Images\\aLivesIcon.bmp" };
	lives[2] = { "Images\\SigmaMonkeyOfDoomLivesIcon.bmp" };
	lives[3] = { "Images\\DogLivesIcon.bmp" };
	lives[4] = { "Images\\ChickenLivesIcon.bmp" };
	lives[5] = { "Images\\RockLivesIcon.bmp" };
	lives[6] = { "Images\\TrollLivesIcon.bmp" };
	lives[7] = { "Images\\BigBLivesIcon.bmp" };
}

void Game::Go()
{
	gfx.BeginFrame();	
	GameState();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::GameState()
{
	if (credits) {
		if ((wnd.kbd.KeyIsPressed(VK_RETURN) || wnd.kbd.KeyIsPressed(VK_ESCAPE) && !enterOrEscapeHeld) || (wnd.kbd.KeyIsPressed(VK_SPACE) && !spaceHeld)) {
			credits = false;
			CreditsTheme.StopAll();
		}
	}
	else if (startMenu) {
		StartMenu();
	}
	else if (!paused && timeUntilStart > 0) { //If the game is starting
		timeUntilStart--; //Reduce time until the game starts
	}
	else {
		GameLoop();
	}
	enterOrEscapeHeld = wnd.kbd.KeyIsPressed(VK_RETURN) || wnd.kbd.KeyIsPressed(VK_ESCAPE); //If enter or escape is held set it to true, otherwise set it to false
	spaceHeld = wnd.kbd.KeyIsPressed(VK_SPACE);
}

void Game::StartMenu()
{
	if (wnd.kbd.KeyIsPressed(VK_CONTROL) && wnd.kbd.KeyIsPressed(VK_SPACE)) {
		abort();
	}
	else if (wnd.kbd.KeyIsPressed(VK_SPACE) && !spaceHeld) {
		credits = true;
		MainMenuTheme.StopAll();
		mainMenuThemeIsPlaying = false;
		CreditsTheme.Play();
	}
	else {
		if (!mainMenuThemeIsPlaying) {
			MainMenuTheme.Play();
			mainMenuThemeIsPlaying = true;
		}
		if (wnd.kbd.KeyIsPressed(0x45) && player1CharacterID < 7 && !eHeld) { //If Player 1 wants to increase their character ID
			player1CharacterID++; //Increase it
		}
		if (wnd.kbd.KeyIsPressed(0x51) && player1CharacterID > 0 && !qHeld) { //If Player 1 wants to decrease their character ID
			player1CharacterID--; //Decrease it
		}
		if (wnd.kbd.KeyIsPressed(0x50) && player2CharacterID < 7 && !pHeld) { //If Player 2 wants to increase their character ID
			player2CharacterID++; //Incerase it
		}
		if (wnd.kbd.KeyIsPressed(0x49) && player2CharacterID > 0 && !iHeld) { //If Player 2 wants to decrease their character ID
			player2CharacterID--; //Decrease it
		}

		if (wnd.kbd.KeyIsPressed(VK_TAB) && !tabHeld) {
			AISelected = !AISelected;
		}

		qHeld = wnd.kbd.KeyIsPressed(0x51); //If q is held set it to true, otherwise set it to false
		eHeld = wnd.kbd.KeyIsPressed(0x45); //If e is held set it to true, otherwise set it to false
		iHeld = wnd.kbd.KeyIsPressed(0x49); //If i is held set it to true, otherwise set it to false
		pHeld = wnd.kbd.KeyIsPressed(0x50); //If p is held set it to true, otherwise set it to false
		tabHeld = wnd.kbd.KeyIsPressed(VK_TAB);

		if ((wnd.kbd.KeyIsPressed(VK_RETURN) || wnd.kbd.KeyIsPressed(VK_ESCAPE)) && !enterOrEscapeHeld) { //If the game is starting
			startMenu = false; //Disable the start menu
			GameStart();
			player1Idle = idle[player1CharacterID]; //Set player 1's idle animation
			player1Move = move[player1CharacterID]; //Set player 1's move animation
			player1Hit = hit[player1CharacterID]; //Set player 1's hit animation
			player1LivesIcon = lives[player1CharacterID]; //Set player 1's lives icon
			Player1.Initialise(parameters[player1CharacterID], stageX0, stageY0, stageX1, stageY1); //Set player 1's variables

			
			player2Idle = idle[player2CharacterID]; //Set player 1's idle animation
			player2Move = move[player2CharacterID]; //Set player 1's move animation
			player2Hit = hit[player2CharacterID]; //Set player 1's hit animation
			player2LivesIcon = lives[player2CharacterID]; //Set player 2's lives icon
			Player2.Initialise(parameters[player2CharacterID], stageX0, stageY0, stageX1, stageY1); //Set player 1's variables
		}
	}
}

void Game::StartBattleTheme()
{
	BattleTheme0.StopAll();
	BattleTheme1.StopAll();
	BattleTheme2.StopAll();
	BattleTheme3.StopAll();
	MainMenuTheme.StopAll();
	mainMenuThemeIsPlaying = false;
	int battleMusic = musicDist(rng);
	if (battleMusic == 0) {
		BattleTheme0.Play();
	}
	else if (battleMusic == 1) {
		BattleTheme1.Play();
	}
	else if (battleMusic == 2) {
		BattleTheme2.Play();
	}
	else {
		BattleTheme3.Play();
	}
}

void Game::GameLoop()
{
	if (paused) {
		if (wnd.kbd.KeyIsPressed(VK_CONTROL) && !enterOrEscapeHeld && (wnd.kbd.KeyIsPressed(VK_RETURN) || wnd.kbd.KeyIsPressed(VK_ESCAPE))) {
			paused = false;
			startMenu = true; //Enable the start menu
			BattleTheme0.StopAll();
			BattleTheme1.StopAll();
			BattleTheme2.StopAll();
			BattleTheme3.StopAll();
			previousWinner = 0;
		}
		else if (wnd.kbd.KeyIsPressed(VK_CONTROL) && wnd.kbd.KeyIsPressed(0x52)) { //If the game should be restarted
			GameStart();
		}
		else if (!enterOrEscapeHeld && (wnd.kbd.KeyIsPressed(VK_RETURN) || wnd.kbd.KeyIsPressed(VK_ESCAPE))) { //If the pause button is pressd
			paused = false; //Unpause the game
		}
	}
	else if (timer == 0) {
		GameEnd();
	}
	else if (Player1.IsAlive(gfx.ScreenWidth, gfx.ScreenHeight, leniancy) && Player2.IsAlive(gfx.ScreenWidth, gfx.ScreenHeight, leniancy)) {//If both players are alive
		timer--;
		if (hitStun > 0) {
			hitStun--;
			Player1.OnlyProjectiles(stageX0, stageY0, stageX1, stageY1);
			Player2.OnlyProjectiles(stageX0, stageY0, stageX1, stageY1);
		}
		else {
			if (enterOrEscapeHeld == false && (wnd.kbd.KeyIsPressed(VK_RETURN) || wnd.kbd.KeyIsPressed(VK_ESCAPE))) { //If the pause button is pressed
				timeUntilStart = 180; //Initialise the countdown
				timeGoIsDisplayed = 60; //Initialise go
				paused = true; //Pause the game
			}

			if (timeGoIsDisplayed > 0) { //If go should be displayed
				timeGoIsDisplayed--; //Reduce the time go should be displayed
			}

			//Update models
			Player1.UpdateCharacter(wnd.kbd.KeyIsPressed(0x41), wnd.kbd.KeyIsPressed(0x44), wnd.kbd.KeyIsPressed(0x53), wnd.kbd.KeyIsPressed(0x57), wnd.kbd.KeyIsPressed(0x47), wnd.kbd.KeyIsPressed(0x46), wnd.kbd.KeyIsPressed(0x54), wnd.kbd.KeyIsPressed(0x48), wnd.kbd.KeyIsPressed(VK_SHIFT), Player2.width, Player2.height, stageX0, stageY0, stageX1, stageY1);
			if (AISelected) {
				int aiSelectedMove = 12 + (int)aiMoveDist(rng);
				ArtifialFriend.Update(Player1.x, Player1.y, Player1.width, Player1.height, Player2.x, Player2.y, Player2.width, Player2.height, Player2.invincibilityCooldown==0, Player2.vy, Player2.doubleJump, stageX0, stageY0, stageX1, stageY1, aiMoveDist(rng));
				Player2.UpdateCharacter(ArtifialFriend.left, ArtifialFriend.right, ArtifialFriend.down, ArtifialFriend.up, ArtifialFriend.jump, ArtifialFriend.light, ArtifialFriend.heavy, ArtifialFriend.special, ArtifialFriend.dodge, Player1.width, Player1.height, stageX0, stageY0, stageX1, stageY1);
			}
			else {
				Player2.UpdateCharacter(wnd.kbd.KeyIsPressed(VK_LEFT), wnd.kbd.KeyIsPressed(VK_RIGHT), wnd.kbd.KeyIsPressed(VK_DOWN), wnd.kbd.KeyIsPressed(VK_UP), wnd.kbd.KeyIsPressed(0x4C), wnd.kbd.KeyIsPressed(0x4B), wnd.kbd.KeyIsPressed(0x4F), wnd.kbd.KeyIsPressed(VK_OEM_1), wnd.kbd.KeyIsPressed(0x4E), Player1.width, Player1.height, stageX0, stageY0, stageX1, stageY1);
			}

		}
		if (Player1.IsMoveColliding(Player2.x, Player2.y, Player2.width, Player2.height)) { //Is player 1 hitting any move
			HitSound.Play(10 / Player1.MoveThatHitDamage());
			hitStun = Player1.MoveThatHitDamage() * 2; //Hit stun activation
			Player2.IsHit(Player1.MoveThatHitStun(), Player1.MoveThatHitDamage(), Player1.MoveThatHitFixedX(), Player1.MoveThatHitFixedY(), Player1.MoveThatHitScalarX(), Player1.MoveThatHitScalarY()); //Register that player 2 has been hit
		}
		if (Player2.IsMoveColliding(Player1.x, Player1.y, Player1.width, Player1.height)) { //Is player 2 hitting any move
			HitSound.Play(10 / Player2.MoveThatHitDamage());
			hitStun = Player2.MoveThatHitDamage() * 2; //Hit stun activation
			Player1.IsHit(Player2.MoveThatHitStun(), Player2.MoveThatHitDamage(), Player2.MoveThatHitFixedX(), Player2.MoveThatHitFixedY(), Player2.MoveThatHitScalarX(), Player2.MoveThatHitScalarY()); //Register that player 1 has been hit
		}
	}
	else {
		GameEnd();
	}
}

void Game::GameStart()
{
	timer = 10 * 60 * 60 - 1;
	StartBattleTheme();
	Player1.x = gfx.ScreenWidth / 4 + 100; //Set player 1's starting position
	Player1.facingRight = true;
	Player1.Restart();
	Player2.x = gfx.ScreenWidth / 4 * 3 - Player2.width - 100; //Set player 2's starting position
	Player2.facingRight = false;
	Player2.Restart();
	timeUntilStart = 180;
	timeGoIsDisplayed = 60;
	paused = false;
}

void Game::GameEnd()
{
	startMenu = true; //Enable the start menu
	BattleTheme0.StopAll();
	BattleTheme1.StopAll();
	BattleTheme2.StopAll();
	BattleTheme3.StopAll();
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

void Game::ComposeFrame()
{
	if (credits) {
		gfx.DrawSprite(0, 0, creditsVisual , SpriteEffect::Copy{}, false);
	}
	else if (startMenu) {
		gfx.DrawSprite(0, 0, startVisual, SpriteEffect::Copy{}, false); //Displays the visual for the start menu

		if (AISelected) {
			gfx.DrawSprite(1600, 100, aiWarning, SpriteEffect::Copy{}, false);
		}

		if (previousWinner == 1) { //If Player 1 won the last match
			gfx.DrawSprite(gfx.ScreenWidth / 2 - 250, 500, player1Win, SpriteEffect::Copy{}, false); //Dislpay a sprite that shows this
		}
		else if (previousWinner == 2) { //If player 2 won the last match
			gfx.DrawSprite(gfx.ScreenWidth / 2 - 250, 500, player2Win, SpriteEffect::Copy{}, false); //Dislpay a sprite that shows this
		}

		gfx.DrawSprite(gfx.ScreenWidth / 4, 100, idle[player1CharacterID], SpriteEffect::Copy{}, false); //Draws an appropriate sprite based on character ID in the start menu
		gfx.DrawSprite(gfx.ScreenWidth / 4 * 3, 100, idle[player2CharacterID], SpriteEffect::Copy{}, true); //Draws an appropriate sprite based on character ID in the start menu
	}
	else {
		if (paused) {
			gfx.DrawSprite(0, 0, pauseVisual , SpriteEffect::Copy{}, false); //Display the pause menu

			if (Player1.lives == Player2.lives && Player1.playerPercentage == Player2.playerPercentage) { //If both players are even
				gfx.DrawRect(gfx.ScreenWidth / 2 - Player1.width - 1, 99, gfx.ScreenWidth / 2 + 1, 100 + Player1.height + 1, 255, 199, 0); //Player 1 border
				gfx.DrawSprite(gfx.ScreenWidth / 2 - Player1.width, 100, player1Idle, SpriteEffect::Copy{}, false); //Display player 1 at the top of the screen
				gfx.DrawRect(gfx.ScreenWidth / 2, 99, gfx.ScreenWidth / 2 + Player2.width + 1, 100 + Player2.height + 1, 255, 199, 0); //Player 2 border
				gfx.DrawSprite(gfx.ScreenWidth / 2, 100, player2Idle, SpriteEffect::Copy{}, false); //Display player 2 at the top of the screen
			}
			else if (Player1.lives > Player2.lives || (Player1.lives == Player2.lives && Player1.playerPercentage < Player2.playerPercentage)) {
				gfx.DrawRect(gfx.ScreenWidth / 4 - 1, 349, gfx.ScreenWidth / 4 + Player1.width + 1, 351 + Player1.height, 255, 199, 0); //Player 1 border
				gfx.DrawSprite(gfx.ScreenWidth / 4, 350, player1Idle, SpriteEffect::Copy{}, false); //Display player 1 on the left side
				gfx.DrawRect(gfx.ScreenWidth / 4 * 3 - 1, 349, gfx.ScreenWidth / 4 * 3 + Player2.width + 1, 351 + Player2.height, 255, 199, 0); //Player 2 border
				gfx.DrawSprite(gfx.ScreenWidth / 4 * 3, 350, player2Hit, SpriteEffect::Copy{}, false); //Display player 2 on the right side
			}
			else {
				gfx.DrawRect(gfx.ScreenWidth / 4 - 1, 349, gfx.ScreenWidth / 4 + Player2.width + 1, 351 + Player2.height, 255, 199, 0); //Player 1 border
				gfx.DrawSprite(gfx.ScreenWidth / 4, 350, player2Idle, SpriteEffect::Copy{}, false); //Display player 2 on the left side
				gfx.DrawRect(gfx.ScreenWidth / 4 * 3 - 1, 349, gfx.ScreenWidth / 4 * 3 + Player1.width + 1, 351 + Player1.height, 255, 199, 0); //Player 2 border
				gfx.DrawSprite(gfx.ScreenWidth / 4 * 3, 350, player1Hit, SpriteEffect::Copy{}, false); //Display player 1 on the right side
			}
		}

		else if (timeUntilStart > 120) { //If you should display 3
			gfx.DrawSprite(gfx.ScreenWidth / 2 - 150, gfx.ScreenHeight / 4 - 150, startThree, SpriteEffect::Copy{}, false); //Display 3
		}
		else if (timeUntilStart > 60) { //If you should display 2
			gfx.DrawSprite(gfx.ScreenWidth / 2 - 150, gfx.ScreenHeight / 4 - 150, startTwo, SpriteEffect::Copy{}, false); //Display 2
		}
		else if (timeUntilStart > 0) { //If you should display 1
			gfx.DrawSprite(gfx.ScreenWidth / 2 - 150, gfx.ScreenHeight / 4 - 150, startOne, SpriteEffect::Copy{}, false); //Display 1
		}
		else if (timeGoIsDisplayed > 0) { //If you should display go
			gfx.DrawSprite(gfx.ScreenWidth / 2 - 150, gfx.ScreenHeight / 4 - 150, go, SpriteEffect::Copy{}, false); //Display go
		}

		gfx.DrawRect(stageX0, stageY0, stageX1, stageY1, 255, 0, 0); //Stage

		gfx.DrawSprite(gfx.ScreenWidth / 4, 900, numbers[((int)Player1.playerPercentage - (int)Player1.playerPercentage % 10) / 10], SpriteEffect::Copy{}, false); //Player 1 percent
		gfx.DrawSprite(gfx.ScreenWidth / 4 + 30, 900, numbers[(int)Player1.playerPercentage % 10], SpriteEffect::Copy{}, false); //Player 1 percent
		gfx.DrawRect(gfx.ScreenWidth / 4 + 64, 939, gfx.ScreenWidth / 4 + 66, 941, 255, 255, 255); //Decimal point
		gfx.DrawSprite(gfx.ScreenWidth / 4 + 70, 900, numbers[(int)(Player1.playerPercentage * 10) % 10], SpriteEffect::Copy{}, false); //Player 1 percent
		for (int i = 0; i < Player1.lives; i++) {
			gfx.DrawSprite(gfx.ScreenWidth / 4 + i * 30, 950, player1LivesIcon, SpriteEffect::Copy{}, false); //Player 1 lives icon
		}
		for (int i = 0; i < Player2.lives; i++) {
			gfx.DrawSprite(gfx.ScreenWidth / 4 * 3 + i * 30, 950, player2LivesIcon, SpriteEffect::Copy{}, false); //Player 2 lives icon
		}

		gfx.DrawSprite(gfx.ScreenWidth / 4 * 3, 900, numbers[((int)Player2.playerPercentage - (int)Player2.playerPercentage % 10) / 10], SpriteEffect::Copy{}, false); //Player 2 percent
		gfx.DrawSprite(gfx.ScreenWidth / 4 * 3 + 30, 900, numbers[(int)Player2.playerPercentage % 10], SpriteEffect::Copy{}, false); //Player 2 percent
		gfx.DrawRect(gfx.ScreenWidth / 4 * 3 + 64, 939, gfx.ScreenWidth * 3 / 4 + 66, 941, 255, 255, 255); //Decimal point
		gfx.DrawSprite(gfx.ScreenWidth / 4 * 3 + 70, 900, numbers[(int)(Player2.playerPercentage * 10) % 10], SpriteEffect::Copy{}, false); //Player 2 percent
		if (AISelected) {
			gfx.DrawSprite(gfx.ScreenWidth / 4 * 3 + 120, 900, aiWarning, SpriteEffect::Copy{}, false);
		}

		if (Player1.invincibility > 0) { //If player 1 has invincibility
			gfx.DrawRect(Player1.x - 1, Player1.y - 1, Player1.x + Player1.width + 1, Player1.y + Player1.height + 1, 0, 237, 255); //Give them a border
		}
		if (Player2.invincibility > 0) { //If player 2 has invincibility
			gfx.DrawRect(Player2.x - 1, Player2.y - 1, Player2.x + Player2.width + 1, Player2.y + Player2.height + 1, 0, 237, 255); //Give them a border
		}

		if (Player1.moveDuration > 0 || Player1.freeFallDuration > 0) { //If Player 1 is using a move
			gfx.DrawSprite(Player1.x, Player1.y, player1Move, SpriteEffect::Copy{}, !Player1.facingRight); //Display move animation
		}
		else if (Player1.stun > 0) { //If player 1 is in stun
			gfx.DrawSprite(Player1.x, Player1.y, player1Hit, SpriteEffect::Copy{}, !Player1.facingRight); //Display stun animation
		}
		else {
			gfx.DrawSprite(Player1.x, Player1.y, player1Idle, SpriteEffect::Copy{}, !Player1.facingRight); //Display normal animation
		}

		if (Player2.moveDuration > 0 || Player2.freeFallDuration > 0) { //If Player 2 is using a move
			gfx.DrawSprite(Player2.x, Player2.y, player2Move, SpriteEffect::Copy{}, !Player2.facingRight); //Display move animation
		}
		else if (Player2.stun > 0) { //If player 2 is in stun
			gfx.DrawSprite(Player2.x, Player2.y, player2Hit, SpriteEffect::Copy{}, !Player2.facingRight); //Display stun animation
		}
		else {
			gfx.DrawSprite(Player2.x, Player2.y, player2Idle, SpriteEffect::Copy{}, !Player2.facingRight); //Display normal animation
		}


		for (int i = 0; i < 5; i++) { //For every move
			if (Player1.MoveDraw(i)) { //If it can be drawn
				gfx.DrawRect(Player1.MoveX0(i), Player1.MoveY0(i), Player1.MoveX1(i), Player1.MoveY1(i), Player1.MoveR(i), Player1.MoveG(i), Player1.MoveB(i)); //Draw it
			}
			if (Player2.MoveDraw(i)) { //If it can be drawn
				gfx.DrawRect(Player2.MoveX0(i), Player2.MoveY0(i), Player2.MoveX1(i), Player2.MoveY1(i), Player2.MoveR(i), Player2.MoveG(i), Player2.MoveB(i)); //Draw it
			}
		}
		gfx.DrawSprite(gfx.ScreenWidth / 2 - 50, 50, numbers[(int)(timer / 60 / 60)], SpriteEffect::Copy{}, false); //Time in minutes
		gfx.DrawRect(gfx.ScreenWidth / 2 - 15, 60, gfx.ScreenWidth / 2 - 10, 65, 255, 255, 255); //Colon
		gfx.DrawRect(gfx.ScreenWidth / 2 - 15, 85, gfx.ScreenWidth / 2 - 10, 90, 255, 255, 255); //Colon
		gfx.DrawSprite(gfx.ScreenWidth / 2, 50, numbers[(int)((timer /60 % 60 - timer / 60 % 10)/10)], SpriteEffect::Copy{}, false); //Time in tens
		gfx.DrawSprite(gfx.ScreenWidth / 2 + 50, 50, numbers[(int)(timer / 60 % 10)], SpriteEffect::Copy{}, false); //Time in seconds
	}
}