#include "player.h"

static void *get_proc_address(void *ctx, const char *name);
static void on_mpv_render_update(void *ctx);
static void on_mpv_events(void *ctx);
static inline void check_error(int);

void player_init(struct Player *self) {

    *self = (struct Player) {0};
    self->handle = mpv_create();

    if (mpv_initialize(self->handle) < MPV_ERROR_SUCCESS){
        printf("ERROR::MPV::Failed to initialize mpv\n");
        exit(1);
    }

    mpv_opengl_init_params opengl_init_params = {get_proc_address, NULL};
    int adv = 1; // we will use the update callback
    mpv_render_param render_param[] = {
        {MPV_RENDER_PARAM_API_TYPE, (char *)(MPV_RENDER_API_TYPE_OPENGL)},
        {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &opengl_init_params},
        {MPV_RENDER_PARAM_ADVANCED_CONTROL, &adv},
        {MPV_RENDER_PARAM_BLOCK_FOR_TARGET_TIME, NULL},
        {MPV_RENDER_PARAM_INVALID, NULL},
    };

    if (mpv_render_context_create(&(self->ctx), self->handle, render_param) < MPV_ERROR_SUCCESS){
        printf("ERROR::MPV::Failed to create MPV render context\n");
        exit(1);
    }

    mpv_set_wakeup_callback(self->handle, on_mpv_events, NULL);
    mpv_render_context_set_update_callback(self->ctx, on_mpv_render_update, NULL);

    mpv_set_option_string(self->handle, "loop", "");
    mpv_set_option_string(self->handle, "gpu-api", "opengl");
    mpv_set_option_string(self->handle, "hwdec", "auto");
    mpv_set_option_string(self->handle, "vd-lavc-dr", "yes");
    mpv_set_option_string(self->handle, "terminal", "yes");
    // mpv_set_option_string(mpv, "video-timing-offset", "0"); // this need manual fps adjustment  mpv_render_frame_info()
    check_error(mpv_set_option_string(self->handle, "input-default-bindings", "yes"));
    mpv_set_option_string(self->handle, "input-vo-keyboard", "yes");
    int val = 1;
    check_error(mpv_set_option(self->handle, "osc", MPV_FORMAT_FLAG, &val));
}

struct Player* player_create() {
    return malloc(sizeof(struct Player));
}

void player_destroy(struct Player* self) {
    free(self);
}


static void *get_proc_address(void *ctx, const char *name)
{
    glfwGetCurrentContext();
    return (void *)(glfwGetProcAddress(name));
}

static void on_mpv_render_update(void *ctx)
{
    // we set the wakeup flag here to enable the mpv_render_context_render path in the main loop.
    wakeup = 1;
}

static void on_mpv_events(void *ctx)
{
     printf("INFO::%s\n",__func__);
}

static inline void check_error(int status) 
{
    if(status < 0) {
        printf("MPV API error: %s\n", mpv_error_string(status));
        exit(1);
    }
}

void player_loadfile(struct Player* self, const char *file){
    const char *cmd[] = {"loadfile", file, NULL};
    mpv_command(self->handle, cmd);
}
