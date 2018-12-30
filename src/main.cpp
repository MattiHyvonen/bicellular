#include "graphics.h"
#include "utility.h"

int main() {
    
    srand(time(NULL));
    
    //initialize
    initialize();

    //start rendering
    do{
        //clear();
        render();
        handleEvents();
    } while(running());
    
    return 0;
}
