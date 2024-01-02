#include "headers/map.h"
#include "headers/pacman.h"

int coords(int r, int c, int width) { return r * width + c; }

void printMapInBinary(const char* mapArray, int mapWidthInBlocks, int mapHeightInBlocks) {
    for (int row = 0; row < mapHeightInBlocks; ++row) {
        for (int col = 0; col < mapWidthInBlocks; ++col) {
            fprintf(stderr, "%c ", mapArray[col + row * mapWidthInBlocks]);
        }
        fprintf(stderr, "\n");
    }
}

void loadMap(const char* mapPath, int* mapHeightInBlocks, int* mapWidthInBlocks, char** mapArray) {
    FILE* map = fopen(mapPath, "r");
    if (NULL == map) {
        fprintf(stderr, "Could not load map\n");
    }

    char line[100];

    for (int i = 0; i < 2; ++i) {
        if(fgets(line, sizeof(line), map) == NULL) {
            fprintf(stderr, "Unable to load line\n");
        } 

        if (strncmp(line, "h=", 2) == 0) {
            *mapHeightInBlocks = atoi(line + 2);
        } else if (strncmp(line, "w=", 2) == 0) {
            *mapWidthInBlocks = atoi(line + 2);
        }
    }

    *mapArray = malloc(sizeof(char) * (*mapWidthInBlocks) * (*mapHeightInBlocks));

    int row = 0;
    while (fgets(line, sizeof(line), map) != NULL && row < *mapHeightInBlocks) {
        int col = 0;
        for (int i = 0; i < (int)strlen(line) && col < *mapWidthInBlocks; ++i) {
            // 0            - Pills
            // 1            - Walls
            // 2            - Nothing
            // 3            - Pac-Man spawnpoint
            // 4            - Doors
            // r, b, y, p   - Ghost spawnpoints
            // 5            - Portals 
            if (line[i] == '0' || line[i] == '1' || line[i] == '2' || line[i] == '3' || line[i] == '4' || line[i] == 'r' || line[i] == 'p' || line[i] == 'b' || line[i] == 'y' || line[i] == '5') {
                (*mapArray)[col + row * (*mapWidthInBlocks)] = line[i];
                col++;
            }
        }
        row++;
    }
    fprintf(stderr, "============================== MAP ==============================\n\n");
    printMapInBinary(*mapArray, *mapWidthInBlocks, *mapHeightInBlocks);
    fprintf(stderr, "\n============================== MAP ==============================\n\n");
    fclose(map);
}

void initializeWalls(SDL_Rect** block, int mapHeightInBlocks, int mapWidthInBlocks, double blockDimension, int startingPoint) {
    *block = malloc(sizeof(SDL_Rect) * mapHeightInBlocks * mapWidthInBlocks);

    for (int row = 0; row < mapHeightInBlocks; ++row) {
        for (int col = 0; col < mapWidthInBlocks; ++col) {
            int index = coords(row, col, mapWidthInBlocks);
            (*block)[index].h = blockDimension;
            (*block)[index].w = blockDimension;
            (*block)[index].x = round(startingPoint + (col * blockDimension));
            (*block)[index].y = round(startingPoint + (row * blockDimension));
        }
    }
}

void initializePills(SDL_Rect** block, SDL_Rect** pill, int* pillCount, char* mapArray, int mapHeightInBlocks, int mapWidthInBlocks, double blockDimension, int bigPillIndexes[]) {
    (*pillCount) = 0;
    for (int row = 0; row < mapHeightInBlocks - 1; ++row) {
        for (int col = 0; col < mapWidthInBlocks - 1; ++col) {
            int index = coords(row, col, mapWidthInBlocks);
            if (mapArray[index] == '0' && mapArray[index + 1] == '0' && mapArray[index + mapWidthInBlocks] == '0' && mapArray[index + mapWidthInBlocks + 1] == '0') {
                (*pillCount)++;
            }
        }
    }

    // Generate big pills indexes
    for (int i = 0; i < 4; ++i) {
        int randNum = rand() % (*pillCount - 0 + 1) + 0;
        while (isThisBigPill(bigPillIndexes, randNum)) {
            randNum = rand() % (*pillCount - 0 + 1) + 0;
        }
        bigPillIndexes[i] = randNum;
    }

    *pill = malloc(sizeof(SDL_Rect) * (*pillCount));

    int pillIndex = 0;
    for (int row = 0; row < mapHeightInBlocks - 1; ++row) {
        for (int col = 0; col < mapWidthInBlocks - 1; ++col) {
            int index = coords(row, col, mapWidthInBlocks);
            if (mapArray[index] == '0' && mapArray[index + 1] == '0' && mapArray[index + mapWidthInBlocks] == '0' && mapArray[index + mapWidthInBlocks + 1] == '0') {
                if (isThisBigPill(bigPillIndexes, pillIndex)) {
                    (*pill)[pillIndex].h = (blockDimension / PILL_SIZE_COEFFICIENT) * 3;
                    (*pill)[pillIndex].w = (blockDimension / PILL_SIZE_COEFFICIENT) * 3;
                    (*pill)[pillIndex].x = ((*block)[index].x + (*block)[index + 1].w) - ((*pill)[pillIndex].w / 2);
                    (*pill)[pillIndex].y = ((*block)[index].y + (*block)[index + mapWidthInBlocks].h) - ((*pill)[pillIndex].h / 2);
                } else {
                    (*pill)[pillIndex].h = blockDimension / PILL_SIZE_COEFFICIENT;
                    (*pill)[pillIndex].w = blockDimension / PILL_SIZE_COEFFICIENT;
                    (*pill)[pillIndex].x = ((*block)[index].x + (*block)[index + 1].w) - ((*pill)[pillIndex].w / 2);
                    (*pill)[pillIndex].y = ((*block)[index].y + (*block)[index + mapWidthInBlocks].h) - ((*pill)[pillIndex].h / 2);
                }
                pillIndex++;
            }
        }
    }
}

bool isThisBigPill(int bigPillIndexes[], int pillIndex) {
    for (int i = 0; i < 4; ++i) {
        if (bigPillIndexes[i] == pillIndex) {
            return true;
        }
    }
    return false;
}

int collectPill(SDL_Rect** pill, SDL_Rect* pacman, int* pillCount, int pillsLeft, int bigPillIndexes[], bool* frightent){
    // Make the "hitbox" smaller and centered in the pacman. So it really appears that pacman eats the pill
    SDL_Rect hitBox = { .x = pacman->x + (pacman->w - (pacman->w/5)) / 2, .y = pacman->y + (pacman->h - (pacman->h/5)) / 2, .w = pacman->w/5, .h = pacman->h/5 };
    for (int i = 0; i < *pillCount; ++i) {
        if (SDL_HasIntersection(&hitBox, &(*pill)[i]) && !isThisBigPill(bigPillIndexes, i)) {
            (*pill)[i].h = 0; 
            (*pill)[i].w = 0;
            pillsLeft--;
        } else if (SDL_HasIntersection(&hitBox, &(*pill)[i]) && isThisBigPill(bigPillIndexes, i)) {
            // Make big pill side effects
            printf("Big pill has been eaten\n");
            *frightent = true;
            (*pill)[i].h = 0; 
            (*pill)[i].w = 0;
            pillsLeft--;
        }
    }
    return pillsLeft;
}

