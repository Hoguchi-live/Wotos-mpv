#include "main.h"
#include "gfx/vao.h"
#include "gfx/vbo.h"
#include "glad/glad.h"

static inline void check_error(int);

void init() {
    
}

void update() {
    processGLFWInput(wwindow, mpv);

    if (wakeup)
    {
        if ((mpv_render_context_update(mpv_ctx) & MPV_RENDER_UPDATE_FRAME))
        {
            mpv_render_context_render(mpv_ctx, renderer->params_fbo);                 
            glViewport(0, 0, window->size.x, window->size.y);
        }
    }
}

void render(){
    shader_bind(renderer->shaders[SHADER_MPV]);

    vao_bind(renderer->screenVAO);
    glBindTexture(GL_TEXTURE_2D, renderer->video_textureColorbuffer); // <-- SCREEN Colorbuffer IS THE TEXTURE
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // -----
    if (wakeup)
    {
        mpv_render_context_report_swap(mpv_ctx);
        wakeup = 0;
    }

    const char *text = "I Joe you Joe ;)";
    render_text(renderer, text, strlen(text), 0, 0, 0.001, (float [3]){0.0, 1.0, 0.0});
}

int main(int argc, char const *argv[])
{
    if (argc < 2){
        //std::cout << "Usage: " << argv[0] << " videofilename" << std::endl;
        return -1;
    }


    /*
       Create the main window
       wwindow is temporary
    */
    FWindow nof = NULL;
    window = window_create(init, nof, nof, update, render);

    wwindow = window->handle;

    /*  
        Set up the MPV player 
        The mpv and mpv_ctx variables are temporary
    */
    player = player_create();
    player_init(player);
    mpv = player->handle;
    mpv_ctx = player->ctx;


    /*  Setting the renderer
    */
    renderer = renderer_create();
    renderer_init(renderer);

    /* 
       Glyph shenanigans
    */
    //mat4s transformation = glms_ortho(0.0f, (float)(window_width), 0.0f, (float)window_height, -100, 100);
    


    // this doesn't work when called from inside the Renderer struct...
    int flip_y = 1;
    renderer->params_fbo = (mpv_render_param [3]){
        {MPV_RENDER_PARAM_OPENGL_FBO, &(renderer->mpv_fbo)},
        {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
        {MPV_RENDER_PARAM_INVALID, NULL}};

    /*  
        Start video playback
    */
    player_loadfile(player, argv[1]);


    window_loop();

    while (!glfwWindowShouldClose(wwindow))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;



        glfwSwapBuffers(wwindow);
        glfwPollEvents();
    }
    renderer_destroy(renderer);
    player_destroy(player);
    mpv_render_context_free(mpv_ctx);
    mpv_terminate_destroy(mpv);
    glfwTerminate();



    return 0;
}

void processGLFWInput(GLFWwindow *window, mpv_handle *ctx)
{
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_FALSE);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        const char *c[] = {"show-text", "lol", NULL};
        check_error(mpv_command(ctx, c));
    }

}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // we have to rescale the Texture and renderbuffer storage.
    window_height = height;
    window_width = width;
    glBindTexture(GL_TEXTURE_2D, renderer->screen_textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindRenderbuffer(GL_RENDERBUFFER, screen_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height);
}

static inline void check_error(int status)
{
    if (status < 0) {
        printf("mpv API error: %s\n", mpv_error_string(status));

        //exit(1);
    }
}
//
///** Returns true on success, or false if there was an error */
//bool SetSocketBlockingEnabled(int fd, bool blocking)
//{
//   if (fd < 0) return false;
//
//#ifdef _WIN32
//   unsigned long mode = blocking ? 0 : 1;
//   return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false;
//#else
//   int flags = fcntl(fd, F_GETFL, 0);
//   if (flags == -1) return false;
//   flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
//   return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
//#endif



    //////////// SOCKET SERVER
    //// create socket file descriptor
    //int server_fd;
    //if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    //{
    //    perror("socket failed");
    //    exit(EXIT_FAILURE);
    //}

    //// create socket address
    //// forcefully attach socket to the port
    //struct sockaddr_in address;
    //int opt = 1;
    //if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    //{
    //    perror("setsockopt failed");
    //    exit(EXIT_FAILURE);
    //}
    //address.sin_family = AF_INET;
    //address.sin_addr.s_addr = INADDR_ANY;
    //address.sin_port = htons(SERVER_PORT);

    //// bind socket to address
    //if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    //{
    //    perror("bind failed");
    //    exit(EXIT_FAILURE);
    //}


    //// listen to incoming connections
    //if (listen(server_fd, QUEUE_LENGTH) < 0)
    //{
    //    perror("listen failed");
    //    exit(EXIT_FAILURE);
    //}
//}

//// BUNCH OF CRAP
    ////////// IMAGE TEST
    //// Import image
    //int imgWidth, imgHeight, imgChan;
    //stbi_set_flip_vertically_on_load(true);
    //unsigned char* bytes = stbi_load("./assets/miporin.png", &imgWidth, &imgHeight, &imgChan, 0);

    //GLuint imgTex;
    //glGenTextures(1, &imgTex);
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, imgTex);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    ////Load image in texture
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    //glGenerateMipmap(GL_TEXTURE_2D);

    //unsigned int imgVAO, imgVBO;
    //glGenVertexArrays(1, &imgVAO);
    //glGenBuffers(1, &imgVBO);
    //glBindVertexArray(imgVAO);
    //glBindBuffer(GL_ARRAY_BUFFER, imgVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(imgVertices), &imgVertices, GL_STATIC_DRAW);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0); // coordinates
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float))); //texture
    //glEnableVertexAttribArray(1);

    //Shader *imgShader = new Shader("shaders/img_vs.glsl", "shaders/img_fs.glsl");
    //imgShader->use();
    //GLuint texture1 = glGetUniformLocation(imgShader->ID, "texture1");
    //glUniform1i(texture1, 0);

    //glm::mat4 imgMatrix(1.0f); // = glm::ortho(0.0f, static_cast<float>(window_width), 0.0f, static_cast<float>(window_height));
    //imgMatrix = glm::scale(imgMatrix, glm::vec3(0.25f, 0.25f ,0.25f));
    //imgMatrix = glm::translate(imgMatrix, glm::vec3(-15.0f, 0.0f, 0.0f));




    ////////// Text-rendering configuration
    //unsigned int VAO, VBO;
    //glGenVertexArrays(1, &VAO);
    //glGenBuffers(1, &VBO);
    //glBindVertexArray(VAO);
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);

    //Shader *glyphShader = new Shader("shaders/glyph_vs.glsl", "shaders/glyph_fs.glsl");
    //glm::mat4 transformation = glm::ortho(0.0f, static_cast<float>(window_width), 0.0f, static_cast<float>(window_height));

    ////////// Vertical spacing for danmakus and bool freespace array
    //const uint nb_danmakuRows = (uint)(
    //                        (float)(window_height -
    //                                DANMAKU_MARGIN_TOP -
    //                                DANMAKU_MARGIN_TOP) /
    //                        (float)(GLYPH_SIZE + DANMAKU_PADDING_V));

    //uint *ph_danmakuRows = new uint[nb_danmakuRows];
    //std::fill_n(ph_danmakuRows, nb_danmakuRows, 1);


    ////////// Other params
    //float deltaPix = 0.0f;
    //float danmakuNextYPos  = (float)window_height-100.0f;
    //int danmakuLine = 1;
    //int danmakuMaxLines = 10;
    //float danmakuDeltaY = 60.0f;
    //float glyphXPos = 0;

    //int isAvailable = 1;
    //uint nextDanmakuRow = 0;
    //uint danCount = 0;

    ///// WAIT FOR SOCKET
    //// accept a connection
    //int sock;
    //int addrlen = sizeof(address);
    //if ((sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    //{
    //    perror("accept failed");
    //    exit(EXIT_FAILURE);
    //}
    //else {
    //    std::cout << "accepted" << std::endl;
    //}

    //int rr = SetSocketBlockingEnabled(sock, 0);
    //std::cout << "Blocking: " << rr << std::endl;

    //char buffer[RECV_BUFFER_SIZE];
    //int recv_bytes;

    ////Load fake danmakus
    //std::vector<Danmaku> danmakus;


    ////////// Packing infos
    //Render r = {window_height, window_width,
    //            screenShader, glyphShader, imgShader
    //            };



    ////////////////////////////////////////////////////////////////////////////////
