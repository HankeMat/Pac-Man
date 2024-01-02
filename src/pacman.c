#include "headers/pacman.h"
#include "headers/map.h"

void initializePacman(const char* mapArray, SDL_Rect** block, PacMan* pacman, int mapWidthInBlocks, int mapHeightInBlocks, int blockDimension) {
    
    pacman->rect.h = blockDimension * 2 - 1;
    pacman->rect.w = blockDimension * 2 - 1;

    for (int row = 0; row < mapHeightInBlocks - 1; ++row) {
        for (int col = 0; col < mapWidthInBlocks - 1; ++col) {
            int index = coords(row, col, mapWidthInBlocks);
            if (mapArray[index] == '3') {
                pacman->rect.x = (*block)[index].x;
                pacman->rect.y = (*block)[index].y;
            }
        }
    }
}

bool collisionWithWall(const char* mapArray, int newPacmanX, int newPacmanY, PacMan* pacman, int mapHeightInBlocks, int mapWidthInBlocks, SDL_Rect** block){
    SDL_Rect checkingPacManRect = { .x = newPacmanX, .y = newPacmanY, .w = pacman->rect.w, .h = pacman->rect.h};
    for (int i = 0; i < (mapHeightInBlocks * mapWidthInBlocks); ++i) {
        if (mapArray[i] == '1') {
            if (SDL_HasIntersection(&checkingPacManRect, &(*block)[i])) {
                return true;
                // printf("Collision detected\nWanted: %c\nActuall: %c\n", pacman.wantedDir, pacman.dir);
                break;
            }
        }
    }

    return false;
}

float setPacmanAngle(char pacmanWantedDirection){
    switch (pacmanWantedDirection)
    {
    case 'u':
        return 90.0;
        break;
    case 'd':
        return 270.0;
        break;
    case 'l':
        return 0.0;
        break;
    case 'r':
        return 180.0;
        break;
    default:
        break;
    }
    return 0.0;
}



