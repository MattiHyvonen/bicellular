#include "graphics.h"

int main() {
    
    //initialize
    initialize();
    
    //start rendering
    do{
        clear();
        render();
        handleEvents();
    } while(running());
    
    return 0;
}
