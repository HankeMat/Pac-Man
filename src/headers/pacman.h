#ifndef PACMAN_H
#define PACMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct {
    SDL_Rect rect;
    float speed;
    float angle;
    float accumX;
    float accumY;
    char dir;
    char wantedDir;
} PacMan;

void initializePacman(const char* mapArray, SDL_Rect** block, PacMan* pacman, int mapWidthInBlocks, int mapHeightInBlocks, int blockDimension);
bool collisionWithWall(const char* mapArray, int newPacmanX, int newPacmanY, PacMan* pacman, int mapHeightInBlocks, int mapWidthInBlocks, SDL_Rect** block);
float setPacmanAngle(char pacmanWantedDirection);

#endif 
