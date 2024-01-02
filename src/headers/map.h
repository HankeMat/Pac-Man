#ifndef MAP_H
#define MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>

#define PILL_SIZE_COEFFICIENT 3  // The higher number the smaller pill

int coords(int r, int c, int width);
void printMapInBinary(const char* mapArray, int mapWidthInBlocks, int mapHeightInBlocks);
void loadMap(const char* mapPath, int* mapHeightInBlocks, int* mapWidthInBlocks, char** mapArray);
void initializeWalls(SDL_Rect** block, int mapHeightInBlocks, int mapWidthInBlocks, double blockDimension, int startingPoint);
void initializePills(SDL_Rect** block, SDL_Rect** pill, int* pillCount, char* mapArray, int mapHeightInBlocks, int mapWidthInBlocks, double blockDimension, int bigPillIndexes[]);
int collectPill(SDL_Rect** pill, SDL_Rect* pacman, int* pillCount, int pillsLeft, int bigPillIndexes[], bool* frightent);
bool isThisBigPill(int bigPillIndexes[], int pillIndex);

#endif