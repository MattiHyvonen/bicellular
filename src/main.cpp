#include "graphics.h"

int main() {
    
    //initialize
    initialize();
    
    //start rendering
    do{
        clear();
        render();
    } while(keepRunning());
    
    return 0;
}
