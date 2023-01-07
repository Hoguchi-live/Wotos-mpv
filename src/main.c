#include "main.h"
#include "gfx/vao.h"
#include "gfx/vbo.h"
#include "glad/glad.h"

int main(int argc, char const *argv[])
{
    if (argc < 2){
        //std::cout << "Usage: " << argv[0] << " videofilename" << std::endl;
        return -1;
    }
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

    ////////// GLFW INIT
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    ///////// Create window
    if ((wwindow = glfwCreateWindow(window_width, window_height, "a-mpv", NULL, NULL)) == NULL){
        printf("ERROR::GLFW::Failed to create window\n");
        return -1;
    }
    glfwMakeContextCurrent(wwindow);
    glfwSetFramebufferSizeCallback(wwindow, framebuffer_size_callback);

    ///////// Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        printf("ERROR::GLAD::Failed to initialize GLAD\n");
        return -1;
    }

    //////////// Initializing placeholder for FT text glyphs
    //std::map<char, Character> Characters;
    //makeBitmaps(Characters);

    ///////// GL parameters
    glEnable(GL_DEPTH);
    glEnable(GL_MULTISAMPLE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    ///////// MPV INIT
    mpv = mpv_create();
    if (mpv_initialize(mpv) < MPV_ERROR_SUCCESS){
        printf("ERROR::MPV::Failed to initialize mpv\n");
        return -1;
    }

    mpv_opengl_init_params opengl_init_params={get_proc_address, NULL};
    int adv = 1; // we will use the update callback
    mpv_render_param render_param[] = {
        {MPV_RENDER_PARAM_API_TYPE, (char *)(MPV_RENDER_API_TYPE_OPENGL)},
        {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &opengl_init_params},
        {MPV_RENDER_PARAM_ADVANCED_CONTROL, &adv},
        {MPV_RENDER_PARAM_BLOCK_FOR_TARGET_TIME, NULL},
        {MPV_RENDER_PARAM_INVALID, NULL},
    };

    if (mpv_render_context_create(&mpv_ctx, mpv, render_param) < MPV_ERROR_SUCCESS){
        printf("ERROR::MPV::Failed to create MPV render context\n");
        return -1;
    }
    mpv_set_wakeup_callback(mpv, on_mpv_events, NULL);
    mpv_render_context_set_update_callback(mpv_ctx, on_mpv_render_update, NULL);

    const char *cmd[] = {"loadfile", argv[1], NULL};
    mpv_command(mpv, cmd);
    mpv_set_option_string(mpv, "loop", "");
    mpv_set_option_string(mpv, "gpu-api", "opengl");
    mpv_set_option_string(mpv, "hwdec", "auto");
    mpv_set_option_string(mpv, "vd-lavc-dr", "yes");
    //mpv_set_option_string(mpv, "terminal", "yes");
    // mpv_set_option_string(mpv, "video-timing-offset", "0"); // this need manual fps adjustment  mpv_render_frame_info()
    check_error(mpv_set_option_string(mpv, "input-default-bindings", "yes"));
    mpv_set_option_string(mpv, "input-vo-keyboard", "yes");
    int val = 1;
    check_error(mpv_set_option(mpv, "osc", MPV_FORMAT_FLAG, &val));




    struct Shader screenShader = shader_create(
            "res/shaders/screen_vs.glsl", "res/shaders/screen_fs.glsl",
            2, (struct VertexAttr[]){
                { .index = 0, .name = "pos" },
                { .index = 1, .name = "texCoords"}
                });


    //// TEST VAO REPLACE

    quadVAOTEST = vao_create();
    quadVBOTEST = vbo_create(GL_ARRAY_BUFFER, false);

    vao_bind(quadVAOTEST);
    vbo_bind(quadVBOTEST);

    vbo_buffer(quadVBOTEST, &quadVertices, 0, sizeof(quadVertices));
    vao_attr(quadVAOTEST, quadVBOTEST, 0, 3, GL_FLOAT, 5*sizeof(float), 0);
    vao_attr(quadVAOTEST, quadVBOTEST, 1, 2, GL_FLOAT, 5*sizeof(float), (3 * sizeof(float)));

    //Framebuffer for Video
    glGenFramebuffers(1, &video_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, video_framebuffer);

    // create a color attachment texture
    glGenTextures(1, &video_textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, video_textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbo_width, fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, video_textureColorbuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("ERROR::FRAMEBUFFER:: VIDEO Framebuffer #%d is not complete!\n", video_framebuffer);
    mpv_opengl_fbo mpv_fbo = {(int)(video_framebuffer), fbo_width, fbo_height, 0};
    int flip_y = 1;
    mpv_render_param params_fbo[] = {
        {MPV_RENDER_PARAM_OPENGL_FBO, &mpv_fbo},
        {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
        {MPV_RENDER_PARAM_INVALID, NULL}};

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
    while (!glfwWindowShouldClose(wwindow))
    {
        fcount++;
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //if (fcount % 100 == 0){
        //    std::cout << "FPS: " << 1 / deltaTime << "\n" << std::endl;
        //    }
        processGLFWInput(wwindow, mpv);
        // -----

        if (wakeup)
        {
            if ((mpv_render_context_update(mpv_ctx) & MPV_RENDER_UPDATE_FRAME))
            {
                mpv_render_context_render(mpv_ctx, params_fbo); // this "renders" to the video_framebuffer "linked by ID" in the params_fbo - BLOCKING
                glViewport(0, 0, window_width, window_height);  // fucky
            }
        }
        //screenShader->use();
        shader_bind(screenShader);

        //glBindVertexArray(quadVBO); // <-- The SCREEN QUAD
        vao_bind(quadVAOTEST);
        glBindTexture(GL_TEXTURE_2D, video_textureColorbuffer); // <-- SCREEN Colorbuffer IS THE TEXTURE
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // -----
        if (wakeup)
        {
            mpv_render_context_report_swap(mpv_ctx);
            wakeup = 0;
        }

        //Distance during deltaTime for consistent translation

        //// Draw image
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //imgShader->use();
        //glBindVertexArray(imgVBO); // <-- The SCREEN QUAD
        //glBindTexture(GL_TEXTURE_2D, imgTex); // <-- SCREEN Colorbuffer IS THE TEXTURE

        //imgMatrix = glm::translate(imgMatrix, glm::vec3(0.5/deltaPix, 0.0f, 0.0f));
        //glUniformMatrix4fv(glGetUniformLocation(imgShader->ID, "transformation"), 1, GL_FALSE, glm::value_ptr(imgMatrix));

        //glDrawArrays(GL_TRIANGLES, 0, 6);


         // // Glyphs
        //transformation = glm::translate(transformation, glm::vec3(deltaPix , 0.0f, 0.0f));

        //glyphShader->use();

        //// TEMPORARY
        //glUniformMatrix4fv(glGetUniformLocation(glyphShader->ID, "transformation"), 1, GL_FALSE, glm::value_ptr(transformation));

        /////////////////////////////////////////// SOCKET

        //recv_bytes = recv(sock, buffer, RECV_BUFFER_SIZE, 0);
        //if(recv_bytes >= 1){
        //    danmakus.push_back({buffer, 1, (float)window_width, 400.0f, 1, 0, 0});
        //}
        //std::cout << danmakus.front().text << std::endl;

        /////////////////////////////////////////// Danmaku update
        //updateDanmakus(r);

        //}

        glfwSwapBuffers(wwindow);
        glfwPollEvents();
        //usleep(10000); // we LIMIT the main render loop to 100FPS! If VSYSNC is enabled the limit is the VSYNC limit (~60fps)
    }
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

// Returns the address of the specified function (name) for the given context (ctx)
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
     //std::cout << "INFO::" << __func__ << std::endl;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // we have to rescale the Texture and renderbuffer storage.
    window_height = height;
    window_width = width;
    glBindTexture(GL_TEXTURE_2D, screen_textureColorbuffer);
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
//}
