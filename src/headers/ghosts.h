#ifndef GHOSTS_H
#define GHOSTS_H 

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct {
    SDL_Texture* texture[2];
    SDL_Rect rect;
    int dir;
    // Ghost states
    // 0 = scatter
    // 1 = frightent
    // 2 = chase
    // 3 = eaten
    int state;
    
    // Directions
    // 0 = 'u'
    // 1 = 'r'
    // 2 = 'd'
    // 3 = 'l' 
    // -1 = uninitialized
    int wantedDir;
    float speed;
    float accumX;
    float accumY;
} Ghost;

Ghost initializeGhost(char ghostColor, const char* mapArray, SDL_Rect** block, SDL_Renderer* renderer, int mapWidthInBlocks, int mapHeightInBlocks, int blockDimension);
SDL_Texture* getGhostImage(SDL_Renderer* renderer, char *pathToFile);
void ghostMove(Ghost* ghost, double deltaTime, const char* mapArray, SDL_Rect** block, int mapHeightInBlocks, int mapWidthInBlocks);
bool ghostCollisionWithWall(const char* mapArray, int newPacmanX, int newPacmanY, Ghost ghost, int mapHeightInBlocks, int mapWidthInBlocks, SDL_Rect** block);
void wakeGhosts(Ghost *blinky, Ghost *pinky, Ghost *inky, Ghost *clyde);
char ghostAndPacmanIntersection(SDL_Rect pacmanRect, Ghost blinky, Ghost pinky, Ghost inky, Ghost clyde);
bool isFrightent(char ghost, Ghost blinky, Ghost pinky, Ghost inky, Ghost clyde);
void resetState(char ghost, Ghost* blinky, Ghost* pinky, Ghost* inky, Ghost* clyde);
void resetPosition(char ghost, Ghost* blinky, Ghost* pinky, Ghost* inky, Ghost* clyde, const char* mapArray, SDL_Rect** block, int mapWidthInBlocks, int mapHeightInBlocks);
void resetPositionByColor(char ghostColor, Ghost* ghost, const char* mapArray, SDL_Rect** block, int mapWidthInBlocks, int mapHeightInBlocks);
void turnGhosts(Ghost* blinky, Ghost* pinky, Ghost* inky, Ghost* clyde);


#endif 