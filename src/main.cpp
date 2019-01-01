#include "graphics.h"
#include "utility.h"

int main(int argc, char** argv) {
    
    //argument 2 is path for recording. 
    //If provided, record. If omitted, don't record.
    std::string recordPath = "";
    bool record = false;
    if(argc > 2) {
        record = true;
        recordPath = std::string(argv[2]);
    }
    
    //argument 1 is random seed
    int seed = 0;
    if(argc > 1)
        seed = std::atoi(argv[1]);
    else
        seed = time(NULL);
    
    
    std::cout << "random seed: " << seed << "\n";
    srand(seed);
    
    //initialize
    initialize();

    //start rendering
    do{
        render();
        //save frame, quit when finished or if there was an error
        if(record) {
            //start at frame 250, render 500 frames, no frame skip
            if(!saveFrame(recordPath, 500, 500, 0) )
                break;
        }
        handleEvents();
    } while(running());
    
    return 0;
}
