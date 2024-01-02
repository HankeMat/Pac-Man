#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#include "headers/ghosts.h"
#include "headers/map.h"
#include "headers/pacman.h"
#include "headers/sdl.h"
#include "headers/ui.h"

#define MAP_MARGIN 100

SDL_Texture* getImage(SDL_Context* init, char* pathToFile) {
    SDL_Surface* Srfc = SDL_LoadBMP(pathToFile);
    SDL_Texture* Texture = SDL_CreateTextureFromSurface(init->renderer, Srfc);
    SDL_FreeSurface(Srfc);

    return Texture;
}

SDL_Texture* getImageWithoutBackground(SDL_Renderer* renderer, char *pathToFile){
    SDL_Surface* Srfc = SDL_LoadBMP(pathToFile);

    Uint32 BlackColor = SDL_MapRGB(Srfc->format, 0, 0, 0);
    SDL_SetColorKey(Srfc, SDL_ENABLE, BlackColor);

    SDL_Texture* Texture = SDL_CreateTextureFromSurface(renderer, Srfc); 
    SDL_FreeSurface(Srfc); 

    return Texture;
}

void clearBcg(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}

bool warningTexture(SDL_Texture* texture) {
    if (!texture) {
        fprintf(stderr, "Texture could not be loaded!\n");
        return false;
    } else {
        return true;
    }
    return false;
}
bool warningFile(FILE* file) {
    if (!file) {
        fprintf(stderr, "File could not be opened!\n");
        return false;
    } else {
        return true;
    }
    return false;
}
bool warningContext(SDL_Context* context) {
    if (!context) {
        fprintf(stderr, "Context was not created!\n");
        return false;
    } else {
        return true;
    }
    return false;
}

void cleanUp(SDL_Texture* blinkyTextures[2], SDL_Texture* pinkyTextures[2], SDL_Texture* inkyTextures[2], SDL_Texture* clydeTextures[2], SDL_Texture* eyesTextures[4], SDL_Texture* pacmanTexture, SDL_Rect* block, SDL_Texture* frightentGhost[2]) {
    // Cleanup Ghost textures
    for (int i = 0; i < 2; i++) {
        SDL_DestroyTexture(blinkyTextures[i]);
        SDL_DestroyTexture(pinkyTextures[i]);
        SDL_DestroyTexture(inkyTextures[i]);
        SDL_DestroyTexture(clydeTextures[i]);
    }

    // Cleanup Ghost frightent textures
    for (int i = 0; i < 2; ++i) {
        SDL_DestroyTexture(frightentGhost[i]);                                
    }

    // Cleanup Eyes textures
    for (int i = 0; i < 4; i++) {
        SDL_DestroyTexture(eyesTextures[i]);
    }

    // Cleanup Pac-Man texture
    SDL_DestroyTexture(pacmanTexture);

    // Free dynamically allocated memory
    free(block);
}

int countDigits(int number) {
    int count = 0;

    do {
        number /= 10;
        count++;
    } while (number != 0);

    return count;
}

int main() {
    srand(time(NULL));
    // ============================================ Declaration of variables used in infinite while ============================================ //
    SDL_Event event;
    PacMan pacman;
    SDL_Rect* pill;
    SDL_Rect* block;
    Ghost blinky, pinky, inky, clyde;
    bool load = false;
    int running = true;
    int curPacmanFrame = 0;
    int curGhostFrame = 0;
    int frame = 0;
    int pillsLeft;
    int pillCount = 0;
    int blockDimension = 0;
    int firstTime = 1;
    uint32_t points = 0;
    int pacmanLives = 3;
    char* mapArray;
    char pointsText[30];
    int bigPillIndex[4] = {-1,-1,-1,-1};
    bool dontInitPills = false;
    bool frightentImpulse = false;
    long timerStart;
    long timerInterval;
    long timer;
    bool startTimer = false;

    // Context init (Window, Renderer)
    int windowWidth = 1200;
    int windowHeight = 600;
    SDL_Context init = sdl_context_init("Pac-Man", windowWidth, windowHeight);
    running = warningContext(&init);
    int mapHeightInBlocks, mapWidthInBlocks;

    // Textures
    SDL_Texture* bigPillTexture = getImageWithoutBackground(init.renderer, "src/images/map/big-0.bmp");
    SDL_Texture* pacmanTexture[4];
    SDL_Texture* eyes[4];
    SDL_Texture* pillTexture;
    SDL_Texture* frightentGhost[2];

    // TTF/UI init (Font)
    TTF_Init();
    TTF_Font* font = TTF_OpenFont("src/fonts/pacman.ttf", 20);
    SDL_Color scoreColor = {.r = 128, .g = 11, .b = 3, .a = 255};
    SDL_Rect scoreRect = {.w = windowWidth / 4, .x = (windowWidth - MAP_MARGIN) - windowWidth / 4, .h = windowHeight / 8, .y = 20};
    SDL_Rect escExitRect = {.w = windowWidth / 8, .x = 20, .h = windowHeight / 14, .y = 20};
    SDL_Rect livesTextRect = {.w = windowWidth / 6, .x = (windowWidth - MAP_MARGIN) - windowWidth / 4, .h = windowHeight / 10, .y = 40 + (windowHeight / 8) + (windowHeight / 10)};
    SDL_Color escColor = {.r = 46, .g = 2, .b = 160, .a = 255};
    SDL_Color livesTextColor = {.r = 255, .g = 255, .b = 0, .a = 255};

    // ============================================ Delta time init ============================================ //
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;

    // ============================================ Scene handle ============================================ //
    int scene = 0;
    // ============================================ Inifinite loop ============================================ //
    clearBcg(init.renderer);
    while (running) {
        switch (scene) {
            case 0:;
                // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SCENE 0 (Menu) <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< //
                SDL_Texture* menuBcgTexture = getImage(&init, "src/images/ui/menu/MenuBackGround.bmp");
                running = warningTexture(menuBcgTexture);
                SDL_RenderCopy(init.renderer, menuBcgTexture, NULL, NULL);

                SDL_Texture* startButton = getImage(&init, "src/images/ui/menu/Start.bmp");
                SDL_Rect startButtonRect = {.x = (windowWidth / 2) - ((windowWidth / 4) / 2), .y = (windowHeight / 2) - ((windowHeight / 8) / 2), .w = windowWidth / 4, .h = windowHeight / 8};

                SDL_Texture* exitButton = getImage(&init, "src/images/ui/menu/exit.bmp");
                SDL_Rect exitButtonRect = {.x = (windowWidth / 2) - ((windowWidth / 4) / 2), .y = (windowHeight / 1.2) - ((windowHeight / 8) / 2), .w = windowWidth / 4, .h = windowHeight / 8};

                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        SDL_DestroyTexture(menuBcgTexture);
                        SDL_DestroyTexture(startButton);
                        SDL_DestroyTexture(exitButton);
                        running = false;
                    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                        int mouseX, mouseY;
                        SDL_GetMouseState(&mouseX, &mouseY);
                        if (buttonClicked(mouseX, mouseY, startButtonRect)) {
                            scene = 1;
                            SDL_DestroyTexture(menuBcgTexture);
                            SDL_DestroyTexture(startButton);
                            SDL_DestroyTexture(exitButton);
                        } else if (buttonClicked(mouseX, mouseY, exitButtonRect)) {
                            SDL_DestroyTexture(menuBcgTexture);
                            SDL_DestroyTexture(startButton);
                            SDL_DestroyTexture(exitButton);
                            running = false;
                        }
                    }
                }
                SDL_RenderCopy(init.renderer, startButton, NULL, &startButtonRect);
                SDL_RenderCopy(init.renderer, exitButton, NULL, &exitButtonRect);
                SDL_RenderPresent(init.renderer);
                break;
            case 1:;
                // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SCENE 1 (Maps) <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< //
                SDL_Texture* mapsBcgTexture = getImage(&init, "src/images/ui/clearBcg.bmp");
                SDL_RenderCopy(init.renderer, mapsBcgTexture, NULL, NULL);
                SDL_Texture* map1 = getImage(&init, "src/images/ui/menu/map1.bmp");
                SDL_Rect map1Rect = {.x = (windowWidth / 3.8) - ((windowWidth / 4) / 2), .y = (windowHeight / 1.8) - ((windowHeight / 4) / 2), .w = windowWidth / 4, .h = windowHeight / 2};
                SDL_Texture* map2 = getImage(&init, "src/images/ui/menu/map2.bmp");
                SDL_Rect map2Rect = {.x = (windowWidth / 1.4) - ((windowWidth / 4) / 2), .y = (windowHeight / 1.95) - ((windowHeight / 4) / 2), .w = windowWidth / 4, .h = windowHeight / 1.7};
                char mapPath[50] = {"default"};

                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        SDL_DestroyTexture(mapsBcgTexture);
                        SDL_DestroyTexture(map1);
                        SDL_DestroyTexture(map2);
                        running = false;
                    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                        int mouseX, mouseY;
                        SDL_GetMouseState(&mouseX, &mouseY);
                        if (buttonClicked(mouseX, mouseY, map1Rect)) {
                            strcpy(mapPath, "src/maps/map1.txt");
                            load = true;
                            scene = 2;
                            SDL_DestroyTexture(mapsBcgTexture);
                            SDL_DestroyTexture(map1);
                            SDL_DestroyTexture(map2);
                        } else if (buttonClicked(mouseX, mouseY, map2Rect)) {
                            strcpy(mapPath, "src/maps/map2.txt");
                            load = true;
                            scene = 2;
                            SDL_DestroyTexture(mapsBcgTexture);
                            SDL_DestroyTexture(map1);
                            SDL_DestroyTexture(map2);
                        }
                    } else if (event.type == SDL_KEYDOWN) {
                        if (event.key.keysym.sym == SDLK_ESCAPE) {
                            scene = 0;
                            SDL_DestroyTexture(mapsBcgTexture);
                            SDL_DestroyTexture(map1);
                            SDL_DestroyTexture(map2);
                        }
                    }
                }

                SDL_RenderCopy(init.renderer, map1, NULL, &map1Rect);
                SDL_RenderCopy(init.renderer, map2, NULL, &map2Rect);
                SDL_RenderPresent(init.renderer);
                break;
            case 2:;
                // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SCENE 2 (Game) <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< //

                // INICIALIZATION
                if (load == true) {
                    frightentImpulse = false;
                    startTimer = false;
                    firstTime = 1;
                    // Load map
                    loadMap(mapPath, &mapHeightInBlocks, &mapWidthInBlocks, &mapArray);
                    blockDimension = (windowHeight - MAP_MARGIN * 2) / mapHeightInBlocks;

                    // Initialize Walls
                    initializeWalls(&block, mapHeightInBlocks, mapWidthInBlocks, blockDimension, MAP_MARGIN);

                    // Initialize Pills
                    if (!dontInitPills) {
                        pillTexture = getImageWithoutBackground(init.renderer, "src/images/map/pill.bmp");
                        initializePills(&block, &pill, &pillCount, mapArray, mapHeightInBlocks, mapWidthInBlocks, blockDimension, bigPillIndex);
                        pillsLeft = pillCount;
                    }
                    dontInitPills = false;
                
                    // Initialize Pac-Man
                    for (int i = 0; i < 3; ++i) {
                        char path[100];
                        sprintf(path, "src/images/pacman/pacman%d.bmp", i);
                        pacmanTexture[i] = getImageWithoutBackground(init.renderer, path);
                    }
                    pacmanTexture[3] = getImageWithoutBackground(init.renderer, "src/images/pacman/pacman1.bmp");
                    initializePacman(mapArray, &block, &pacman, mapWidthInBlocks, mapHeightInBlocks, blockDimension);

                    // Initialize eyes
                    eyes[0] = getGhostImage(init.renderer, "src/images/eyes/u.bmp");
                    eyes[1] = getGhostImage(init.renderer, "src/images/eyes/r.bmp");
                    eyes[2] = getGhostImage(init.renderer, "src/images/eyes/d.bmp");
                    eyes[3] = getGhostImage(init.renderer, "src/images/eyes/l.bmp");

                    // Initialize ghosts
                    blinky = initializeGhost('r', mapArray, &block, init.renderer, mapWidthInBlocks, mapHeightInBlocks, blockDimension);
                    pinky = initializeGhost('p', mapArray, &block, init.renderer, mapWidthInBlocks, mapHeightInBlocks, blockDimension);
                    inky = initializeGhost('b', mapArray, &block, init.renderer, mapWidthInBlocks, mapHeightInBlocks, blockDimension);
                    clyde = initializeGhost('y', mapArray, &block, init.renderer, mapWidthInBlocks, mapHeightInBlocks, blockDimension);
                    frightentGhost[0] = getImageWithoutBackground(init.renderer, "src/images/ghosts/f-0.bmp");
                    frightentGhost[1] = getImageWithoutBackground(init.renderer, "src/images/ghosts/f-1.bmp");

                    // Set default values
                    pacman.speed = 120.0;
                    pacman.accumX = 0;
                    pacman.accumY = 0;
                    pacman.wantedDir = 'x';
                    pacman.dir = 'x';

                    load = false;
                }

                // Catching an event
                while (SDL_PollEvent(&event)) {
                    // If event is type quit (crossing the window) then set running to zero which stops the main loop
                    if (event.type == SDL_QUIT) {
                        cleanUp(blinky.texture, pinky.texture, inky.texture, clyde.texture, eyes, *pacmanTexture, block, frightentGhost);
                        free(pill);
                        free(mapArray);
                        running = false;
                    } else if (event.type == SDL_KEYDOWN) {
                        switch (event.key.keysym.sym) {
                            case SDLK_ESCAPE:
                                cleanUp(blinky.texture, pinky.texture, inky.texture, clyde.texture, eyes, *pacmanTexture, block, frightentGhost);
                                free(pill);
                                free(mapArray);
                                mapArray = NULL;
                                running = false;
                                break;
                            case SDLK_UP:
                                pacman.wantedDir = 'u';
                                if (firstTime == 1) {
                                    wakeGhosts(&blinky, &pinky, &inky, &clyde);
                                    firstTime = 0;
                                }
                                break;
                            case SDLK_DOWN:
                                pacman.wantedDir = 'd';
                                if (firstTime == 1) {
                                    wakeGhosts(&blinky, &pinky, &inky, &clyde);
                                    firstTime = 0;
                                }
                                break;
                            case SDLK_LEFT:
                                pacman.wantedDir = 'l';
                                if (firstTime == 1) {
                                    wakeGhosts(&blinky, &pinky, &inky, &clyde);
                                    firstTime = 0;
                                }
                                break;
                            case SDLK_RIGHT:
                                pacman.wantedDir = 'r';
                                if (firstTime == 1) {
                                    wakeGhosts(&blinky, &pinky, &inky, &clyde);
                                    firstTime = 0;
                                }
                                break;
                            default:
                                break;
                        }
                    }
                }
                clearBcg(init.renderer);

                if (running == true) {
                    // ============================================ Delta time handleing ============================================ //
                    LAST = NOW;
                    NOW = SDL_GetPerformanceCounter();
                    double deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency());
                    // 75 FPS => 1/75 =>   13ms => 0.013s
                    // 60 FPS => 1/60 =>   16ms => 0.016s
                    // 1  FPS => 1/ 1 => 1000ms => 1.000s
                    // printf("Delta time: %f\n", (float)deltaTime);

                    // ============================================ Pills collecting ============================================ //
                    int tmp = pillsLeft;
                    pillsLeft = collectPill(&pill, &pacman.rect, &pillCount, pillsLeft, bigPillIndex, &frightentImpulse);
                    if (pillsLeft < tmp) {
                        points += 10;
                    }

                    if (frightentImpulse) {
                        timerStart = (long)SDL_GetTicks();
                        timerInterval = 10000;
                        startTimer = true;

                        turnGhosts(&blinky, &pinky, &inky, &clyde);
                        blinky.state = 1;
                        pinky.state = 1;
                        inky.state = 1;
                        clyde.state = 1;

                        frightentImpulse = false;
                    }

                    if (startTimer) {
                        long timeElapsed = (long)(SDL_GetTicks()) - timerStart;
                        timer = timerInterval - timeElapsed;

                        if (timer < 0) {
                            resetState('b', &blinky, &pinky, &inky, &clyde);
                            resetState('p', &blinky, &pinky, &inky, &clyde);
                            resetState('i', &blinky, &pinky, &inky, &clyde);
                            resetState('c', &blinky, &pinky, &inky, &clyde);
                            startTimer = false;
                        }
                    }

                    // ============================================ Draw the map ============================================ //
                    for (int i = 0; i < (mapHeightInBlocks * mapWidthInBlocks); ++i) {
                        if (mapArray[i] == '1') {
                            SDL_SetRenderDrawColor(init.renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
                            SDL_RenderFillRect(init.renderer, &block[i]);
                        } else if (mapArray[i] == '4') {
                            SDL_SetRenderDrawColor(init.renderer, 255, 192, 203, SDL_ALPHA_OPAQUE);
                            SDL_RenderFillRect(init.renderer, &block[i]);
                        }
                    }

                    // ============================================ Pac-Man's movement counting and prep. ============================================ //
                    ghostMove(&blinky, deltaTime, mapArray, &block, mapHeightInBlocks, mapWidthInBlocks);
                    ghostMove(&pinky, deltaTime, mapArray, &block, mapHeightInBlocks, mapWidthInBlocks);
                    ghostMove(&inky, deltaTime, mapArray, &block, mapHeightInBlocks, mapWidthInBlocks);
                    ghostMove(&clyde, deltaTime, mapArray, &block, mapHeightInBlocks, mapWidthInBlocks);

                    pacman.accumX += pacman.speed * deltaTime;
                    pacman.accumY += pacman.speed * deltaTime;

                    int moveX = (int)pacman.accumX;
                    int moveY = (int)pacman.accumY;

                    int newPacmanX = pacman.rect.x;
                    int newPacmanY = pacman.rect.y;

                    if (moveX > 1 || moveY > 1) {
                        if (pacman.wantedDir != 'x') {
                            int wantedTestPacmanX = pacman.rect.x;
                            int wantedTestPacmanY = pacman.rect.y;

                            switch (pacman.wantedDir) {
                                case 'u':
                                    wantedTestPacmanY -= moveY;
                                    break;
                                case 'd':
                                    wantedTestPacmanY += moveY;
                                    break;
                                case 'l':
                                    wantedTestPacmanX -= moveX;
                                    break;
                                case 'r':
                                    wantedTestPacmanX += moveX;
                                    break;
                                default:
                                    break;
                            }

                            if (!collisionWithWall(mapArray, wantedTestPacmanX, wantedTestPacmanY, &pacman, mapHeightInBlocks, mapWidthInBlocks, &block)) {
                                pacman.angle = setPacmanAngle(pacman.wantedDir);
                                pacman.dir = pacman.wantedDir;
                                pacman.wantedDir = 'x';
                            }
                        }

                        switch (pacman.dir) {
                            case 'u':
                                newPacmanY -= moveY;
                                break;
                            case 'd':
                                newPacmanY += moveY;
                                break;
                            case 'l':
                                newPacmanX -= moveX;
                                break;
                            case 'r':
                                newPacmanX += moveX;
                                break;
                            default:
                                break;
                        }

                        bool collisionDetected = collisionWithWall(mapArray, newPacmanX, newPacmanY, &pacman, mapHeightInBlocks, mapWidthInBlocks, &block);
                        // Decide whether to actually move the pacman or not
                        if (!collisionDetected) {
                            // Moving pacman
                            pacman.rect.x = newPacmanX;
                            pacman.rect.y = newPacmanY;
                            // Clearing accumulated values
                            pacman.accumX -= moveX;
                            pacman.accumY -= moveY;
                        } else if (collisionDetected) {
                            // Clearing accumulated values
                            pacman.accumX -= moveX;
                            pacman.accumY -= moveY;
                        }
                    }

                    // ============================================ Animation handler ============================================ //
                    frame++;
                    if (frame >= 5) {
                        curPacmanFrame = (curPacmanFrame + 1) % 4;
                        curGhostFrame = (curGhostFrame + 1) % 2;
                        frame = 0;
                    }

                    // ============================================ Draw UI ============================================ //
                    FILE* highestScore = fopen("userData/highestScore.txt", "r");
                    char line[10];
                    fread(line, sizeof(line), 1, highestScore);
                    int pointsFromFile = atoi(line);
                    fclose(highestScore);
                    char highScore[21];
                    snprintf(highScore, 30, "High score: %d", pointsFromFile);
                    SDL_Rect highScoreText = {.w = windowWidth / 4, .x = ((windowWidth - MAP_MARGIN) - windowWidth / 4), .h = windowHeight / 10, .y = 80 + (windowHeight / 8) + (windowHeight / 10) + (windowHeight / 10) + (windowHeight / 10)};
                    sdl_draw_text(init.renderer, font, scoreColor, highScoreText, highScore);

                    snprintf(pointsText, 30, "%d", points);
                    int digits = countDigits(points);
                    SDL_Rect pointsRect = {.w = (windowWidth / 18) * digits, .x = (windowWidth - MAP_MARGIN) - windowWidth / 4, .h = windowHeight / 10, .y = 20 + windowHeight / 8};
                    sdl_draw_text(init.renderer, font, scoreColor, pointsRect, pointsText);
                    sdl_draw_text(init.renderer, font, scoreColor, scoreRect, "Score:");
                    sdl_draw_text(init.renderer, font, escColor, escExitRect, "ESC - EXIT");
                    sdl_draw_text(init.renderer, font, livesTextColor, livesTextRect, "Lives:");
                    int xOffset = 0;
                    for (int i = 0; i < pacmanLives; ++i) {
                        SDL_Rect pacmanLives = {.w = windowWidth / 15, .x = ((windowWidth - MAP_MARGIN) - windowWidth / 4) + xOffset, .h = windowHeight / 10, .y = 60 + (windowHeight / 8) + (windowHeight / 10) + (windowHeight / 10)};
                        SDL_RenderCopyEx(init.renderer, pacmanTexture[1], 0, &pacmanLives, 0.0, NULL, SDL_FLIP_NONE);
                        xOffset += (windowWidth / 15);
                    }

                    // ============================================ Draw pacman ============================================ //
                    SDL_RenderCopyEx(init.renderer, pacmanTexture[curPacmanFrame], 0, &pacman.rect, pacman.angle, NULL, SDL_FLIP_NONE);

                    // ============================================ Draw the pills ============================================ //
                    
                    int pillIndex = 0;
                    for (int row = 0; row < mapHeightInBlocks - 1; ++row) {
                        for (int col = 0; col < mapWidthInBlocks - 1; ++col) {
                            int index = coords(row, col, mapWidthInBlocks);
                            if (mapArray[index] == '0' && mapArray[index + 1] == '0' && mapArray[index + mapWidthInBlocks] == '0' && mapArray[index + mapWidthInBlocks + 1] == '0') {
                                // if pillIndex == bigPillIndex tak vykreslit velky pill a zvetsit pillIndex o jeden jinak vykreslit maly pill
                                if (isThisBigPill(bigPillIndex ,pillIndex)) {
                                    SDL_RenderCopy(init.renderer, bigPillTexture, 0, &pill[pillIndex]);
                                } else {
                                    SDL_RenderCopy(init.renderer, pillTexture, 0, &pill[pillIndex]);
                                }
                                pillIndex++;
                            }
                        }
                    }

                    // ============================================ Draw Ghosts ============================================ //
                    if (blinky.dir != -1 && pinky.dir != -1 && inky.dir != -1 && clyde.dir != -1) {
                        if (!isFrightent('b', blinky, pinky, inky, clyde)) {
                            SDL_RenderCopy(init.renderer, blinky.texture[curGhostFrame], 0, &blinky.rect);
                            SDL_RenderCopy(init.renderer, eyes[blinky.dir], 0, &blinky.rect);
                        } else {
                            SDL_RenderCopy(init.renderer, frightentGhost[curGhostFrame], 0, &blinky.rect);
                        }

                        if (!isFrightent('p', blinky, pinky, inky, clyde)) {
                            SDL_RenderCopy(init.renderer, pinky.texture[curGhostFrame], 0, &pinky.rect);
                            SDL_RenderCopy(init.renderer, eyes[pinky.dir], 0, &pinky.rect);
                        } else {
                            SDL_RenderCopy(init.renderer, frightentGhost[curGhostFrame], 0, &pinky.rect);
                        }

                        if (!isFrightent('i', blinky, pinky, inky, clyde)) {
                            SDL_RenderCopy(init.renderer, inky.texture[curGhostFrame], 0, &inky.rect);
                            SDL_RenderCopy(init.renderer, eyes[inky.dir], 0, &inky.rect);
                        } else {
                            SDL_RenderCopy(init.renderer, frightentGhost[curGhostFrame], 0, &inky.rect);
                        }

                        if (!isFrightent('c', blinky, pinky, inky, clyde)) {
                            SDL_RenderCopy(init.renderer, clyde.texture[curGhostFrame], 0, &clyde.rect);
                            SDL_RenderCopy(init.renderer, eyes[clyde.dir], 0, &clyde.rect);
                        } else {
                            SDL_RenderCopy(init.renderer, frightentGhost[curGhostFrame], 0, &clyde.rect);
                        }     
                    }

                    // Reseting when pacman dies or wins the round
                    char intersectionWith = ghostAndPacmanIntersection(pacman.rect, blinky, pinky, inky, clyde);
                    //printf("Intersection with : %c\n", intersectionWith);

                    if (intersectionWith != 'x' && !isFrightent(intersectionWith, blinky, pinky,inky, clyde)) {
                        pacmanLives--;
                        if (pacmanLives <= 0) {
                            // Save highest score into file
                            if (points > (uint32_t)pointsFromFile) {
                                FILE* highestScore = fopen("userData/highestScore.txt", "w");
                                
                                char pointsText[12]; 
                                snprintf(pointsText, sizeof(pointsText), "%u", points);
                                fwrite(pointsText, 1, sizeof(pointsText), highestScore);

                                fclose(highestScore);
                            }

                            /**
                             * @todo Pacman death animation and then destroy these textures as well
                             */
                            
                            cleanUp(blinky.texture, pinky.texture, inky.texture, clyde.texture, eyes, *pacmanTexture, block, frightentGhost);
                            free(pill);
                            free(mapArray);
                            clearBcg(init.renderer);
                            SDL_Color gameOverColor = {.r = 255, .g = 0, .b = 0, .a = 255};
                            SDL_Rect gameOverRect = {.h = windowHeight / 4, .w = windowWidth / 3, .x = ((windowWidth / 2) - ((windowWidth / 3) / 2)), .y = ((windowHeight / 2) - ((windowHeight / 4) / 2))};
                            sdl_draw_text(init.renderer, font, gameOverColor, gameOverRect, "Game Over");
                            if (points > (uint32_t)pointsFromFile) {
                                SDL_Color newHighScoreColor = {.r = 255, .g = 255, .b = 0, .a = 255};
                                SDL_Rect newHighScoreRect = {.h = windowHeight / 12, .w = windowWidth / 3, .x = ((windowWidth / 2) - ((windowWidth / 3) / 2)), .y = ((windowHeight / 2) - ((windowHeight / 4) / 2)) + (windowHeight / 4)};
                                char newHighScoreText[40];
                                sprintf(newHighScoreText, "New highest score: %d", points);
                                sdl_draw_text(init.renderer, font, newHighScoreColor, newHighScoreRect, newHighScoreText);
                            }
                            running = false;
                            SDL_RenderPresent(init.renderer);
                            // Delay
                            SDL_Delay(5000);
                        } else {

                            /**
                             * @todo Pacman death animation and then destroy these textures as well
                             */

                            cleanUp(blinky.texture, pinky.texture, inky.texture, clyde.texture, eyes, *pacmanTexture, block, frightentGhost);
                            free(mapArray);
                            dontInitPills = true;
                            load = true;
                            intersectionWith = 'x';
                            // Delay that ignores even the event handleing
                            Uint32 startTime = SDL_GetTicks();
                            Uint32 delayTime = 2000;
                            while ((SDL_GetTicks() - startTime) < delayTime) {
                                SDL_PollEvent(&event);
                            };
                        }
                    } else if (intersectionWith != 'x' && isFrightent(intersectionWith, blinky, pinky,inky, clyde)) {
                        // Look for which ghost had intersection with pacman, destroy him (invisible and position back to spawn) after a while (timer) spawn ghost
                        //printf("Pacman ate ghost\n");
                        resetState(intersectionWith, &blinky, &pinky, &inky, &clyde);
                        resetPosition(intersectionWith, &blinky, &pinky, &inky, &clyde, mapArray, &block, mapWidthInBlocks, mapHeightInBlocks);
                    }

                    if (pillsLeft <= 0) {
                        cleanUp(blinky.texture, pinky.texture, inky.texture, clyde.texture, eyes, *pacmanTexture, block, frightentGhost);
                        free(pill);
                        free(mapArray);
                        SDL_Color stageClearColor = {.r = 224, .g = 227, .b = 64, .a = 255};
                        SDL_Rect stageClearRect = {.h = windowHeight / 4, .w = windowWidth / 3, .x = ((windowWidth / 2) - ((windowWidth / 3) / 2)), .y = ((windowHeight / 2) - ((windowHeight / 4) / 2))};
                        sdl_draw_text(init.renderer, font, stageClearColor, stageClearRect, "Stage clear");
                        SDL_RenderPresent(init.renderer);
                        load = true;

                        // Delay that ignores even the event handleing
                        Uint32 startTime = SDL_GetTicks();
                        Uint32 delayTime = 2000;
                        while (SDL_GetTicks() - startTime < delayTime) {
                            SDL_PollEvent(&event);
                        }
                    }

                    // Changes monitor view to second image (fully drown) - (double buffering)
                    SDL_RenderPresent(init.renderer);
                    break;   
                }    
        } 
    }

    TTF_CloseFont(font);
    TTF_Quit();
    sdl_context_free(&init);

    return 0;
}