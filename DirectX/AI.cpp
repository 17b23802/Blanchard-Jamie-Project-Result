#include "AI.h"

void AI::Update(int player2x, int player2y, int player2Width, int player2Height, int x, int y, int vx, int vy, int width, int height, bool canDodge, int doubleJump, Platform Platforms[10], float moveType, int opponentPercent)
{
	int stageX0 = Platforms[0].x0;
	int stageX1 = Platforms[0].x1;
	int stageY0 = Platforms[0].y0;
	int stageY1 = Platforms[0].y1;
	//Reset all inputs
	left = false;
	right = false;
	down = false;
	up = false;
	jump = false;
	light = false;
	heavy = false;
	special = false;
	dodge = false;

	if (!ShouldUseMove(player2x, player2y, player2Width, player2Height, x, y, vx, width, height, doubleJump, moveType, stageX0, stageY0, stageX1, stageY1, opponentPercent) && !(y > stageY0 || x < stageX0 || x > stageX1)) {
		GeneralMovement(player2x, player2y, x, y, vy, doubleJump, moveType);
	}
	if (y > stageY0 || x < stageX0 || x > stageX1) {
		Recover(x, y, width, vy, canDodge, doubleJump, stageX0, stageY0, stageX1, stageY1);
	}
}

void AI::Recover(int x, int y, int width, int vy, bool canDodge, int doubleJump, int stageX0, int stageY0, int stageX1, int stageY1)
{
	if (x < 100 && canDodge) {
		right = true;
		dodge = true;
	}
	else if (x > 1820 && canDodge) {
		left = true;
		dodge = true;
	}
	else if (y < stageY1) { //If not under the stage
		if (x < stageX0) {
			right = true;
		}
		else {
			left = true;
		}
		if (doubleJump > 0 && vy >= 0) {
			jump = true;
		}
		else if (doubleJump == 0 && vy >= 0 && canDodge) {
			up = true;
			dodge = true;
		}
		else if (vy >= 0 && y>=stageY0-100) {
			up = true;
			special = true;
		}
	}
	else { //If under the stage
		if (x < stageX0 - width - 50) { //If x is far away from the stage
			right = true; //Go towards it
		}
		else if (x < stageX0 + (stageX1 - stageX0) / 2) { //If under the stage
			left = true; //Get out
		}
		else if (x < stageX1 + 50) { //If under the stage
			right = true; //Get out
		}
		else { //If x is far away from the stage
			left = true; //Go towards it
		}
		if (vy >= 0 && x < stageX0 || x > stageX1) {
			if (doubleJump > 0) {
				jump = true;
			}
			else {
				up = true;
				if (canDodge) {
					dodge = true;
				}
				else {
					special = true;
				}
			}
		}
		else {
			dodge = true;
		}
	}
}

bool AI::ShouldUseMove(int player2x, int player2y, int player2Width, int player2Height, int x, int y, int vx, int width, int height, int doubleJump, float moveType, int stageX0, int stageY0, int stageX1, int stageY1, int opponentPercent)
{
	if (x > stageX0 && x < stageX1 && (x > stageX0 + 20 || vx >= 0) && (x < stageX1 - 20 || vx <= 0) && y <= stageY0 && doubleJump > 0 && x - 30 < player2x + player2Width && x + width + 30 > player2x && y - 50 < player2y + player2Height && y + height + 50 > player2y) {
		if (x - 50 < player2x + width && x > player2x) {
			left = true;
		}
		else if (x + width < player2x && x + width + 50 > player2x) {
			right = true;
		}
		else if (y - 50 < player2y + player2Height && y > player2y + player2Height) {
			up = true;
		}
		else if (y + height < player2y && y + height + 50 > player2y) {
			down = true;
		}
		else if (moveType > 20) {
			up = true;
		}
		else {
			down = true;
		}

		if (moveType < 4 && x>stageX0 + 200 && x < stageX1 - 200) {
			dodge = true;
		}
		moveType = moveType * (opponentPercent+1) / 50;
		if (moveType < 20) {
			light = true;
		}
		else if (moveType < 25 && ((x>stageX0 + 200 && x < stageX1 - 200)||(!left&&!right))) {
			special = true;
		}
		else {
			heavy = true;
		}

		return true;
	}
	return false;

}

void AI::GeneralMovement(int player2x, int player2y, int x, int y, int vy, int doubleJump, float moveType)
{
	if (x > player2x) {
		left = true;
	}
	else if (x < player2x) {
		right = true;
	}
	if (moveType < 3) {
		if (player2y < y && vy >= 0) {
			jump = true;
		}
		if (player2y > y) {
			down = true;
		}
	}
	else if (moveType == 3 && vy >= 0) {
		jump = true;
	}
}
