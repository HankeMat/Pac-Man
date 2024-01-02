#include "headers/ui.h"

bool buttonClicked(int mouseX, int mouseY, SDL_Rect button){
    if(mouseX > button.x && mouseX < (button.x + button.w) && mouseY > button.y && mouseY < (button.y + button.h)){
        return true;
    } else {
        return false;
    }
    return false;
}