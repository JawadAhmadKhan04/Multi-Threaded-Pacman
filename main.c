
#include <GL/glut.h>

#include <SOIL/SOIL.h> // Include SOIL header

#include <stdio.h>

#include <time.h>

#include <string.h>

#include <unistd.h>

#include <stdbool.h>

#include <pthread.h>

#include <math.h>

#include <semaphore.h>
#include <stdlib.h>
#include <signal.h>


#define MAX_FOOD 147 // Maximum number of food items

#define FOOD_WIDTH 0.1f // Width of each food item

#define FOOD_HEIGHT 0.1f // Height of each food item

#define MAX_WALLS 200

#define wall_height 0.1f

#define wall_width 0.1f

#define NUM_GHOSTS 4

#define GHOST_WIDTH 0.074f

#define GHOST_HEIGHT 0.074f

#define PACMAN_RADIUS sqrt(0.033f * 0.033f + 0.033f * 0.033f) // Diagonal of the quad

#define COLLISION_BUFFER 0.035f // Adjust this value as needed

#define Keys 2

#define Permits 2



sem_t scenario1;
sem_t scenario4;
sem_t scenario3_keys;
sem_t scenario3_permits;



pthread_mutex_t handleKeypress_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t check_food_mutex = PTHREAD_MUTEX_INITIALIZER;



pthread_mutex_t ghost_locker;

pthread_mutex_t ghost_creation[4];



GLuint playerTexture; // Texture ID for player

GLuint backgroundTexture; // Texture ID for background

GLuint foodTexture; // Texture ID for food items



bool ghost_turn[4] = {false, false, false, false};



float playerX = 0.f;

float playerY = -0.2f;

float prevX = 0.0f;

float prevY = 0.0f;

float playerAngle = 0.0f;

bool movingUp = false;

bool movingDown = false;

bool movingLeft = false;

bool movingRight = false;

int score = 0;

int colorChanged = 0;

int lives = 3;

bool gameLost = false;

bool showHome = true;

float timer = 1;

bool src = false;

float val = 0.04;

float val2 = 0.04;

float putimer = 1;

int xtimer = 0;



bool pellet_eaten = false;

bool move1 = true;

bool move2 = true;

bool move3 = true;

bool move4 = true;

int bit = 1;

int bit2 = 1;

int gtimer = 0;

bool up = true, down = true, left = true, right = true;

const float playerSpeed = 0.05f;

struct Food {

    float x;

    float y;

    int eaten;

    GLuint textureID; // Texture ID for the food item

};

// Function to handle key press events

bool stopPlayerMovement() {

    movingUp = false;

    movingDown = false;

    movingLeft = false;

    movingRight = false;

    return true;

}

// Struct for wall

typedef struct {

    float x, y; // position of the wall

    float width, height; // size of the wall

    GLuint textureID;

    int type;

} Wall;


struct Food foodArray[MAX_FOOD]; // Array to store food items

struct Food pellets[4];

struct Food ghostpellet[2];

GLuint pelletTex;

GLuint ghostpelletTex;

// Function to check collision between pacman and food items

void checkFoodCollision() {

    pthread_mutex_lock(&check_food_mutex);

    for (int i = 0; i < MAX_FOOD; i++) {

        if (!foodArray[i].eaten) {

            // Calculate bounding box for food item

            float foodLeft = foodArray[i].x - FOOD_WIDTH / 4;  // Adjusted size

            float foodRight = foodArray[i].x + FOOD_WIDTH / 4; // Adjusted size

            float foodBottom = foodArray[i].y - FOOD_HEIGHT / 4; // Adjusted size

            float foodTop = foodArray[i].y + FOOD_HEIGHT / 4; // Adjusted size

            // Calculate bounding box for pacman

            float pacmanLeft = playerX - 0.05f; // Adjusted size

            float pacmanRight = playerX + 0.05f; // Adjusted size

            float pacmanBottom = playerY - 0.05f; // Adjusted size

            float pacmanTop = playerY + 0.05f; // Adjusted size

            // Check for intersection

            if (pacmanLeft <= foodRight && pacmanRight >= foodLeft &&

                pacmanBottom <= foodTop && pacmanTop >= foodBottom) {

                // Collision occurred

                foodArray[i].eaten = 1;

                score++; // Updating Score

            }

        }

    }
    pthread_mutex_unlock(&check_food_mutex);

}

bool checkPelletCollision() {

    if (pellet_eaten == false) {         

        for (int i = 0; i < 4; i++) {

            if (!pellets[i].eaten) {

                // Calculate bounding box for food item

                float foodLeft = pellets[i].x;//- FOOD_WIDTH / 4;  // Adjusted size

                float foodRight = pellets[i].x;// + FOOD_WIDTH / 4; // Adjusted size

                float foodBottom = pellets[i].y;// - FOOD_HEIGHT / 4; // Adjusted size

                float foodTop = pellets[i].y;

                // Calculate bounding box for pacman

                float pacmanLeft = playerX - 0.05f; // Adjusted size

                float pacmanRight = playerX + 0.05f; // Adjusted size

                float pacmanBottom = playerY - 0.05f; // Adjusted size

                float pacmanTop = playerY + 0.05f; // Adjusted size

                // Check for intersection

                if (pacmanLeft <= foodRight && pacmanRight >= foodLeft &&

                    pacmanBottom <= foodTop && pacmanTop >= foodBottom) {

                    // Collision occurred

                    pellets[i].eaten = 1;

                    pellet_eaten = true;

                    //printf("pellet eaten");

                    return true;

                }

            }

        }

    }

    return false;

}

GLuint wallTextureVert; // Texture ID for maze walls

GLuint wallTextureHor;

static Wall mazeWalls[MAX_WALLS]; // Array to store maze walls

typedef struct {

    float x;

    float y;

    float velocityX;

    float velocityY;

    GLuint textureID;

    bool isAlive;

    bool key;

    bool permit;

    bool isEaten;

} Ghost;


static Ghost ghosts[NUM_GHOSTS];

static GLuint ghostTex1;

static GLuint ghostTex2;

static GLuint ghostTex3;

static GLuint ghostTex4;

static GLuint ghostTex01;


void Scared(){

    //printf("%f\n", timer);

    // Set all ghosts to scared state

    if(src == true){

    	checkGhostCollisionScared();

		ghosts[0].isAlive = false;

		ghosts[1].isAlive = false;

		ghosts[2].isAlive = false;

		ghosts[3].isAlive = false;

		val = 0.01;

		val2 = 0.01;

		// Increment the timer while the ghosts are scared

		if (timer <= 75) {

		    timer++;

		}

    }

    // Check if the scared duration has ended

    if (timer > 75) {

        // Reset the timer and set all ghosts back to normal state

        pellet_eaten = false;

        timer = 1;

        src = false;

        ghosts[0].isAlive = true;

        ghosts[1].isAlive = true;

        ghosts[2].isAlive = true;

        ghosts[3].isAlive = true;

        ghosts[0].isEaten = false;

        ghosts[1].isEaten = false;

        ghosts[2].isEaten = false;

        ghosts[3].isEaten = false;

        val = 0.04;

        if(ghostpellet[0].eaten){

        	val2 = 0.06;

        }

        else{

        	val2 = 0.04;

        }

    }

}

bool checkWallCollision() {

    if ((playerX >= -0.17f && playerX <= 0.17f) && (playerY >= -.04f && playerY <= .1f)) {

        // Skip collision detection if player is within the specified range

        return false;

    }

    for (int i = 0; i < MAX_WALLS; i++) {

        float wallLeft = mazeWalls[i].x - wall_width / 2;

        float wallRight = mazeWalls[i].x + wall_width / 2;

        float wallTop = mazeWalls[i].y + wall_height / 2;

        float wallBottom = mazeWalls[i].y - wall_height / 2;

        // Adjusted size for pacman

        float pacmanLeft = playerX - 0.02f; // Increase left boundary

        float pacmanRight = playerX + 0.02f; // Increase right boundary

        float pacmanBottom = playerY - 0.02f; // Increase bottom boundary

        float pacmanTop = playerY + 0.02f; // Increase top boundary

        // Check for intersection

        if (pacmanLeft <= wallRight && pacmanRight >= wallLeft &&

            pacmanBottom <= wallTop && pacmanTop >= wallBottom) {

            // Collision occurred

            //printf("collision detected");

            return true;

        }

    }
    // No collision occurred
    return false;

}

void checkGhostCollision() {

    // Iterate over each ghost

    for (int i = 0; i < NUM_GHOSTS; i++) {

   		if(ghosts[i].isAlive == false){

   			continue;

   		}

        // Calculate bounding box for the ghost with a buffer zone

        float ghostLeft = ghosts[i].x - (GHOST_WIDTH / 2 + COLLISION_BUFFER);

        float ghostRight = ghosts[i].x + (GHOST_WIDTH / 2 + COLLISION_BUFFER);

        float ghostBottom = ghosts[i].y - (GHOST_HEIGHT / 2 + COLLISION_BUFFER);

        float ghostTop = ghosts[i].y + (GHOST_HEIGHT / 2 + COLLISION_BUFFER);



        // Calculate bounding box for pacman with a buffer zone

        float pacmanLeft = playerX - (PACMAN_RADIUS - COLLISION_BUFFER);

        float pacmanRight = playerX + (PACMAN_RADIUS - COLLISION_BUFFER);

        float pacmanBottom = playerY - (PACMAN_RADIUS - COLLISION_BUFFER);

        float pacmanTop = playerY + (PACMAN_RADIUS - COLLISION_BUFFER);



        // Check for intersection between pacman and the ghost

        if (pacmanLeft <= ghostRight && pacmanRight >= ghostLeft &&

            pacmanBottom <= ghostTop && pacmanTop >= ghostBottom) {

            // Collision occurred

            // Reset player coordinates to (0, -0.2)

            playerX = 0.0f;

            playerY = -0.2f;

            // ghost_turn[i] = false;

            // sem_post(&scenario3_keys);

            lives--;

            if(lives == 0){

            	gameLost = true;

            }

            // You may perform additional actions here, such as decreasing player lives

            // or resetting the game state

            break; // No need to continue checking other ghosts

        }

    }

}

void checkGhostCollisionScared() {

    // Iterate over each ghost

    for (int i = 0; i < NUM_GHOSTS; i++) {

        // Calculate bounding box for the ghost with a buffer zone

        float ghostLeft = ghosts[i].x - (GHOST_WIDTH / 2 + COLLISION_BUFFER);

        float ghostRight = ghosts[i].x + (GHOST_WIDTH / 2 + COLLISION_BUFFER);

        float ghostBottom = ghosts[i].y - (GHOST_HEIGHT / 2 + COLLISION_BUFFER);

        float ghostTop = ghosts[i].y + (GHOST_HEIGHT / 2 + COLLISION_BUFFER);

        // Calculate bounding box for pacman with a buffer zone

        float pacmanLeft = playerX - (PACMAN_RADIUS - COLLISION_BUFFER);

        float pacmanRight = playerX + (PACMAN_RADIUS - COLLISION_BUFFER);

        float pacmanBottom = playerY - (PACMAN_RADIUS - COLLISION_BUFFER);

        float pacmanTop = playerY + (PACMAN_RADIUS - COLLISION_BUFFER);



        // Check for intersection between pacman and the ghost

        if (pacmanLeft <= ghostRight && pacmanRight >= ghostLeft &&

            pacmanBottom <= ghostTop && pacmanTop >= ghostBottom) {

            // Collision occurred

            // Reset player coordinates to (0, -0.2)
            ghost_turn[i] = false;
            printf("ghost entering house: %i\n", i);
            sem_post(&scenario3_permits);
            sem_post(&scenario3_keys);

			ghosts[i].isEaten = true;

			if(i == 0){

				ghosts[i].x = -0.15f;

				ghosts[i].y = 0.1f;

				move1 = true;

			}

			if(i == 1){

				ghosts[i].x = -0.05f;

				ghosts[i].y = 0.1f;

				move2 = true;

			}

			if(i == 2){

				ghosts[i].x = 0.05f;

				ghosts[i].y = 0.1f;

				move3 = true;

				bit = 1;

			}

			if(i == 3){

				ghosts[i].x = 0.15f;

				ghosts[i].y = 0.1f;

				move4 = true;

				bit2 = 1;

			}

            // You may perform additional actions here, such as decreasing player lives

            // or resetting the game state

            break; // No need to continue checking other ghosts

        }

    }

}


bool fastmove = false;


void checkPelletGhostCollision() {

    // Iterate over each ghost

    for (int i = 0; i < NUM_GHOSTS; i++) {

        if (!ghosts[i].isAlive) {

            continue; // Skip if ghost is not alive

        }



        // Calculate bounding box for the ghost

        float ghostLeft = ghosts[i].x - (GHOST_WIDTH / 2);

        float ghostRight = ghosts[i].x + (GHOST_WIDTH / 2);

        float ghostBottom = ghosts[i].y - (GHOST_HEIGHT / 2);

        float ghostTop = ghosts[i].y + (GHOST_HEIGHT / 2);



        // Iterate over each food pellet

        for (int j = 0; j < 1; j++) {

            if (ghostpellet[j].eaten) {

                continue; // Skip if pellet is already eaten

            }



            // Calculate bounding box for the pellet

            float pelletLeft = ghostpellet[j].x;// - (PELLET_SIZE / 2);

            float pelletRight = ghostpellet[j].x;// + (PELLET_SIZE / 2);

            float pelletBottom = ghostpellet[j].y;// - (PELLET_SIZE / 2);

            float pelletTop = ghostpellet[j].y;// + (PELLET_SIZE / 2);



            // Check for intersection between pellet and ghost

            if (pelletLeft <= ghostRight && pelletRight >= ghostLeft &&

                pelletBottom <= ghostTop && pelletTop >= ghostBottom) {

                // Collision occurred
                sem_wait(&scenario4);
                ghostpellet[j].eaten = 1;

                for(int k = 0; k<2; k++){

                	if(ghostpellet[k].eaten == true){

                		fastmove = true;

                		continue;

                	}

                	else{

                		fastmove = false;

                		break;

                	}

                }

                if(fastmove){

                	val2 = 0.06;

                }

                break; // No need to check other pellets for this ghost

            }

        }

    }

}


void movePlayerUp(int value) {

    if (playerY + 0.1f < 1.0f && movingUp) {

        playerY += 0.03f;

        if (checkWallCollision()) { // Check for collision after moving

            playerY -= 0.03f; // Move back if collision detected

            movingUp = false; // Stop continuous movement

        }

        glutPostRedisplay();

        glutTimerFunc(100, movePlayerUp, 0);

    } else {

        movingUp = false;

    }

    checkFoodCollision();

    checkGhostCollision();

 	if(checkPelletCollision()){

    	src = true;

    }

}


void movePlayerDown(int value) {

    if (playerY - 0.1f > -1.0f && movingDown) {

        playerY -= 0.03f;

        if (checkWallCollision()) {

            playerY += 0.03f;

            movingDown = false;

        }

        glutPostRedisplay();

        glutTimerFunc(100, movePlayerDown, 0);

    } else {

        movingDown = false;

    }

    checkFoodCollision();

    checkGhostCollision();

    if(checkPelletCollision()){

    	src = true;

    }

}



void movePlayerLeft(int value) {

    if (playerX - 0.1f > -1.0f && movingLeft) {

        playerX -= 0.03f;

        if (checkWallCollision()) {

            playerX += 0.03f;

            movingLeft = false;

        }

        glutPostRedisplay();

        glutTimerFunc(100, movePlayerLeft, 0);

    } else {

        movingLeft = false;

    }

    checkFoodCollision();

    checkGhostCollision();

    if(checkPelletCollision()){

    	src = true;

    }

}



void movePlayerRight(int value) {

    if (playerX + 0.1f < 1.0f && movingRight) {

        playerX += 0.03f;

        if (checkWallCollision()) {

            playerX -= 0.03f;

            movingRight = false;

        }

        glutPostRedisplay();

        glutTimerFunc(100, movePlayerRight, 0);

    } else {

        movingRight = false;

    }

    checkFoodCollision();

    checkGhostCollision();

    if(checkPelletCollision()){

    	src = true;

    }

}



void ghost_general_movement(int i) {

    if (i == 0) {

        if(ghosts[0].isEaten == false){

    	    ghostMovement1();

        }

    }

    else if (i == 1) {

        if(ghosts[1].isEaten == false){

    	    ghostMovement2();

        }

    }

    else if (i == 2) {

        if(ghosts[2].isEaten == false){

        	ghostMovement3();

        }

    }

    else if (i == 3) {

        if(ghosts[3].isEaten == false){

    	    ghostMovement4();

        }

    }

   // Scared();

}

void init_ghosts(int i){

    for (int i = 0; i < 4; i++) {

        if (i == 0)

        {

            ghosts[0].x = -0.15;

            ghosts[0].y = 0.1;

            ghosts[0].isAlive = true;

            ghosts[0].isEaten = 0; //false initially

        }



        else if (i == 1) {       

            ghosts[1].x = -0.05;

            ghosts[1].y = .1;

            ghosts[1].isAlive = true;

            ghosts[1].isEaten = 0; //false initially

        }

        

        else if (i == 2) {

            ghosts[2].x = +0.05;

            ghosts[2].y = .1;

            ghosts[2].isAlive = true;

            ghosts[2].isEaten = 0; //false initially

        }

        

        else if (i == 3){

            ghosts[3].x = 0.15;

            ghosts[3].y = .1;

            ghosts[3].isAlive = true;

            ghosts[3].isEaten = 0; //false initially
        }

        if (i == 0)

        {

        	//printf("1");

            ghosts[0].textureID = ghostTex1 = SOIL_load_OGL_texture(

                "./images/ghost.png", // File path to wall image

                SOIL_LOAD_RGBA, // Load with alpha channel

                SOIL_CREATE_NEW_ID,

                SOIL_FLAG_INVERT_Y // Invert Y-axis because OpenGL's origin is at the bottom-left

            );		

            if (ghosts[0].textureID == 0) {

                printf("Error loading ghost1 texture: %s\n", SOIL_last_result());

            }

        }


        else if (i == 1)

        {

        	//printf("2");

            ghosts[1].textureID = ghostTex2 = SOIL_load_OGL_texture(

            "./images/ghost2(1).png", // File path to wall image

            SOIL_LOAD_RGBA, // Load with alpha channel

            SOIL_CREATE_NEW_ID,

            SOIL_FLAG_INVERT_Y // Invert Y-axis because OpenGL's origin is at the bottom-left

            );		

            if (ghosts[1].textureID == 0) {

                printf("Error loading ghost2 texture: %s\n", SOIL_last_result());

            }

        }

        else if (i == 2)

        {

        	//printf("3");

            ghosts[2].textureID = ghostTex3 = SOIL_load_OGL_texture(

                "./images/ghost3(1).png", // File path to wall image

                SOIL_LOAD_RGBA, // Load with alpha channel

                SOIL_CREATE_NEW_ID,

                SOIL_FLAG_INVERT_Y // Invert Y-axis because OpenGL's origin is at the bottom-left

            );		

            if (ghosts[2].textureID == 0) {

                printf("Error loading ghost3 texture: %s\n", SOIL_last_result());

            }

        }

        else if (i == 3) {

        	//printf("4");

            ghosts[3].textureID = ghostTex4 = SOIL_load_OGL_texture(

                "./images/ghost4(2).png", // File path to wall image

                SOIL_LOAD_RGBA, // Load with alpha channel

                SOIL_CREATE_NEW_ID,

                SOIL_FLAG_INVERT_Y // Invert Y-axis because OpenGL's origin is at the bottom-left

            );		

            if (ghosts[3].textureID == 0) {

                printf("Error loading ghost4 texture: %s\n", SOIL_last_result());

            }

        }

    }

}


void initghostpellet(){

	ghostpellet[0].x = 0;

	ghostpellet[0].y = -0.8;

	ghostpellet[0].eaten = 0;

	ghostpellet[0].textureID = ghostpelletTex = SOIL_load_OGL_texture(

		"./images/ghostpellet.png", // File path to wall image

		SOIL_LOAD_RGBA, // Load with alpha channel

		SOIL_CREATE_NEW_ID,

		SOIL_FLAG_INVERT_Y // Invert Y-axis because OpenGL's origin is at the bottom-left

	);		

	if (ghostpellet[0].textureID == 0) {

		printf("Error loading ghost pellet texture: %s\n", SOIL_last_result());

	}

	ghostpellet[1].x = 0;

	ghostpellet[1].y = 0.7;

	ghostpellet[1].eaten = 0;

	ghostpellet[1].textureID = ghostpelletTex = SOIL_load_OGL_texture(

		"./images/ghostpellet.png", // File path to wall image

		SOIL_LOAD_RGBA, // Load with alpha channel

		SOIL_CREATE_NEW_ID,

		SOIL_FLAG_INVERT_Y // Invert Y-axis because OpenGL's origin is at the bottom-left

	);		

	if (ghostpellet[1].textureID == 0) {

		printf("Error loading ghost pellet texture: %s\n", SOIL_last_result());

	}

}


void initMaze() {

    // Define position and size of the wall

    float wallX = -0.25f; // X-coordinate of the wall

    float wallY = 0.f; // Y-coordinate of the wall

    float wallWidth = .1f; // Width of the wall

    float wallHeight = .1f; // Height of the wall

    // Store the wall in the mazeWalls array

    mazeWalls[0].x = wallX;

    mazeWalls[0].y = wallY;

    mazeWalls[0].width = wallWidth;  //left wala box

    mazeWalls[0].height = wallHeight;

    

    wallX = .25f;

	wallY = 0.f;

	wallWidth = .1f;  //right wala box

	wallHeight = .1f;

    

    mazeWalls[1].x = wallX;

    mazeWalls[1].y = wallY;

    mazeWalls[1].width = wallWidth;

    mazeWalls[1].height = wallHeight;



	wallX = 0.25f;

	wallY = 0.1f;

	wallWidth = .1f;

	wallHeight = 0.1f;

    

    mazeWalls[2].x = wallX;

    mazeWalls[2].y = wallY;

    mazeWalls[2].width = wallWidth;

    mazeWalls[2].height = wallHeight;

    

    wallX = .25;

	wallY = 0.2f;

	wallWidth = .1f;

	wallHeight = 0.1f;


    mazeWalls[3].x = wallX;

    mazeWalls[3].y = wallY;

    mazeWalls[3].width = wallWidth;

    mazeWalls[3].height = wallHeight;
	wallX = 0.25f;

	wallY = 0.3f;

	wallWidth = .1f;  

	wallHeight = .1f; 

    mazeWalls[4].x = wallX;

    mazeWalls[4].y = wallY;

    mazeWalls[4].width = wallWidth;

    mazeWalls[4].height = wallHeight;

	/////////////right wala starting wall ^^ ////////////////
	

	wallX = -0.25f;

	wallY = 0.1f;

	wallWidth = .1f;  

	wallHeight = .1f;

    

    mazeWalls[5].x = wallX;

    mazeWalls[5].y = wallY;

    mazeWalls[5].width = wallWidth;

    mazeWalls[5].height = wallHeight;

    

    wallX = -0.25f;

	wallY = 0.2f;

	wallWidth = .1f;  

	wallHeight = .1f;

    

    mazeWalls[6].x = wallX;

    mazeWalls[6].y = wallY;

    mazeWalls[6].width = wallWidth;

    mazeWalls[6].height = wallHeight;

    

    wallX = -0.25f;

	wallY = 0.3f;

	wallWidth = .1f;  

	wallHeight = .1f;

    

    mazeWalls[7].x = wallX;

    mazeWalls[7].y = wallY;

    mazeWalls[7].width = wallWidth;

    mazeWalls[7].height = wallHeight;

    //////////////left starting wala wall^^^/////////////

    

    wallX = -0.25f;

	wallY = -0.1f;

	wallWidth = .1f;  

	wallHeight = .1f;

    

    mazeWalls[8].x = wallX;

    mazeWalls[8].y = wallY;

    mazeWalls[8].width = wallWidth;

    mazeWalls[8].height = wallHeight;

    

    wallX = -0.25f + 0.1;

	wallY = -0.1f;

	wallWidth = .1f;  

	wallHeight = .1f;

    

    mazeWalls[9].x = wallX;

    mazeWalls[9].y = wallY;

    mazeWalls[9].width = wallWidth;

    mazeWalls[9].height = wallHeight;

	

	wallX = -0.25f + 0.2;

	wallY = -0.1f;

	wallWidth = .1f;  

	wallHeight = .1f;

    

    mazeWalls[10].x = wallX;

    mazeWalls[10].y = wallY;

    mazeWalls[10].width = wallWidth;

    mazeWalls[10].height = wallHeight;

    

    wallX = -0.25f + 0.3;

	wallY = -0.1f;

	wallWidth = .1f;  

	wallHeight = .1f;

    

    mazeWalls[11].x = wallX;

    mazeWalls[11].y = wallY;

    mazeWalls[11].width = wallWidth;

    mazeWalls[11].height = wallHeight;

    

    wallX = -0.25f + 0.4;

	wallY = -0.1f;

	wallWidth = .1f;  

	wallHeight = .1f;

    

    mazeWalls[12].x = wallX;

    mazeWalls[12].y = wallY;

    mazeWalls[12].width = wallWidth;

    mazeWalls[12].height = wallHeight;

    

    wallX = -0.25f + 0.5;

	wallY = -0.1f;

	wallWidth = .1f;  

	wallHeight = .1f;

    mazeWalls[13].x = wallX;

    mazeWalls[13].y = wallY;

    mazeWalls[13].width = wallWidth;

    mazeWalls[13].height = wallHeight;

    wallX = -0.25f + 0.1;

	wallY = 0.3f;

	wallWidth = .1f;  

	wallHeight = .1f;

    mazeWalls[14].x = wallX;

    mazeWalls[14].y = wallY;

    mazeWalls[14].width = wallWidth;

    mazeWalls[14].height = wallHeight;

    wallX = -0.25f + 0.4;

	wallY = 0.3f;

	wallWidth = .1f;  

	wallHeight = .1f;

    mazeWalls[15].x = wallX;

    mazeWalls[15].y = wallY;

    mazeWalls[15].width = wallWidth;

    mazeWalls[15].height = wallHeight;

	/////////////////starting box k neechay wali line^^/////////////////	

	float k = 0.f;

	int i;

	for (i = 16; i < 34; i++) {

		wallX = -0.9f;

		wallY = 0.8f - k;

		wallWidth = .1f;  

		wallHeight = .1f;

		mazeWalls[i].x = wallX;

		mazeWalls[i].y = wallY;

		mazeWalls[i].width = wallWidth;

		mazeWalls[i].height = wallHeight;

		k+=0.1f;

    }

    ////////////////////////left boundary^^^///////////

	k = 0.f;

	for (i = 34; i < 52; i++) {

		wallX = 0.9f;

		wallY = 0.8f - k;

		wallWidth = .1f;  

		wallHeight = .1f;

		mazeWalls[i].x = wallX;

		mazeWalls[i].y = wallY;

		mazeWalls[i].width = wallWidth;

		mazeWalls[i].height = wallHeight;

		k+=0.1f;

    }

    //////////////////right boundary^^^^^///////////////

    k = 0.f;

    for (i = 52; i < 69; i++) {

		wallX = -0.8f + k;

		wallY = 0.8f;

		wallWidth = .1f;  

		wallHeight = .1f;

		mazeWalls[i].x = wallX;

		mazeWalls[i].y = wallY;

		mazeWalls[i].width = wallWidth;

		mazeWalls[i].height = wallHeight;

		k+=0.1f;

    }

    //////////////////////upper boundary^^^//////////////////

    k = 0.f;

    for (i = 69; i < 86; i++) {

		wallX = -0.8f + k;

		wallY = -0.9f;

		wallWidth = .1f;  

		wallHeight = .1f;	

		mazeWalls[i].x = wallX;

		mazeWalls[i].y = wallY;

		mazeWalls[i].width = wallWidth;

		mazeWalls[i].height = wallHeight;

		k+=0.1f;

    }

	///////////////////////lower boundary^^^^//////////////////

	k = 0.f;

	for (i = 86; i < 100; i++) {

		wallX = -0.7f;

		wallY = 0.6f - k;

		wallWidth = .1f;  

		wallHeight = .1f;

		mazeWalls[i].x = wallX;

		mazeWalls[i].y = wallY;

		mazeWalls[i].width = wallWidth;

		mazeWalls[i].height = wallHeight;

		k+=0.1f;

    }

	///////////////////left inner line^^^////////////////

	k = 0.f;

	for (i = 100; i < 114; i++) {

		wallX = 0.7f;

		wallY = 0.6f - k;

		wallWidth = .1f;  

		wallHeight = .1f;		

		mazeWalls[i].x = wallX;

		mazeWalls[i].y = wallY;

		mazeWalls[i].width = wallWidth;

		mazeWalls[i].height = wallHeight;

		k+=0.1f;

    }

    ///////////////////right inner line^^//////////////////
    k = 0.f;

	for (i = 114; i < 120; i++) {

		wallX = -0.48f;

		wallY = 0.6f - k;

		wallWidth = .1f;  

		wallHeight = .1f;	

		mazeWalls[i].x = wallX;

		mazeWalls[i].y = wallY;

		mazeWalls[i].width = wallWidth;

		mazeWalls[i].height = wallHeight;

		k+=0.1f;

    }

    /////////////////////third lefty^^////////////////
    k = 0.f;

	for (i = 120; i < 127; i++) {

		wallX = -0.48f;

		wallY = -0.7f + k;

		wallWidth = .1f;  

		wallHeight = .1f;
		mazeWalls[i].x = wallX;

		mazeWalls[i].y = wallY;

		mazeWalls[i].width = wallWidth;

		mazeWalls[i].height = wallHeight;

		k+=0.1f;

    }

    ///////////////////third left lower^^^///////////////
 k = 0.f;

	for (i = 127; i < 133; i++) {

		wallX = 0.48f;

		wallY = 0.6f - k;

		wallWidth = .1f;  

		wallHeight = .1f;

		mazeWalls[i].x = wallX;

		mazeWalls[i].y = wallY;

		mazeWalls[i].width = wallWidth;

		mazeWalls[i].height = wallHeight;

		k+=0.1f;

    }

    /////////////////////third righty^^////////////////

    k = 0.f;
	for (i = 133; i < 140; i++) {

		wallX = 0.48f;

		wallY = -0.7f + k;

		wallWidth = .1f;  

		wallHeight = .1f;

		mazeWalls[i].x = wallX;

		mazeWalls[i].y = wallY;

		mazeWalls[i].width = wallWidth;

		mazeWalls[i].height = wallHeight;

		k+=0.1f;

    }

    ///////////////////////third righty lower^^^^//////////
	k = 0.f;

	for (i = 140; i < 145; i++) {

		wallX = -0.2f + k;

		wallY = -0.7f;

		wallWidth = .1f;  

		wallHeight = .1f;

		mazeWalls[i].x = wallX;

		mazeWalls[i].y = wallY;

		mazeWalls[i].width = wallWidth;

		mazeWalls[i].height = wallHeight;

		k+=0.1f;

    }

    k = 0.f;

	for (i = 145; i < 150; i++) {

		wallX = -0.2f + k;

		wallY = -0.5f;

		wallWidth = .1f;  

		wallHeight = .1f;

		mazeWalls[i].x = wallX;

		mazeWalls[i].y = wallY;

		mazeWalls[i].width = wallWidth;

		mazeWalls[i].height = wallHeight;

		k+=0.1f;

    }

    k = 0.f;

	for (i = 150; i < 155; i++) {

		wallX = -0.2f + k;

		wallY = -0.3f;

		wallWidth = .1f;  

		wallHeight = .1f;

		mazeWalls[i].x = wallX;

		mazeWalls[i].y = wallY;

		mazeWalls[i].width = wallWidth;

		mazeWalls[i].height = wallHeight;

		k+=0.1f;

    }

    k = 0.f;

	for (i = 155; i < 160; i++) {

		wallX = -0.2f + k;

		wallY = 0.5f;

		wallWidth = .1f;  

		wallHeight = .1f;

		mazeWalls[i].x = wallX;

		mazeWalls[i].y = wallY;

		mazeWalls[i].width = wallWidth;

		mazeWalls[i].height = wallHeight;

		k+=0.1f;

    }

    k = 0.f;

	for (i = 160; i < 165; i++) {

		wallX = -0.2f + k;

		wallY = 0.6f;

		wallWidth = .1f;  

		wallHeight = .1f;

		mazeWalls[i].x = wallX;

		mazeWalls[i].y = wallY;

		mazeWalls[i].width = wallWidth;

		mazeWalls[i].height = wallHeight;

		k+=0.1f;

    }
    for(int i=0; i<MAX_WALLS; i++){

		mazeWalls[i].textureID = wallTextureVert = SOIL_load_OGL_texture(

			"./images/wall1.png", // File path to wall image

			SOIL_LOAD_RGBA, // Load with alpha channel

			SOIL_CREATE_NEW_ID,

			SOIL_FLAG_INVERT_Y // Invert Y-axis because OpenGL's origin is at the bottom-left

		);		

		if (mazeWalls[i].textureID == 0) {

			printf("Error loading maze wall texture: %s\n", SOIL_last_result());

		}

	}
}
// Function to handle key press events

void handleKeypress(unsigned char key, int x, int y) {
    sem_post(&scenario1);

	if (key != 'w' && key != 'a' && key != 's' && key != 'd') {

        return;

    }

    // Stop current movement in all directions

    stopPlayerMovement();

	// If space is pressed, the player stops wherever it is

    switch (key) {

        case 'w':

        	down = true;

        	left = true;

        	right = true;

            if (up == true && playerY + 0.1f < 1.0f) { // Check if moving up will keep player within window bounds

                movingUp = true; // Start continuous movement

               	up = false;

                playerAngle = 90.0f; // Setting angles

                glutTimerFunc(100, movePlayerUp, 0); // Start timer to continuously move player up

            }

            break;

        case 's':

        	up = true;

        	left = true;

        	right = true;

            if (down == true && playerY - 0.1f > -1.0f) { // Check if moving down will keep player within window bounds

                movingDown = true; // Start continuous movement

                down = false;

                playerAngle = -90.0f; // Setting angles

                glutTimerFunc(100, movePlayerDown, 0); // Start timer to continuously move player down

            }

            break;

        case 'a':

        	up = true;

        	down = true;

        	left = true;

            if (right == true && playerX - 0.1f > -1.0f) { // Check if moving left will keep player within window bounds

                movingLeft = true; // Start continuous movement

                right = false;

                playerAngle = 180.0f; // Setting angles

                glutTimerFunc(100, movePlayerLeft, 0); // Start timer to continuously move player left

            }

            break;

        case 'd':

        	up = true;

        	down = true;

        	right = true;

            if (left == true && playerX + 0.1f < 1.0f) { // Check if moving right will keep player within window bounds

                movingRight = true; // Start continuous movement

                left = false;

                playerAngle = 0.0f; // Setting angles

                glutTimerFunc(100, movePlayerRight, 0); // Start timer to continuously move player right
            }

            break;
    }

    pthread_mutex_unlock(&handleKeypress_mutex);

    sem_wait(&scenario1);

}

void initPellet(){

	pellets[0].x = -0.8;

	pellets[0].y = 0.7;

	pellets[1].x = 0.8;

	pellets[1].y = 0.7;

	pellets[2].x = -0.8;

	pellets[2].y = -0.8;

	pellets[3].x = 0.8;

	pellets[3].y = -0.8;

	for(int j = 0; j<4; j++){

    	pellets[j].textureID = SOIL_load_OGL_texture(

            "./images/berry1.png", // Replace with actual file path

            SOIL_LOAD_RGBA,

            SOIL_CREATE_NEW_ID,

            SOIL_FLAG_INVERT_Y

        );

        if (pellets[j].textureID == 0) {

            printf("Error pellet food image: %s\n", SOIL_last_result());

        }
        pellets[j].eaten = 0; // Not eaten initially

    }

}

void initFoodArray() {

    // Calculate the spacing between food items

    float horizontalSpacing = 1.0 / 11; // Divide the window width into 11 parts (10 for food, 1 for padding)

    float verticalSpacing = 1.0 / 11; // Divide the window height into 11 parts (10 for food, 1 for padding)

	int k = 0;

	int i;

    // Initialize food items in horizontal lines at the top and bottom

    for (i = 0; i < 17; i++) {

        foodArray[i].x = -.80 + horizontalSpacing * (k + 1); // Start from left padding and move right

        foodArray[i].y = 0.7; // Top of the screen

		++k;

    }

    k = 0;

    horizontalSpacing = 1.0/11;

    for (i=17; i < 35; i++){

    	foodArray[i].x = -.80 + horizontalSpacing * (k + 1);

    	foodArray[i].y = -0.8; 

    	++k;

    }

	 k =0;

    // Initialize food items in vertical lines at the left and right

    for (int i = 35; i < 52; i++) {

        foodArray[i].x = -0.8; // Left of the screen

        foodArray[i].y = -.90 + verticalSpacing * (k + 1); // Start from bottom padding and move up

		++k;

    }

	k = 0;
    // Initialize food items in vertical lines at the right

    for (int i = 52; i < 68; i++) {

        foodArray[i].x = 0.8; // Right of the screen

        foodArray[i].y = -.80 + verticalSpacing * (k + 1); // Start from bottom padding and move up

		++k;

        // Load texture for food item
    }

    k = 0;

    for(int i=68; i<84; i++){

    	foodArray[i].x = -0.6;

    	foodArray[i].y = -0.80 + verticalSpacing * (k + 1);

    	++k;

    }

    k = 0;

    for(int i=84; i<100; i++){

    	foodArray[i].x = -0.35;

    	foodArray[i].y = -0.80 + verticalSpacing * (k + 1);

    	++k;

    }

    k = 0;

    for(int i=100; i<116; i++){

    	foodArray[i].x = 0.6;

    	foodArray[i].y = -0.80 + verticalSpacing * (k + 1);

    	++k;

    }

    k = 0;

    for(int i=116; i<132; i++){

    	foodArray[i].x = 0.35;

    	foodArray[i].y = -0.80 + verticalSpacing * (k + 1);

    	++k;

    }

    k = 0;

    for(int i = 132; i < 137; i++){

    	foodArray[i].x = -0.25 + horizontalSpacing * (k + 1);

    	foodArray[i].y = -0.6;

    	++k;

    }

    k = 0;

    for(int i = 137; i < 142; i++){

    	foodArray[i].x = -0.25 + horizontalSpacing * (k + 1);

    	foodArray[i].y = -0.4;

    	++k;

    }

    k = 0;

    for(int i = 142; i < 147; i++){

    	foodArray[i].x = -0.25 + horizontalSpacing * (k + 1);

    	foodArray[i].y = +0.4;

    	++k;

    }

    for(int j = 0; j<MAX_FOOD; j++){

    	foodArray[j].textureID = SOIL_load_OGL_texture(

            "./images/cherry.png", 

            SOIL_LOAD_RGBA,

            SOIL_CREATE_NEW_ID,

            SOIL_FLAG_INVERT_Y

        );
        if (foodArray[j].textureID == 0) {

            printf("Error loading food image: %s\n", SOIL_last_result());

        }

        foodArray[j].eaten = 0; // Not eaten initially

    }

}


void drawText(float x, float y, const char* string) {

    glRasterPos2f(x, y);

    int len = strlen(string);

    for (int i = 0; i < len; i++) {

        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);

    }

}
void displayGameWon() {

    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.0f, 1.0f, 0.0f); //  text color to white

    glRasterPos2f(-0.3f, 0.0f); //  text position

    char message[] = "Game Won!";

    for (int i = 0; message[i] != '\0'; i++) {

        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]); // Render each character of the message

    }

    glFlush();

}

void displayGameLost() {

    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0f, 0.0f, 0.0f); // Set text color to white

    glRasterPos2f(-0.3f, 0.0f); // Set text position

    char message[] = "Game Lost!";

    for (int i = 0; message[i] != '\0'; i++) {

        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]); // Render each character of the message

    }

    glFlush();

}

void GameWon(){

	if (score == MAX_FOOD) {

        // Close the current window

        glutDestroyWindow(glutGetWindow());
        // Open a new window with "Game Won!" message

        glutInitWindowSize(400, 200);

        glutCreateWindow("Game Won!");

        glutDisplayFunc(displayGameWon);

    }

}

void GameLost(){

	if (gameLost) {

        // Close the current window

        glutDestroyWindow(glutGetWindow());
        // Open a new window with "Game Won!" message

        glutInitWindowSize(400, 200);

        glutCreateWindow("Game Lost!");

        glutDisplayFunc(displayGameLost);

    }

}

GLuint bgHome;


void init_Home(){

	bgHome = SOIL_load_OGL_texture(

        //"/home/fahd/Downloads/jakha(1).png", // File path to background image

        "./images/home.png",

        SOIL_LOAD_RGBA, // Load with alpha channel

        SOIL_CREATE_NEW_ID,

        SOIL_FLAG_INVERT_Y // Invert Y-axis because OpenGL's origin is at the bottom-left

    );

    if (bgHome == 0) {

        printf("Error loading background texture: %s\n", SOIL_last_result());

    }

    glColor3f(1.0f, 1.0f, 1.0f);

    glRasterPos2f(0.f, -0.3f);

    char Str[30];

    sprintf(Str, "Press Enter To Play: %d", score);

    for (int i = 0; Str[i] != '\0'; i++) {

        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, Str[i]);

    }		

}

void displayHomeScreen() {

    // Clear color buffer only

    glClear(GL_COLOR_BUFFER_BIT);

    // Draw background with a PNG texture

    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, bgHome);

    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);

    glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);

    glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);

    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);

    glEnd();

    glDisable(GL_TEXTURE_2D);

    

    // Display "Press Enter to Play" text

    glColor3f(1.0f, 1.0f, 1.0f);

    glRasterPos2f(-0.9f, 0.9f);

    char Str[30];

    sprintf(Str, "Press Enter To Play: %d", score);

    for (int i = 0; Str[i] != '\0'; i++) {

        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, Str[i]);

    }

    

    glFlush();

}


void handleHomeKey(unsigned char key){

	if(key == 13){

		glutDestroyWindow(glutGetWindow());

	}

	showHome = false;

	return;

}

bool allow_ghost_creation = false;


void ghost_controller(int i) {

    while(allow_ghost_creation == false);  

    init_ghosts(i);


    pthread_mutex_unlock(&ghost_creation[i]);

    while (true)

        {   

        sem_wait(&scenario3_keys);
        sem_wait(&scenario3_permits);
        printf("Ghost turn: %i\n", i);
        ghost_turn[i] = true;

        while (true) {

            if (ghost_turn[i] == false) {

                break;

            }

            if (gtimer >= 1)

            {

                gtimer = 0;

                ghost_general_movement(i);

            }

        }   

    }

}	


bool ghostpelletflag = false;

float gptimer = 1;

// Function to display content on the screen

void display() {


ghostTex1 = SOIL_load_OGL_texture(

    	"./images/ghost.png",

    	SOIL_LOAD_RGBA,

    	SOIL_CREATE_NEW_ID,

    	SOIL_FLAG_INVERT_Y

    );

    if(ghostTex1 == 0){

    	printf("error loading ghost image: %s\n", SOIL_last_result());

    }

    

    ghostTex2 = SOIL_load_OGL_texture(

    	"./images/ghost2(1).png",

    	SOIL_LOAD_RGBA,

    	SOIL_CREATE_NEW_ID,

    	SOIL_FLAG_INVERT_Y

    );

    if(ghostTex2 == 0){

    	printf("error loading ghost image: %s\n", SOIL_last_result());

    }

    

    ghostTex3 = SOIL_load_OGL_texture(

    	"./images/ghost3(1).png",

    	SOIL_LOAD_RGBA,

    	SOIL_CREATE_NEW_ID,

    	SOIL_FLAG_INVERT_Y

    );

    if(ghostTex3 == 0){

    	printf("error loading ghost image: %s\n", SOIL_last_result());

    }

    

    ghostTex4 = SOIL_load_OGL_texture(

    	"./images/ghost4(2).png",

    	SOIL_LOAD_RGBA,

    	SOIL_CREATE_NEW_ID,

    	SOIL_FLAG_INVERT_Y

    );
	// Clear color buffer only   

	glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0f,1.0f,1.0f);

    xtimer++;

    //printf("%f\n", timer);

    putimer++;
    gtimer++;
    gptimer++;
    // Draw background only once

    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, backgroundTexture);

    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);

    glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);

    glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);

    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);

    glEnd();

    glDisable(GL_TEXTURE_2D);

    // Draw food items

    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, foodTexture);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < MAX_FOOD; i++) {

        if (!foodArray[i].eaten) {

            glPushMatrix();

            glTranslatef(foodArray[i].x, foodArray[i].y, 0.0f);

            glBegin(GL_QUADS);

            glTexCoord2f(0.0f, 0.0f); glVertex2f(-FOOD_WIDTH / 2.5, -FOOD_HEIGHT / 2.5);

            glTexCoord2f(1.0f, 0.0f); glVertex2f(FOOD_WIDTH / 2.5, -FOOD_HEIGHT / 2.5);

            glTexCoord2f(1.0f, 1.0f); glVertex2f(FOOD_WIDTH / 2.5, FOOD_HEIGHT / 2.5);

            glTexCoord2f(0.0f, 1.0f); glVertex2f(-FOOD_WIDTH / 2.5, FOOD_HEIGHT / 2.5);

            glEnd();

            glPopMatrix();

        }

    }

    glDisable(GL_TEXTURE_2D);

    //draw pellets

	glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, pelletTex);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < 4; i++) {

        if (!pellets[i].eaten) {

            glPushMatrix();

            glTranslatef(pellets[i].x, pellets[i].y, 0.0f);

            glBegin(GL_QUADS);

            glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.037f, -0.037f);

			glTexCoord2f(1.0f, 0.0f); glVertex2f(0.037f, -0.037f);

			glTexCoord2f(1.0f, 1.0f); glVertex2f(0.037f, 0.037f);

			glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.037f, 0.037f);

            glEnd();

            glPopMatrix();

        }

    }

    glDisable(GL_TEXTURE_2D);

    //ghost pellet

    if(putimer >= 150.f && putimer <= 300.f){

		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, ghostpelletTex);

		glEnable(GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (int i = 0; i < 2; i++) {

		    if (!ghostpellet[i].eaten) {

		        glPushMatrix();

		        glTranslatef(ghostpellet[i].x, ghostpellet[i].y, 0.0f);

		        glBegin(GL_QUADS);

		        glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.037f, -0.037f);

				glTexCoord2f(1.0f, 0.0f); glVertex2f(0.037f, -0.037f);

				glTexCoord2f(1.0f, 1.0f); glVertex2f(0.037f, 0.037f);

				glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.037f, 0.037f);

		        glEnd();

		        glPopMatrix();

		    }

		}

		glDisable(GL_TEXTURE_2D);

    }


    if(gptimer > 250.f){

    	ghostpellet[0].eaten = false;

    	ghostpellet[1].eaten = false;

    	gptimer = 1;

    }
	

	// Draw Maze

	glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, wallTextureVert);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < MAX_WALLS; i++) {

        glPushMatrix();

        glTranslatef(mazeWalls[i].x, mazeWalls[i].y, 0.0f);

        glBegin(GL_QUADS);

        glTexCoord2f(0.0f, 0.0f); glVertex2f(-mazeWalls[i].width / 2.0f, -mazeWalls[i].height / 2.0f);

        glTexCoord2f(1.0f, 0.0f); glVertex2f(mazeWalls[i].width / 2.0f, -mazeWalls[i].height / 2.0f);

        glTexCoord2f(1.0f, 1.0f); glVertex2f(mazeWalls[i].width / 2.0f, mazeWalls[i].height / 2.0f);

        glTexCoord2f(0.0f, 1.0f); glVertex2f(-mazeWalls[i].width / 2.0f, mazeWalls[i].height / 2.0f);

        glEnd();

        glPopMatrix();

    }

    glDisable(GL_TEXTURE_2D);


    // Draw player

    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, playerTexture);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();

    glTranslatef(playerX, playerY, 0.0f);

    glRotatef(playerAngle, 0.0f, 0.0f, 1.0f);

    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.028f, -0.028f);

	glTexCoord2f(1.0f, 0.0f); glVertex2f(0.028f, -0.028f);

	glTexCoord2f(1.0f, 1.0f); glVertex2f(0.028f, 0.028f);

	glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.028f, 0.028f);

    glEnd();

    glPopMatrix();

    glDisable(GL_BLEND);

    glDisable(GL_TEXTURE_2D);

       

    sem_post(&scenario1);

    checkPelletGhostCollision();
	 Scared();



    if (xtimer >= 200) {

        xtimer = 0;

        for (int i = 0;i < 4; i++) {

            pellets[i].eaten = false;

        }

    }
    //ghost 1

    if(ghosts[0].isAlive){

		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, ghostTex1);

		glEnable(GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPushMatrix();

		glTranslatef(ghosts[0].x, ghosts[0].y, 0.0f);

		glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.037f, -0.037f);

		glTexCoord2f(1.0f, 0.0f); glVertex2f(0.037f, -0.037f);

		glTexCoord2f(1.0f, 1.0f); glVertex2f(0.037f, 0.037f);

		glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.037f, 0.037f);

		glEnd();

		glPopMatrix();

		glDisable(GL_BLEND);

		glDisable(GL_TEXTURE_2D);

    }

    else{

    	if(!ghosts[0].isEaten){

			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, ghostTex01);

			glEnable(GL_BLEND);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glPushMatrix();

			glTranslatef(ghosts[0].x, ghosts[0].y, 0.0f);

			glBegin(GL_QUADS);

			glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.037f, -0.037f);

			glTexCoord2f(1.0f, 0.0f); glVertex2f(0.037f, -0.037f);

			glTexCoord2f(1.0f, 1.0f); glVertex2f(0.037f, 0.037f);

			glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.037f, 0.037f);

			glEnd();

			glPopMatrix();

			glDisable(GL_BLEND);

			glDisable(GL_TEXTURE_2D);

		}

    }

    //ghost 2

    if(ghosts[1].isAlive){

		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, ghostTex2);

		glEnable(GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPushMatrix();

		glTranslatef(ghosts[1].x, ghosts[1].y, 0.0f);

		glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.037f, -0.037f);

		glTexCoord2f(1.0f, 0.0f); glVertex2f(0.037f, -0.037f);

		glTexCoord2f(1.0f, 1.0f); glVertex2f(0.037f, 0.037f);

		glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.037f, 0.037f);

		glEnd();

		glPopMatrix();

		glDisable(GL_BLEND);

		glDisable(GL_TEXTURE_2D);

    }

    else{

    	if(!ghosts[1].isEaten){

			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, ghostTex01);

			glEnable(GL_BLEND);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glPushMatrix();

			glTranslatef(ghosts[1].x, ghosts[1].y, 0.0f);

			glBegin(GL_QUADS);

			glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.037f, -0.037f);

			glTexCoord2f(1.0f, 0.0f); glVertex2f(0.037f, -0.037f);

			glTexCoord2f(1.0f, 1.0f); glVertex2f(0.037f, 0.037f);

			glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.037f, 0.037f);

			glEnd();

			glPopMatrix();

			glDisable(GL_BLEND);

			glDisable(GL_TEXTURE_2D);

		}

    }    

    //ghost 3

    if(ghosts[2].isAlive){

		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, ghostTex3);

		glEnable(GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPushMatrix();

		glTranslatef(ghosts[2].x, ghosts[2].y, 0.0f);

		glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.037f, -0.037f);

		glTexCoord2f(1.0f, 0.0f); glVertex2f(0.037f, -0.037f);

		glTexCoord2f(1.0f, 1.0f); glVertex2f(0.037f, 0.037f);

		glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.037f, 0.037f);

		glEnd();

		glPopMatrix();

		glDisable(GL_BLEND);

		glDisable(GL_TEXTURE_2D);

    }

    else{

    	if(!ghosts[2].isEaten){

			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, ghostTex01);

			glEnable(GL_BLEND);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glPushMatrix();

			glTranslatef(ghosts[2].x, ghosts[2].y, 0.0f);

			glBegin(GL_QUADS);

			glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.037f, -0.037f);

			glTexCoord2f(1.0f, 0.0f); glVertex2f(0.037f, -0.037f);

			glTexCoord2f(1.0f, 1.0f); glVertex2f(0.037f, 0.037f);

			glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.037f, 0.037f);

			glEnd();

			glPopMatrix();

			glDisable(GL_BLEND);

			glDisable(GL_TEXTURE_2D);

		}

    }

    //ghost 4

    if(ghosts[3].isAlive){

		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, ghostTex4);

		glEnable(GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPushMatrix();

		glTranslatef(ghosts[3].x, ghosts[3].y, 0.0f);

		glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.037f, -0.037f);

		glTexCoord2f(1.0f, 0.0f); glVertex2f(0.037f, -0.037f);

		glTexCoord2f(1.0f, 1.0f); glVertex2f(0.037f, 0.037f);

		glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.037f, 0.037f);

		glEnd();

		glPopMatrix();

		glDisable(GL_BLEND);

		glDisable(GL_TEXTURE_2D);

    }

    else{

    	if(!ghosts[3].isEaten){

			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, ghostTex01);

			glEnable(GL_BLEND);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glPushMatrix();

			glTranslatef(ghosts[3].x, ghosts[3].y, 0.0f);

			glBegin(GL_QUADS);

			glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.037f, -0.037f);

			glTexCoord2f(1.0f, 0.0f); glVertex2f(0.037f, -0.037f);

			glTexCoord2f(1.0f, 1.0f); glVertex2f(0.037f, 0.037f);

			glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.037f, 0.037f);

			glEnd();

			glPopMatrix();

			glDisable(GL_BLEND);

			glDisable(GL_TEXTURE_2D);

		}

    }
   sem_wait(&scenario1);

    // Display score

    glColor3f(1.0f, 0.0f, 0.0f);

    glRasterPos2f(-0.9f, 0.9f);

    char scoreStr[20];

    sprintf(scoreStr, "Score: %d", score);

    for (int i = 0; scoreStr[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreStr[i]);
    }
    // Display lives

    glColor3f(1.0f, 0.0f, 0.0f);

    glRasterPos2f(-0.3f, 0.9f);

    char livesStr[20];

    sprintf(livesStr, "Lives: %d", lives);

    for (int i = 0; livesStr[i] != '\0'; i++) {

        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, livesStr[i]);

    }

    // Check if the game is won

 	GameWon();

 	GameLost();

	if(putimer >= 100.f && ghostpelletflag == false){

		initghostpellet();

		ghostpelletflag = true;

        putimer = 0;

	}
    glFlush();

}

void init() {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background color

    // Load PNG image and create texture for player

    playerTexture = SOIL_load_OGL_texture(

        "./images/pacman1.png", // File path to player image

        SOIL_LOAD_RGBA, // Load with alpha channel

        SOIL_CREATE_NEW_ID,

        SOIL_FLAG_INVERT_Y // Invert Y-axis because OpenGL's origin is at the bottom-left

    );

    if (playerTexture == 0) {

        printf("Error loading player texture: %s\n", SOIL_last_result());

    }

    // Load PNG image and create texture for background

    backgroundTexture = SOIL_load_OGL_texture(

        //"/home/fahd/Downloads/jakha(1).png", // File path to background image

        "./images/black.jpg",

        SOIL_LOAD_RGBA, // Load with alpha channel

        SOIL_CREATE_NEW_ID,

        SOIL_FLAG_INVERT_Y // Invert Y-axis because OpenGL's origin is at the bottom-left

    );

    if (backgroundTexture == 0) {

        printf("Error loading background texture: %s\n", SOIL_last_result());

    }

    // Load PNG image and create texture for food items

    foodTexture = SOIL_load_OGL_texture(

        "./images/cherry.png", // File path to food image

        SOIL_LOAD_RGBA, // Load with alpha channel

        SOIL_CREATE_NEW_ID,

        SOIL_FLAG_INVERT_Y // Invert Y-axis because OpenGL's origin is at the bottom-left

    );

    if (foodTexture == 0) {

        printf("Error loading food texture: %s\n", SOIL_last_result());

    }

    //scared ghost 1

    ghostTex01 = SOIL_load_OGL_texture(

        "./images/scared1(1).png", // File path to food image

        SOIL_LOAD_RGBA, // Load with alpha channel

        SOIL_CREATE_NEW_ID,

        SOIL_FLAG_INVERT_Y // Invert Y-axis because OpenGL's origin is at the bottom-left

    );

    if (ghostTex01 == 0) {

        printf("Error loading scared texture: %s\n", SOIL_last_result());

    }

    //pellets of power

    pelletTex = SOIL_load_OGL_texture(

        "./images/berry1.png", // File path to food image

        SOIL_LOAD_RGBA, // Load with alpha channel

        SOIL_CREATE_NEW_ID,

        SOIL_FLAG_INVERT_Y // Invert Y-axis because OpenGL's origin is at the bottom-left

    );

    if (foodTexture == 0) {

        printf("Error loading pellet texture: %s\n", SOIL_last_result());

    }

    //ghost pellet

    ghostpelletTex = SOIL_load_OGL_texture(

    	"./images/ghostpellet.png",

    	SOIL_LOAD_RGBA,

    	SOIL_CREATE_NEW_ID,

    	SOIL_FLAG_INVERT_Y

    );

    if(ghostpelletTex == 0){

    	//printf("error loading ghost pelle: %s\n", SOIL_last_result());

    }

    ghostTex1 = SOIL_load_OGL_texture(

    	"./images/ghost.png",

    	SOIL_LOAD_RGBA,

    	SOIL_CREATE_NEW_ID,

    	SOIL_FLAG_INVERT_Y

    );

    if(ghostTex1 == 0){

    	printf("error loading ghost image: %s\n", SOIL_last_result());

    }
    

    ghostTex2 = SOIL_load_OGL_texture(

    	"./images/ghost2(1).png",

    	SOIL_LOAD_RGBA,

    	SOIL_CREATE_NEW_ID,

    	SOIL_FLAG_INVERT_Y

    );

    if(ghostTex2 == 0){

    	printf("error loading ghost image: %s\n", SOIL_last_result());

    }


    ghostTex3 = SOIL_load_OGL_texture(

    	"./images/ghost3(1).png",

    	SOIL_LOAD_RGBA,

    	SOIL_CREATE_NEW_ID,

    	SOIL_FLAG_INVERT_Y

    );

    if(ghostTex3 == 0){

    	printf("error loading ghost image: %s\n", SOIL_last_result());

    }


    ghostTex4 = SOIL_load_OGL_texture(

    	"./images/ghost4(2).png",

    	SOIL_LOAD_RGBA,

    	SOIL_CREATE_NEW_ID,

    	SOIL_FLAG_INVERT_Y

    );

    if(ghostTex4 == 0){

    	printf("error loading ghost image: %s\n", SOIL_last_result());

    }

    
    wallTextureVert = SOIL_load_OGL_texture(

    	"./images/wall1.png",

    	SOIL_LOAD_RGBA,

    	SOIL_CREATE_NEW_ID,

    	SOIL_FLAG_INVERT_Y

    );

    if(wallTextureVert == 0){

    	printf("error loading wall1 image: %s\n", SOIL_last_result());

    }
   // ghost_tex = true;

    initFoodArray();

    initMaze();

    allow_ghost_creation = true;

    // for (int i = 0; i < 4; i++) {

    //     //pthread_t pid;

    //   //  pthread_create(&pid, NULL, init_ghosts, i);

    // }

    initPellet();

}

void ghostMovement1() {

  // small threshold to account for floating-point imprecision

	  float threshold = 0.001;

	  // Check if the ghost is moving up

	if(move1){  

	  if(fabs(ghosts[0].x - (-0.15)) < threshold && ghosts[0].y < 0.2){

		ghosts[0].y += 0.05;

		if(ghosts[0].y > 0.2){

		  ghosts[0].y = 0.2;

		}

	  }

	  // Check if the ghost has reached x = -0.15 and y = 0.2, then move right until x = 0

	  if(ghosts[0].x != 0.f && ghosts[0].y == 0.2f) {

		// Move right until x reaches 0

		ghosts[0].x += 0.05;

		if (ghosts[0].x >= 0) { // Changed this line from > to >=

		  ghosts[0].x = 0; // Changed this line to set x to 0 instead of 0.025

		}

		// Ensuring y remains at 0.2

		ghosts[0].y = 0.2;

	  }

	  if(ghosts[0].y != 0.4 && ghosts[0].x == 0){

	  	ghosts[0].y += 0.05;

	  	if(ghosts[0].y >= 0.4){

	  		ghosts[0].y = 0.4;

	  	}

	  }

	  if(ghosts[0].x != 0.35f && ghosts[0].y == 0.4f){

	  	ghosts[0].x += 0.05;

	  	if(ghosts[0].x >= 0.35){

	  		ghosts[0].x = 0.35;

	  	}

	  	ghosts[0].y = 0.4;

	  }

	  if(ghosts[0].x == 0.35f && ghosts[0].y != 0.7f){

	  	ghosts[0].y += 0.05;

	  	if(ghosts[0].y >= 0.7f){

	  		ghosts[0].y = 0.7f;

	  	}

	  }

	  if(ghosts[0].x != 0.8f && ghosts[0].y == 0.7f){

	  	ghosts[0].x += 0.05;

	  	if(ghosts[0].x >= 0.8f){

	  		ghosts[0].x = 0.8f;

	  		move1 = false;

	  	}
	  	ghosts[0].y = 0.7f;
	  }

	 }

	 if(!move1){

	 	if(ghosts[0].x == 0.8f && ghosts[0].y != -0.8f){

	 		ghosts[0].y -= val2;

	 		if(ghosts[0].y <= -0.8f){

	 			ghosts[0].y = -0.8f;

	 		}

	 	}

	 	if(ghosts[0].x != -0.8f && ghosts[0].y == -.8f){

	 		ghosts[0].x -= val2;

	 		if(ghosts[0].x <= -0.8f){

	 			ghosts[0].x = -0.8;

	 		}

	 		ghosts[0].y = -0.8f;

	 	}

	 	if(ghosts[0].x == -0.8f && ghosts[0].y != .7f){

	 		ghosts[0].y += val2;

	 		if(ghosts[0].y >= 0.7f){

	 			ghosts[0].y = 0.7;

			}

	 	}

	 	if(ghosts[0].x != 0.8f && ghosts[0].y == 0.7f){

	 		ghosts[0].x += val2;

	 		if(ghosts[0].x >= 0.8f){

	 			ghosts[0].x = 0.8f;

	 		}

	 		ghosts[0].y = 0.7f;

	 	}

	 }	

  //printf("Initial x: %f, y: %f\n", ghosts[0].x, ghosts[0].y);

}

//upper limit 0.7f

//lower limit -0.8f



void ghostMovement2(){

	if(move2){

		if(ghosts[1].x == -0.05f && ghosts[1].y != 0.4f){

			ghosts[1].y += 0.05;

			if(ghosts[1].y >= 0.4f){

				ghosts[1].y = 0.4f;

			}

		}

		if(ghosts[1].x != -0.35f && ghosts[1].y == 0.4f){

			ghosts[1].x -= 0.05;

			if(ghosts[1].x <= -0.35){

				ghosts[1].x = -0.35;

			}

			ghosts[1].y = 0.4f;

		}

		if(ghosts[1].x == -0.35f && ghosts[1].y != 0.7f){

			ghosts[1].y += 0.05;

			if(ghosts[1].y >= 0.7f){

				ghosts[1].y = 0.7f;

				move2 = false;

			}

		}

		//printf("Initial x: %f, y: %f\n", ghosts[1].x, ghosts[1].y);

	}

	if(!move2){

		if(ghosts[1].x != 0.35f && ghosts[1].y == 0.7f){

			ghosts[1].x += val2;

			if(ghosts[1].x >= 0.35f){

				ghosts[1].x = 0.35f;

			}

			ghosts[1].y = 0.7f;

		}

		if(ghosts[1].x == 0.35f && ghosts[1].y != -0.8f){

			ghosts[1].y -= val2;

			if(ghosts[1].y <= -0.8f){

				ghosts[1].y = -0.8f;

			}

		}

		if(ghosts[1].x != -0.35f && ghosts[1].y == -.8f){

			ghosts[1].x -= val2;

			if(ghosts[1].x <= -0.35f){

				ghosts[1].x = -0.35f;

			}

			ghosts[1].y = -0.8f;

		}

		if(ghosts[1].x == -0.35f && ghosts[1].y != 0.7f){

			ghosts[1].y += val2;

			if(ghosts[1].y >= 0.7f){

				ghosts[1].y = 0.7f;

			}

		}

	}

}



void ghostMovement3(){

	if(move3){

		if(ghosts[2].x == 0.05f && ghosts[2].y != 0.4f){

			ghosts[2].y += 0.05;

			if(ghosts[2].y >= 0.4f){

				ghosts[2].y = 0.4f;

			}

		}

		if(ghosts[2].y == 0.4f && ghosts[2].x != 0.35f){

			ghosts[2].x += 0.05;

			if(ghosts[2].x >= 0.35f){

				ghosts[2].x = 0.35f;

			}

			ghosts[2].y = 0.4f;

		}

		if(ghosts[2].x == 0.35f && ghosts[2].y != -0.8f){

			ghosts[2].y -= 0.05;

			if(ghosts[2].y <= -0.8f){

				ghosts[2].y = -0.8f;

			}

		}

		if(ghosts[2].x != 0.6f && ghosts[2].y == -0.8f){

			ghosts[2].x += 0.05;

			if(ghosts[2].x >= 0.6f){

				ghosts[2].x = 0.6f;

				move3 = false;

			}

			ghosts[2].y = -0.8f;

		}

	}

	if(!move3){

		if(ghosts[2].y != 0.7f && ghosts[2].x == 0.6f && bit == 1){

			ghosts[2].y += val;

			if(ghosts[2].y >= 0.7f){

				ghosts[2].y = 0.7f;

				bit = 0;

			}

		}

		if(ghosts[2].y != -0.8f && ghosts[2].x == 0.6f && bit == 0){

			ghosts[2].y -= val;

			if(ghosts[2].y <= -0.8f){

				ghosts[2].y = -0.8f;

				bit = 1;

			}

		}

	}

}


void ghostMovement4(){

	if(move4){

		if(ghosts[3].x == 0.15f && ghosts[3].y != 0.2f){

			ghosts[3].y += 0.05;

			if(ghosts[3].y >= 0.2f){

				ghosts[3].y = 0.2f;

			}

		}

		if(ghosts[3].x != 0.0f && ghosts[3].y == 0.2f){

			ghosts[3].x -= 0.05;

			if(ghosts[3].x <= 0.0f){

				ghosts[3].x = 0.0f;

			}

			ghosts[3].y = 0.2f;

		}

		if(ghosts[3].x == 0.0f && ghosts[3].y != 0.4f){

			ghosts[3].y += 0.05;

			if(ghosts[3].y >= 0.4f){

				ghosts[3].y = 0.4f;

			}

		}

		if(ghosts[3].y == 0.4f && ghosts[3].x != -0.35f){

			ghosts[3].x -= 0.05;

			if(ghosts[3].x <= -0.35f){

				ghosts[3].x = -0.35f;

			}

			ghosts[3].y = 0.4f;

		}

		if(ghosts[3].x == -0.35f && ghosts[3].y != -0.8f){

			ghosts[3].y -= 0.05;

			if(ghosts[3].y <= -0.8f){

				ghosts[3].y = -0.8f;

			}

		}

		if(ghosts[3].x != -0.6f && ghosts[3].y == -0.8f){

			ghosts[3].x -= 0.05;

			if(ghosts[3].x <= -0.6f){

				ghosts[3].x = -0.6f;

				move4 = false;

			}

			ghosts[3].y = -0.8f;

		}

	}

	if(!move4){

		if(ghosts[3].y != 0.7f && ghosts[3].x == -0.6f && bit2 == 1){

			ghosts[3].y += val;

			if(ghosts[3].y >= 0.7f){

				ghosts[3].y = 0.7f;

				bit2 = 0;

			}

		}

		if(ghosts[3].y != -0.8f && ghosts[3].x == -0.6f && bit2 == 0){

			ghosts[3].y -= val;

			if(ghosts[3].y <= -0.8f){

				ghosts[3].y = -0.8f;

				bit2 = 1;

			}

		}

	}

}


pthread_mutex_t homeScreen;


bool home_screen_done = false;
bool glut_keyboard = false;


void home_screen_thread(char** t2){

	srand(time(NULL));

    int t1=1;

    glutInit(&t1, t2);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(600, 600);

    glutInitWindowPosition(100, 100);
    glutCreateWindow("Pacman");
    init_Home(); // Initialize OpenGL
    glutDisplayFunc(displayHomeScreen);
    glutKeyboardFunc(handleHomeKey);
    glutMainLoop();
    pthread_mutex_unlock(&homeScreen);
    home_screen_done = true;
}


void user_interface(char** t2) {

    srand(time(NULL));

    int t1=1;

    glutInit(&t1, t2);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(600, 600);

    glutInitWindowPosition(100, 100);

    glutCreateWindow("Pacman");

    init(); // Initialize OpenGL

    //initFoodArray(); // Initialize food array
    glutDisplayFunc(display);
    glut_keyboard = true;
  //  ghost_tex = true;
    glutMainLoop();
    
}


void main_enging_thread(char** t2){

    pthread_t user_interface_thread_id;

    pthread_create(&user_interface_thread_id, NULL, user_interface, t2);


    for (int i = 0; i < 4; i++)
        pthread_mutex_lock(&ghost_creation[i]);

    
    for (int i = 0; i < NUM_GHOSTS;i++) {
        pthread_t ghost_thread_id;
        pthread_create(&ghost_thread_id, NULL, ghost_controller, i);
    }

    while (true) {

        while (!glut_keyboard);

        glutKeyboardFunc(handleKeypress);

    }
}

int main(int argc, char** argv) {
    sem_init(&scenario1, 0, 1);
    sem_init(&scenario3_keys, 0, 2);
    sem_init(&scenario3_permits, 0, 2);
    sem_init(&scenario4, 0, 2);




    //pthread_t home_Screen_thread_id;

    //pthread_create(&home_Screen_thread_id, NULL, (void* (*)(void*))home_screen_thread, argv);

    //while (!home_screen_done);

    // Wait until the home screen thread signals that it's done

    pthread_t main_enging_thread_id;
    pthread_create(&main_enging_thread_id, NULL, (void* (*)(void*))main_enging_thread, argv);

    // This thread doesn't need to do anything more, so it can simply exit

    while(true) {

    }

    return 0;


}

