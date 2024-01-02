#include "headers/ghosts.h"
#include "headers/pacman.h"
#include "headers/map.h"
#include "headers/sdl.h"

SDL_Texture* getGhostImage(SDL_Renderer* renderer, char *pathToFile){
    SDL_Surface* Srfc = SDL_LoadBMP(pathToFile);

    Uint32 BlackColor = SDL_MapRGB(Srfc->format, 0, 0, 0);
    SDL_SetColorKey(Srfc, SDL_ENABLE, BlackColor);

    SDL_Texture* Texture = SDL_CreateTextureFromSurface(renderer, Srfc); 
    SDL_FreeSurface(Srfc); 

    return Texture;
}

void resetPosition(char ghost, Ghost* blinky, Ghost* pinky, Ghost* inky, Ghost* clyde, const char* mapArray, SDL_Rect** block, int mapWidthInBlocks, int mapHeightInBlocks) {
    switch (ghost) {
        case 'b':
            resetPositionByColor('r', blinky, mapArray, block, mapWidthInBlocks, mapHeightInBlocks);
            break;
        case 'p':
            resetPositionByColor('p', pinky, mapArray, block, mapWidthInBlocks, mapHeightInBlocks);
            break;
        case 'i':
            resetPositionByColor('b', inky, mapArray, block, mapWidthInBlocks, mapHeightInBlocks);
            break;
        case 'c':
            resetPositionByColor('y', clyde, mapArray, block, mapWidthInBlocks, mapHeightInBlocks);
            break;
    }
}

void resetPositionByColor(char ghostColor, Ghost* ghost, const char* mapArray, SDL_Rect** block, int mapWidthInBlocks, int mapHeightInBlocks) {
    for (int row = 0; row < mapHeightInBlocks - 1; ++row) {
        for (int col = 0; col < mapWidthInBlocks - 1; ++col) {
            int index = coords(row, col, mapWidthInBlocks);
            if (mapArray[index] == ghostColor) {
                    ghost->rect.x = (*block)[index].x;
                    ghost->rect.y = (*block)[index].y;
            }
        }
    }
}

Ghost initializeGhost(char ghostColor, const char* mapArray, SDL_Rect** block, SDL_Renderer* renderer, int mapWidthInBlocks, int mapHeightInBlocks, int blockDimension) {
    Ghost ghost;
    ghost.wantedDir = -1;
    ghost.dir = -1;
    ghost.speed = 125;
    ghost.state = 0;

    for (int i = 0; i < 2; ++i) {
        char ghostPath[100];
        snprintf(ghostPath ,100, "src/images/ghosts/%c-%d.bmp", ghostColor, i);
        ghost.texture[i] = getGhostImage(renderer, ghostPath);
    }

    // Set ghost dimensions
    ghost.rect.h = blockDimension * 2 - 1;
    ghost.rect.w = blockDimension * 2 - 1;

    for (int row = 0; row < mapHeightInBlocks - 1; ++row) {
        for (int col = 0; col < mapWidthInBlocks - 1; ++col) {
            int index = coords(row, col, mapWidthInBlocks);
            if (mapArray[index] == ghostColor) {
                    ghost.rect.x = (*block)[index].x;
                    ghost.rect.y = (*block)[index].y;
            }
        }
    }

    return ghost;
}

bool ghostCollisionWithWall(const char* mapArray, int newPacmanX, int newPacmanY, Ghost ghost, int mapHeightInBlocks, int mapWidthInBlocks, SDL_Rect** block){
    SDL_Rect checkingRect = { .x = newPacmanX, .y = newPacmanY, .w = ghost.rect.w, .h = ghost.rect.h};
    for (int i = 0; i < (mapHeightInBlocks * mapWidthInBlocks); ++i) {
        if (mapArray[i] == '1') {
            if (SDL_HasIntersection(&checkingRect, &(*block)[i])) {
                return true;
                break;
            }
        }
    }

    return false;
}

char ghostAndPacmanIntersection(SDL_Rect pacmanRect, Ghost blinky, Ghost pinky, Ghost inky, Ghost clyde) {
    SDL_Rect hitBox = { .x = pacmanRect.x + (pacmanRect.w - (pacmanRect.w/2)) / 2, .y = pacmanRect.y + (pacmanRect.h - (pacmanRect.h/2)) / 2, .w = pacmanRect.w/2, .h = pacmanRect.h/2 };
    
    if (SDL_HasIntersection(&hitBox, &blinky.rect)) {
        return 'b';
    } else if (SDL_HasIntersection(&hitBox, &pinky.rect)) {
        return 'p';
    } else if (SDL_HasIntersection(&hitBox, &inky.rect)) {
        return 'i';
    } else if (SDL_HasIntersection(&hitBox, &clyde.rect)) {
        return 'c';
    }

    return 'x';
}

bool isFrightent(char ghost, Ghost blinky, Ghost pinky, Ghost inky, Ghost clyde) {
    switch (ghost) {
        case 'b':
            if (blinky.state == 1) {
                return true;
            } else return false;
            break;
        case 'p':
            if (pinky.state == 1) {
                return true;
            } else return false;
            break;
        case 'i':
            if (inky.state == 1) {
                return true;
            } else return false;
            break;
        case 'c':
            if (clyde.state == 1) {
                return true;
            } else return false;
            break;
    }

    return false;
}

void turnGhosts(Ghost* blinky, Ghost* pinky, Ghost* inky, Ghost* clyde) {
    blinky->wantedDir = (blinky->dir + 2) % 4;
    pinky->wantedDir = (pinky->dir + 2) % 4;
    inky->wantedDir = (inky->dir + 2) % 4;
    clyde->wantedDir = (clyde->dir + 2) % 4;
}

void resetState(char ghost, Ghost* blinky, Ghost* pinky, Ghost* inky, Ghost* clyde) {
    switch (ghost) {
        case 'b':
            blinky->state = 0;
            break;
        case 'p':
            pinky->state = 0;
            break;
        case 'i':
            inky->state = 0;
            break;
        case 'c':
            clyde->state = 0;
            break;
    }   
}

/**
 * @todo    Improve AI of the ghosts, random direction picker is not a great approach.
 *          Try to actually follow pacman and add those other ghost states (scatter, chase, frightent, eyes)
 */
void ghostMove(Ghost* ghost, double deltaTime, const char* mapArray, SDL_Rect** block, int mapHeightInBlocks, int mapWidthInBlocks) {
    ghost->accumX += ghost->speed * deltaTime;
    ghost->accumY += ghost->speed * deltaTime;
    
    int moveX = (int)ghost->accumX;
    int moveY = (int)ghost->accumY;

    int newghostX = ghost->rect.x;
    int newghostY = ghost->rect.y;

    if (moveX > 1 || moveY > 1) {
        if (ghost->wantedDir != -1) {
            int wantedTestX = ghost->rect.x;
            int wantedTestY = ghost->rect.y;

            switch (ghost->wantedDir) {
                case 0:
                    wantedTestY -= moveY;
                    break;
                case 2:
                    wantedTestY += moveY;
                    break;
                case 3:
                    wantedTestX -= moveX;
                    break;
                case 1:
                    wantedTestX += moveX;
                    break;
                default:
                    break;
            }

            if (!ghostCollisionWithWall(mapArray, wantedTestX, wantedTestY, *ghost, mapHeightInBlocks, mapWidthInBlocks, block)) {
                ghost->dir = ghost->wantedDir;
                ghost->wantedDir = -1;
            }
        }

        switch (ghost->dir) {
            case -1:
                // Movement uninitialized
                break;
            case 0:
                newghostY -= moveY;
                break;
            case 2:
                newghostY += moveY;
                break;
            case 3:
                newghostX -= moveX;
                break;
            case 1:
                newghostX += moveX;
                break;
        }

        bool collisionDetected = ghostCollisionWithWall(mapArray, newghostX, newghostY, *ghost, mapHeightInBlocks, mapWidthInBlocks, block);
        // Decide whether to actually move the ghost or not
        if (!collisionDetected) {
            // Moving ghost
            ghost->rect.x = newghostX;
            ghost->rect.y = newghostY;
            // Clearing accumulated values
            ghost->accumX -= moveX;
            ghost->accumY -= moveY;
        } else if (collisionDetected) {
            // Clearing accumulated values
            ghost->accumX -= moveX;
            ghost->accumY -= moveY;

            // Change later on...
            ghost->wantedDir = rand() % (3 - 0 + 1) + 0;
        }
    }
}

void wakeGhosts(Ghost *blinky, Ghost *pinky, Ghost *inky, Ghost *clyde) {
    blinky->dir = 0;
    pinky->dir = 0;
    inky->dir = 0;
    clyde->dir = 0;
}