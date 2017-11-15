#include <conio.h>
#include <stdio.h>
#include <Windows.h>
#include <list>


const char KEY_ESC = 27;
const char KEY_D = 'D';
const char KEY_d = 'd';
const char KEY_A = 'A';
const char KEY_a = 'a';
const char KEY_L = 'L';
const char KEY_l = 'l';
const char KEY_K = 'K';
const char KEY_k = 'k';
const int LEFT_LIMIT = 5;
const int RIGHT_LIMIT = 50;
const int ENEMY_RESPAWN_COUNT = 15;
const int CHARACTER_RESPAWN_COUNT = 45;
const int MUSHROOM_RESPAWN_COUNT = (60 + LEFT_LIMIT) * -1;
const int MUSHROOM_OBTAINED_POINTS = 50;
const int MUSHROOM_DESTROYED_POINTS = 10;


struct Bullet {
	int position;
	bool direction; // true to right, false to left
};

struct Enemy {
	int position;
	bool direction; // true to right, false to left
};

/************/

bool isValidPosition(int position) {
	return (position >= LEFT_LIMIT && position < RIGHT_LIMIT);
}

bool checkBulletHit(const Bullet &bullet, int hitPosition) {
	int nextPosition = bullet.direction ? (bullet.position + 1) : (bullet.position - 1);
	return(!(bullet.position - hitPosition) || !(nextPosition - hitPosition));
}

/************/

// TODO Utilizar clases. Módulo gestor meteorológico (varios estados de lluvia).

int main() {
	int                         i;
	char                        keyPressed = 0;
	bool                        doPlay = true;
	int                         characterPosition = LEFT_LIMIT;
	int                         mushroomPosition = MUSHROOM_RESPAWN_COUNT;
	int                         score = 0;
	int                         lives = 3;
	int                         loopsToEnemyRespawn = ENEMY_RESPAWN_COUNT;
	std::list<Bullet>           lstBullets;
	std::list<Bullet>::iterator itB;
	std::list<Enemy>            lstEnemies;
	std::list<Enemy>::iterator  itE;

	printf("\n\n\n\n\n\n");

	do {
		// Process input
		if (kbhit()) {
			keyPressed = getch();
			switch (keyPressed) {
			case KEY_ESC:
				doPlay = false;
				break;

			case KEY_D:
			case KEY_d:
				// Move right
				if (characterPosition >= LEFT_LIMIT && characterPosition < (RIGHT_LIMIT - 1))
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
				Bullet bulletR;
				bulletR.position = characterPosition;
				bulletR.direction = true;
				lstBullets.push_back(bulletR);
				break;

			case KEY_K:
			case KEY_k:
				// Shoot left
				Bullet bulletL;
				bulletL.position = characterPosition;
				bulletL.direction = false;
				lstBullets.push_back(bulletL);
				break;
			}
		}


		// Logic
		if (!isValidPosition(mushroomPosition)) {
			++mushroomPosition;
			if (isValidPosition(mushroomPosition)) { // Mushroom appears in a random position.
				mushroomPosition = (rand() % (RIGHT_LIMIT - LEFT_LIMIT)) + LEFT_LIMIT;
			}
		}

		itE = lstEnemies.begin();
		while (itE != lstEnemies.end()) {
			if (!itE->direction) --(itE->position);
			else                 ++(itE->position);

			if (isValidPosition(itE->position) && itE->position == characterPosition) { // Enemy hits character
				--lives;
				if (lives) characterPosition = -1 * CHARACTER_RESPAWN_COUNT;
				itE = lstEnemies.erase(itE);
			}
			else
				++itE;
		}

		if (isValidPosition(mushroomPosition) && mushroomPosition == characterPosition) { // Character takes mushroom
			mushroomPosition = MUSHROOM_RESPAWN_COUNT;
			score += MUSHROOM_OBTAINED_POINTS;
		}

		itB = lstBullets.begin();
		while (itB != lstBullets.end()) {
			bool bulletErased = false;
			if (isValidPosition(mushroomPosition) && checkBulletHit(*itB, mushroomPosition)) { // Bullet hits mushroom
				mushroomPosition = MUSHROOM_RESPAWN_COUNT;
				score -= MUSHROOM_DESTROYED_POINTS;
				itB = lstBullets.erase(itB);
				bulletErased = true;
			}
			else {
				itE = lstEnemies.begin();
				while (itE != lstEnemies.end()) {
					if (checkBulletHit(*itB, itE->position)) { // Bullet hits enemy
						itE = lstEnemies.erase(itE);
						itB = lstBullets.erase(itB);
						bulletErased = true;
						break;
					}
					else
						++itE;
				}
			}

			if (!bulletErased) {
				itB->direction                   ? ++(itB->position) : --(itB->position);
				(isValidPosition(itB->position)) ? ++itB             : itB = lstBullets.erase(itB);
			}
		}

		--loopsToEnemyRespawn;
		if (!loopsToEnemyRespawn) { // Enemy respawn
			loopsToEnemyRespawn = ENEMY_RESPAWN_COUNT;
			Enemy newEnemy;
			newEnemy.direction = rand() % 2;
			newEnemy.direction ? newEnemy.position = LEFT_LIMIT : newEnemy.position = RIGHT_LIMIT;
			lstEnemies.push_back(newEnemy);
		}

		if (!isValidPosition(characterPosition)) characterPosition++; // Character respawning


																	  // Render
		printf("\r");
		for (i = 0; i <= RIGHT_LIMIT; i++) {
			if (isValidPosition(i)) {
				bool renderedEntity = false;
				for (std::list<Enemy>::iterator itE = lstEnemies.begin(); itE != lstEnemies.end(); ++itE) {
					if (i == itE->position) {
						printf("@");
						renderedEntity = true;
						break;
					}
				}

				if (!renderedEntity) {
					for (std::list<Bullet>::iterator itB = lstBullets.begin(); itB != lstBullets.end(); ++itB) {
						if (i == itB->position) {
							if (itB->direction) printf(">");
							else                printf("<");
							renderedEntity = true;
							break;
						}
					}
				}

				if (!renderedEntity) {
					if (i == characterPosition)      printf("X");
					else if (i == mushroomPosition)  printf(":");
					else                             printf("_");
				}
			}
			else {
				if (RIGHT_LIMIT == i) printf("   LIVES: %d  SCORE: %d", lives, score);
				else                  printf(" ");
			}
		}

		Sleep(50);
	} while (doPlay && lives);

	if (!lives) {
		printf("\n\n\n\n");
		printf("       GAME OVER \n");
		printf("       Your score is %d", score);
		getch();
	}

	return 0;
}


