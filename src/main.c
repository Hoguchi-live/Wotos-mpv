#include "main.h"

bool SetSocketBlockingEnabled(int fd, bool blocking);

void _setup(){
    sock = socket_init_start();
    //window = window_create(init, destroy, tick, update, render);
    //player = player_create();
    //renderer = renderer_create();

    //player_init(player);
    //renderer_init(renderer); 

    /* Glyph shenanigans */
    //mat4s transformation = glms_ortho(0.0f, (float)(window_width), 0.0f, (float)window_height, -100, 100);
}

int main(int argc, char const *argv[]){
    if (argc < 2){ return -1;}
    _setup();

    //player_loadfile(player, argv[1]);
    //window_loop();
    return 0;
}


