#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>
#include <learnopengl/filesystem.h>
#include <stb_image.h>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/shader.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/shader_s.h>
#include <learnopengl/camera.h>
#include <learnopengl/mesh.h>
#include <learnopengl/model.h>

using namespace glm;

//velicina prozora
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 700;

bool gammaEnabled = true;
bool gammaKeyPressed = false;

//kamera deklaracija + mis
Camera kamera(vec3(0.0f, 0.3f, 3.0f));

bool first_mouse = true;
float last_X = SCR_WIDTH / 2.0f;
float last_Y = SCR_HEIGHT / 2.0f;

// timing
float delta_time = 0.0f;	// time between current frame and last frame
float last_frame = 0.0f;

//svetlost
vec3 lightPos(1.2f, 1.0f, 2.0f);

void fb_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow *window, double x_pozicija, double y_pozicija);
void scroll_callback(GLFWwindow *window, double x_offset, double y_offset);
void update(GLFWwindow* window);
unsigned int loadTexture(const char *path, bool gammaCorrection);


int main() {

    glfwInit();
    //verzija ce biti 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //necemo stare verzije
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow *window;
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Vaporfishdisco", nullptr, nullptr);
    if (window == nullptr){
        std::cout << "GPU nije kompatibilna sa 3.3 verzijom\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, fb_size_callback);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Ne mozemo inicijalizovati glad\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_MULTISAMPLE);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    //pravougaonik
    Shader pravougaonik_shader("resources/shaders/pravougaonik.vs", "resources/shaders/pravougaonik.fs");

    float vertices[] = {
            //pozicije              //boja                          //tekstura koordinate
            -0.7f, 0.7f, -0.5f,       0.7f, 1.0f, 1.0f, 1.0f,        1.0f, 1.0f,  //gore levo
            -0.7f, -0.7f, -0.5f,      1.0f, 0.7f, 1.0f, 1.0f,        1.0f, 0.0f, //dole levo
            0.7f, -0.7f, -0.5f,       1.0f, 1.0f, 0.7f, 1.0f,        0.0f, 0.0f,//gore desno
            0.7f, 0.7f, -0.5f,        1.0f, 1.0f, 1.0f, 1.0f,        0.0f, 1.0f//dole desno
    };


    unsigned int indices[] = {
            1, 2, 3,
            0, 1, 3
    };

    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)(3*sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)(7*sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Kocka

    Shader kocka_shader("resources/shaders/kocka.vs", "resources/shaders/kocka.fs");
    Shader screenShader("resources/shaders/aa_post.vs","resources/shaders/aa_post.fs");

    float vertices_kocka[] = {
            //koordinate tacaka     //koordinate teksture   //normale
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,             0.0f,  0.0f, -1.0f,
            0.5f, -0.5f, -0.5f,     1.0f, 0.0f,             0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,     1.0f, 1.0f,             0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,     1.0f, 1.0f,             0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,             0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,             0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,             0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,     1.0f, 0.0f,             0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,     1.0f, 1.0f,             0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,     1.0f, 1.0f,             0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,             0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,             0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,             -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,             -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,             -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,             -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,             -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,             -1.0f,  0.0f,  0.0f,

            0.5f,  0.5f,  0.5f,     1.0f, 0.0f,             1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,     1.0f, 1.0f,             1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,     0.0f, 1.0f,             1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,     0.0f, 1.0f,             1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,     0.0f, 0.0f,             1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,     1.0f, 0.0f,             1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,             0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,     1.0f, 1.0f,             0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,     1.0f, 0.0f,             0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,     1.0f, 0.0f,             0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,             0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,             0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,             0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,     1.0f, 1.0f,             0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,     1.0f, 0.0f,             0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,     1.0f, 0.0f,             0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,             0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,             0.0f,  1.0f,  0.0f
    };
    float quadVertices[] = {   // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
            1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f
    };

    //setup za kocku
    unsigned VBO_kocka, VAO_kocka;
    glGenVertexArrays(1, &VAO_kocka);
    glGenBuffers(1, &VBO_kocka);

    glBindVertexArray(VAO_kocka);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_kocka);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_kocka), vertices_kocka, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    //setup za screen quad
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


    //MSAA framebuffer
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // create a multisampled color attachment texture
    unsigned int textureColorBufferMultiSampled;
    glGenTextures(1, &textureColorBufferMultiSampled);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);

    // create a (also multisampled) renderbuffer object for depth and stencil attachments
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // configure second post-processing framebuffer
    unsigned int intermediateFBO;
    glGenFramebuffers(1, &intermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

    // create a color attachment texture
    unsigned int screenTexture;
    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);	// we only need a color buffer

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //shader
    kocka_shader.use();
    screenShader.setInt("screenTexture", 0);


    //LIGHT SOURCE
    Shader light_source_shader("resources/shaders/izvor_svetlosti.vs", "resources/shaders/izvor_svetlosti.fs");
    Shader light_edge("resources/shaders/izvor_svetlosti.vs", "resources/shaders/edge.fs");

    float vertices_light[] = {
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,

            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,

            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
    };
    //setup za svetlo
    unsigned VBO_light, VAO_light;
    glGenVertexArrays(1, &VAO_light);
    glGenBuffers(1, &VBO_light);

    glBindVertexArray(VAO_light);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_light);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_light), &vertices_light, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);


    //TEKSTURA - pravougaonik i kocka


    unsigned tekstura_pravougaonik = loadTexture(FileSystem::getPath("resources/textures/checkers.jpg").c_str(), false);
    unsigned tekstura_pravougaonik_gamma = loadTexture(FileSystem::getPath("resources/textures/checkers.jpg").c_str(), true);

    unsigned diffuseMap = loadTexture(FileSystem::getPath("resources/textures/vaporwave.jpg").c_str(), false);
    unsigned diffuseMap_gamma = loadTexture(FileSystem::getPath("resources/textures/vaporwave.jpg").c_str(), true);

    unsigned specularMap = loadTexture(FileSystem::getPath("resources/textures/vaporwave_specular.jpg").c_str(), false);

    kocka_shader.use();
    kocka_shader.setInt("material.diffuse", 0);
    kocka_shader.setInt("material.specular", 1);

    pravougaonik_shader.use();

    //Model!
    Shader model_shader("resources/shaders/model.vs", "resources/shaders/model.fs");
    Model model_fishy(FileSystem::getPath("resources/objects/fish/12265_Fish_v1_L2.obj"));

    //petlja renderovanja
    while(!glfwWindowShouldClose(window)) {
        //frame-time, vezano za kameru
        float curr_frame = glfwGetTime();
        delta_time = curr_frame -last_frame;
        last_frame = curr_frame;

        update(window);

        //KAMERA - POGLED
        mat4 pogled = mat4(1.0f);
        pogled = kamera.GetViewMatrix();

        //postavimo boju pozadine i ocistimo bafere da bi ta boja mogla da se vidi
        glClearColor(0.6f, 1.0f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // 1. draw scene as normal in multisampled buffers
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClearColor(0.6f, 1.0f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);

        kocka_shader.use();
        mat4 projection_light = perspective(radians(kamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        vec3 pointLightPositions[] = {
                vec3( 2.0*sin((float)glfwGetTime()),  1.0f,  2.0*cos((float)glfwGetTime())),
                vec3( -1.2*sin((float)glfwGetTime()), -1.0f, -1.2*cos((float)glfwGetTime())),
                vec3(-4.0*sin((float)glfwGetTime()),  2.0f, -9.0*cos((float)glfwGetTime())),
        };
        glBindVertexArray(VAO_kocka);
        lightPos = vec3(1.2 * cos(curr_frame), 1.0f, 2.0 * sin(curr_frame)); //rotacija svetlosti


        for (unsigned int i = 0; i < 3; i++)
        {
            light_edge.use();

            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);

            light_edge.setMat4("projection", projection_light);
            light_edge.setMat4("view", pogled);

            light_source_shader.use();

            light_source_shader.setMat4("projection", projection_light);
            light_source_shader.setMat4("view", pogled);

            mat4 model = mat4(1.0f);
            model = translate(model, pointLightPositions[i]);
            model = scale(model, vec3(0.2f));
            light_source_shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            glDisable(GL_DEPTH_TEST);
            light_edge.use();
            model = scale(model, vec3(1.1f));
            light_edge.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glEnable(GL_DEPTH_TEST);
        }


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gammaEnabled ?  tekstura_pravougaonik_gamma : tekstura_pravougaonik);

        //PROJEKCIJA - PRAVOUGAONIK

        pravougaonik_shader.use();

        mat4 model_pravougaonik = mat4(1.0f);
        model_pravougaonik = rotate(model_pravougaonik,  radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
        mat4 projection_pravougaonik = mat4(1.0f);
        projection_pravougaonik = perspective(radians(kamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        pravougaonik_shader.setInt("texture1", 0);

        pravougaonik_shader.setMat4("model_pravougaonik", model_pravougaonik);
        pravougaonik_shader.setMat4("view_pravougaonik", pogled);
        pravougaonik_shader.setMat4("projection_pravougaonik", projection_pravougaonik);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //MODEL

        model_shader.use();

        //direkciono
        model_shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        model_shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        model_shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        model_shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        model_shader.setVec3("pointLights[0].position", pointLightPositions[0]);
        model_shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        model_shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        model_shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        model_shader.setFloat("pointLights[0].constant", 1.0f);
        model_shader.setFloat("pointLights[0].linear", 0.09);
        model_shader.setFloat("pointLights[0].quadratic", 0.032);
        // point light 2
        model_shader.setVec3("pointLights[1].position", pointLightPositions[1]);
        model_shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        model_shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        model_shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        model_shader.setFloat("pointLights[1].constant", 1.0f);
        model_shader.setFloat("pointLights[1].linear", 0.09);
        model_shader.setFloat("pointLights[1].quadratic", 0.032);
        // point light 3
        model_shader.setVec3("pointLights[2].position", pointLightPositions[2]);
        model_shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        model_shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        model_shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        model_shader.setFloat("pointLights[2].constant", 1.0f);
        model_shader.setFloat("pointLights[2].linear", 0.09);
        model_shader.setFloat("pointLights[2].quadratic", 0.032);
        // point light
        model_shader.setVec3("pointLights[3].position", pointLightPositions[3]);
        model_shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        model_shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        model_shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        model_shader.setFloat("pointLights[3].constant", 1.0f);
        model_shader.setFloat("pointLights[3].linear", 0.09);
        model_shader.setFloat("pointLights[3].quadratic", 0.032);
        //materijal
        model_shader.setFloat("material.shininess", 90.0f);


        mat4 projection_model = perspective(radians(kamera.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
        mat4 model_model = mat4(1.0f);
        model_model = translate(model_model, vec3(0.0f, 1.0f, 0.0f));
        model_model = scale(model_model, vec3(0.03f, 0.03f, 0.03f));
        model_model = rotate(model_model, radians(-90.0f),vec3(1.0f, 0.0f, 0.0f));

        model_shader.setMat4("projection", projection_model);
        model_shader.setMat4("view", pogled);
        model_shader.setMat4("model", model_model);
        model_fishy.Draw(model_shader);


        // PROJEKCIJA - KOCKA

        kocka_shader.use();
        light_source_shader.setInt("material.diffuse", 0);

        //svetlost kocka
        kocka_shader.use();
        //uniforms
        kocka_shader.setVec3("viewPos", kamera.Position);
        //direkciono
        kocka_shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        kocka_shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        kocka_shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        kocka_shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        kocka_shader.setVec3("pointLights[0].position", pointLightPositions[0]);
        kocka_shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        kocka_shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        kocka_shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        kocka_shader.setFloat("pointLights[0].constant", 1.0f);
        kocka_shader.setFloat("pointLights[0].linear", 0.09);
        kocka_shader.setFloat("pointLights[0].quadratic", 0.032);
        // point light 2
        kocka_shader.setVec3("pointLights[1].position", pointLightPositions[1]);
        kocka_shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        kocka_shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        kocka_shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        kocka_shader.setFloat("pointLights[1].constant", 1.0f);
        kocka_shader.setFloat("pointLights[1].linear", 0.09);
        kocka_shader.setFloat("pointLights[1].quadratic", 0.032);
        // point light 3
        kocka_shader.setVec3("pointLights[2].position", pointLightPositions[2]);
        kocka_shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        kocka_shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        kocka_shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        kocka_shader.setFloat("pointLights[2].constant", 1.0f);
        kocka_shader.setFloat("pointLights[2].linear", 0.09);
        kocka_shader.setFloat("pointLights[2].quadratic", 0.032);
        // point light 4
        kocka_shader.setVec3("pointLights[3].position", pointLightPositions[3]);
        kocka_shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        kocka_shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        kocka_shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        kocka_shader.setFloat("pointLights[3].constant", 1.0f);
        kocka_shader.setFloat("pointLights[3].linear", 0.09);
        kocka_shader.setFloat("pointLights[3].quadratic", 0.032);
        //materijal
        kocka_shader.setFloat("material.shininess", 90.0f);

        kocka_shader.setInt("gamma", true);

        //proj
        kocka_shader.setMat4("projection", projection_light);
        kocka_shader.setMat4("view", pogled);
        mat4 model_kocka = mat4(1.0f);
        //model_kocka = rotate(model_kocka, radians(-45.0f), vec3(0.0f, 1.0f, 0.0f));
        kocka_shader.setMat4("model", model_kocka);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gammaEnabled ? diffuseMap_gamma : diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glBindVertexArray(VAO_kocka);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 2. now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // 3. now render quad with scene's visuals as its texture image
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);

        //iscrtavanje Screen quad
        screenShader.use();
        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO_kocka);
    glDeleteBuffers(1, &VBO_kocka);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void fb_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0,0,width,height);
}

//pomeranje misa
void mouse_callback(GLFWwindow *window, double x_pozicija, double y_pozicija){
    if(first_mouse) {
        last_X = x_pozicija;
        last_Y = y_pozicija;
        first_mouse = false;
    }
    float x_offset = x_pozicija - last_X;
    float y_offset = last_Y - y_pozicija ; //naopako jer y-koordinate idu od dna ka vrhu
    last_X = x_pozicija;
    last_Y = y_pozicija;

    kamera.ProcessMouseMovement(x_offset, y_offset);
}

//TODO: nmp ni sta ovo radi protumacicu ga valjda lmao
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods){
}

//zoom
void scroll_callback(GLFWwindow *window, double x_offset, double y_offset){
    kamera.ProcessMouseScroll(y_offset);
}

//WASD, ESC
void update(GLFWwindow* window) {
    //ako kliknemo escape izlazimo iz prozora
    if(glfwGetKey(window, GLFW_KEY_ESCAPE)==GLFW_PRESS)
        glfwSetWindowShouldClose(window,true);
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        kamera.ProcessKeyboard(FORWARD, delta_time);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        kamera.ProcessKeyboard(BACKWARD, delta_time);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        kamera.ProcessKeyboard(LEFT, delta_time);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        kamera.ProcessKeyboard(RIGHT, delta_time);
}
unsigned int loadTexture(char const * path, bool gammaCorrection)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum internalFormat;
        GLenum dataFormat;
        if (nrComponents == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (nrComponents == 3)
        {
            internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}