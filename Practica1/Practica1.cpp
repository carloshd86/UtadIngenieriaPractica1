#include <conio.h>
#include <stdio.h>
#include <Windows.h>


const int KEY_ESC      = 27;
const int KEY_D        = 'D';
const int KEY_d        = 'd';
const int KEY_A        = 'A';
const int KEY_a        = 'a';
const int KEY_L        = 'L';
const int KEY_l        = 'l';
const int KEY_K        = 'K';
const int KEY_k        = 'k';
const int LEFT_LIMIT  = 10;
const int RIGHT_LIMIT  = 40;
const int ENEMY_RESPAWN_COUNT  = 65;
const int CHARACTER_RESPAWN_COUNT = 30;
const int MUSHROOM_RESPAWN_COUNT = 60;
const int MUSHROOM_POINTS = 50;
const int MUSHROOM_DESTROYED_POINTS = 10;

/************/

bool isValidPosition(int position) {
	return (position >= LEFT_LIMIT && position < RIGHT_LIMIT);
}

/************/

int main() {
	int  i;
	int  keyPressed          = 0;
	bool doPlay              = true;
	int  characterPosition   = LEFT_LIMIT;
	int  bulletPosition      = -1;
	int  enemyPosition       = (ENEMY_RESPAWN_COUNT + LEFT_LIMIT) * -1;
	int  mushroomPosition    = (MUSHROOM_RESPAWN_COUNT + LEFT_LIMIT) * -1;
	bool bulletDirection     = true;                              // true to right, false to left
	bool enemyDirection      = true;                              // true to right, false to left
	bool bulletHitsEnemy     = false;
	bool enemyHitsCharacter  = false;
	int  score               = 0;
	int  lives               = 3;

	printf("\n\n\n\n\n\n");

	while (doPlay && lives) {

		Sleep(50);

		// Input
		if (kbhit()) {
			keyPressed = getch();
			switch (keyPressed) {
				case KEY_ESC: //ESC
					doPlay = false;
					break;

				case KEY_D:
				case KEY_d:
					// Move right
					if(characterPosition >= LEFT_LIMIT && characterPosition < (RIGHT_LIMIT - 1))
						++characterPosition;
					break;

				case KEY_A:
				case KEY_a:
					// Move left
					if (characterPosition > LEFT_LIMIT)
						--characterPosition;
					break;

				case KEY_L:
				case KEY_l:
					// Shoot right
					if (bulletPosition < LEFT_LIMIT || bulletPosition >= RIGHT_LIMIT) {
						bulletPosition = characterPosition;
						bulletDirection = true;
					}
					break;

				case KEY_K:
				case KEY_k:
					// Shoot left
					if (bulletPosition < LEFT_LIMIT || bulletPosition >= RIGHT_LIMIT) {
						bulletPosition = characterPosition;
						bulletDirection = false;
					}
					break;
			}
		}


		// Logic
		if (enemyPosition == characterPosition)
			enemyHitsCharacter = true;

		if (!enemyDirection) {
			--enemyPosition;
			if (bulletDirection && (enemyPosition - bulletPosition) <= 1)
				bulletHitsEnemy = true;
		}
		else {
			++enemyPosition;
			if (!bulletDirection && (bulletPosition - enemyPosition) <= 1)
				bulletHitsEnemy = true;
		}

		if (isValidPosition(bulletPosition)) {
			if (bulletDirection)
				++bulletPosition;
			else
				--bulletPosition;

			if (bulletHitsEnemy || enemyPosition == bulletPosition) { // Bullet hits enemy
				bulletPosition = -1;
				if (enemyDirection) {
					enemyPosition = ENEMY_RESPAWN_COUNT + RIGHT_LIMIT;
					enemyDirection = false;
				}
				else {
					enemyPosition = (ENEMY_RESPAWN_COUNT + LEFT_LIMIT) * -1;
					enemyDirection = true;
				}

				enemyHitsCharacter = false;
				bulletHitsEnemy = false;
			}
		}

		if (enemyHitsCharacter || enemyPosition == characterPosition) { // Enemy hits character
			--lives;
			if (lives)
				characterPosition = -1 * CHARACTER_RESPAWN_COUNT;

				enemyPosition = ENEMY_RESPAWN_COUNT + RIGHT_LIMIT;
			if (enemyDirection) {
				enemyDirection = false;
			}
			else {
				enemyPosition *= -1;
				enemyDirection = true;
			}

			enemyHitsCharacter = false;
			bulletHitsEnemy = false;
		}

		if (!isValidPosition(mushroomPosition)) {
			++mushroomPosition;
			if (isValidPosition(mushroomPosition)) { // Mushroom appears in a random position.
				mushroomPosition = (rand() % (RIGHT_LIMIT - LEFT_LIMIT)) + LEFT_LIMIT;
			}
		}

		if (isValidPosition(mushroomPosition)) {
			if (mushroomPosition == characterPosition) { // Character takes mushroom
				mushroomPosition = (MUSHROOM_RESPAWN_COUNT + LEFT_LIMIT) * -1;
				score += MUSHROOM_POINTS;
			}

			if (mushroomPosition == bulletPosition) { // Bullet hits mushroom
				bulletPosition = -1;
				bulletHitsEnemy = false;
				mushroomPosition = (MUSHROOM_RESPAWN_COUNT + LEFT_LIMIT) * -1;
				score -= MUSHROOM_DESTROYED_POINTS;
			}
		}

		if (characterPosition < LEFT_LIMIT)
			characterPosition++;


		// Paint elements
		printf("\r");
		for (i = 0; i <= RIGHT_LIMIT; i++) {
			if (isValidPosition(i)) {
				if (i == bulletPosition) {
					if (bulletDirection)
						printf(">");
					else
						printf("<");
				}
				else if (i == characterPosition)
					printf("X");
				else if (i == enemyPosition)
					printf("@");
				else if (i == mushroomPosition)
					printf(":");
				else
					printf("_");
			} else {
				if (0 == i)
					printf("%d", lives);
				else if (RIGHT_LIMIT == i) {
					printf("   %d   ", score);
				}
				else
					printf(" ");
			}
		}

	}

	if (!lives) {
		printf("\n\n\n\n");
		printf("       GAME OVER \n");
		printf("       Your score is %d", score);
		getch();
	}

    return 0;
}


