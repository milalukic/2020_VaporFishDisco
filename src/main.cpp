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

using namespace glm;

//velicina prozora
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 800;

//kamera deklaracija + mis
Camera kamera(vec3(0.0f, 0.0f, 3.0f));

bool first_mouse = true;
float last_X = SCR_WIDTH / 2.0f;
float last_Y = SCR_HEIGHT / 2.0f;

// timing
float delta_time = 0.0f;	// time between current frame and last frame
float last_frame = 0.0f;

//svetlost
//vec3 lightPos(1.2f, 1.0f, 2.0f);

void fb_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow *window, double x_pozicija, double y_pozicija);
void scroll_callback(GLFWwindow *window, double x_offset, double y_offset);
void update(GLFWwindow* window);

int main() {

    glfwInit();
    //verzija ce biti 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //necemo stare verzije

    GLFWwindow *window;
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Andjela Niketic, Mila Lukic", nullptr, nullptr);
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

    float vertices_kocka[] = {
            //koordinate tacaka     //koordinate teksture   //boja
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,             0.9f, 0.2f, 0.7f, 1.0f,
            0.5f, -0.5f, -0.5f,     1.0f, 0.0f,             0.3f, 0.7f, 0.8f, 1.0f,
            0.5f,  0.5f, -0.5f,     1.0f, 1.0f,             0.6f, 0.3f, 0.5f, 1.0f,
            0.5f,  0.5f, -0.5f,     1.0f, 1.0f,             1.0f, 0.5f, 0.6f, 1.0f,
            -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,             0.9f, 0.2f, 0.7f, 1.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,             0.6f, 0.3f, 0.5f, 1.0f,

            -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,             0.9f, 0.2f, 0.7f, 1.0f,
            0.5f, -0.5f,  0.5f,     1.0f, 0.0f,             0.3f, 0.7f, 0.8f, 1.0f,
            0.5f,  0.5f,  0.5f,     1.0f, 1.0f,             0.6f, 0.3f, 0.5f, 1.0f,
            0.5f,  0.5f,  0.5f,     1.0f, 1.0f,             1.0f, 0.5f, 0.6f, 1.0f,
            -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,             0.9f, 0.2f, 0.7f, 1.0f,
            -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,             0.6f, 0.3f, 0.5f, 1.0f,

            -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,             1.0f, 0.5f, 0.6f, 1.0f,
            -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,             0.9f, 0.2f, 0.7f, 1.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,             0.3f, 0.7f, 0.8f, 1.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,             0.6f, 0.3f, 0.5f, 1.0f,
            -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,             1.0f, 0.5f, 0.6f, 1.0f,
            -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,             0.9f, 0.2f, 0.7f, 1.0f,

            0.5f,  0.5f,  0.5f,     1.0f, 0.0f,             0.9f, 0.2f, 0.7f, 1.0f,
            0.5f,  0.5f, -0.5f,     1.0f, 1.0f,             0.6f, 0.3f, 0.5f, 1.0f,
            0.5f, -0.5f, -0.5f,     0.0f, 1.0f,             1.0f, 0.5f, 0.6f, 1.0f,
            0.5f, -0.5f, -0.5f,     0.0f, 1.0f,             0.9f, 0.2f, 0.7f, 1.0f,
            0.5f, -0.5f,  0.5f,     0.0f, 0.0f,             0.3f, 0.7f, 0.8f, 1.0f,
            0.5f,  0.5f,  0.5f,     1.0f, 0.0f,             0.6f, 0.3f, 0.5f, 1.0f,

            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,             1.0f, 0.5f, 0.6f, 1.0f,
            0.5f, -0.5f, -0.5f,     1.0f, 1.0f,             0.9f, 0.2f, 0.7f, 1.0f,
            0.5f, -0.5f,  0.5f,     1.0f, 0.0f,             0.9f, 0.2f, 0.7f, 1.0f,
            0.5f, -0.5f,  0.5f,     1.0f, 0.0f,             0.6f, 0.3f, 0.5f, 1.0f,
            -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,             1.0f, 0.5f, 0.6f, 1.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,             0.9f, 0.2f, 0.7f, 1.0f,

            -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,             0.3f, 0.7f, 0.8f, 1.0f,
            0.5f,  0.5f, -0.5f,     1.0f, 1.0f,             0.6f, 0.3f, 0.5f, 1.0f,
            0.5f,  0.5f,  0.5f,     1.0f, 0.0f,             1.0f, 0.5f, 0.6f, 1.0f,
            0.5f,  0.5f,  0.5f,     1.0f, 0.0f,             0.9f, 0.2f, 0.7f, 1.0f,
            -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,             0.9f, 0.2f, 0.7f, 1.0f,
            -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,             0.6f, 0.3f, 0.5f, 1.0f
    };

    unsigned VBO_kocka, VAO_kocka;
    glGenVertexArrays(1, &VAO_kocka);
    glGenBuffers(1, &VBO_kocka);

    glBindVertexArray(VAO_kocka);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_kocka);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_kocka), vertices_kocka, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)(5 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);


//    //LIGHT SOURCE
//    Shader light_source_shader("resources/shaders/izvor_svetlosti.vs", "resources/shaders/izvor_svetlosti.fs");
//
//    float vertices_light[] = {
//            -0.5f, -0.5f, -0.5f,
//            0.5f, -0.5f, -0.5f,
//            0.5f,  0.5f, -0.5f,
//            0.5f,  0.5f, -0.5f,
//            -0.5f,  0.5f, -0.5f,
//            -0.5f, -0.5f, -0.5f,
//
//            -0.5f, -0.5f,  0.5f,
//            0.5f, -0.5f,  0.5f,
//            0.5f,  0.5f,  0.5f,
//            0.5f,  0.5f,  0.5f,
//            -0.5f,  0.5f,  0.5f,
//            -0.5f, -0.5f,  0.5f,
//
//            -0.5f,  0.5f,  0.5f,
//            -0.5f,  0.5f, -0.5f,
//            -0.5f, -0.5f, -0.5f,
//            -0.5f, -0.5f, -0.5f,
//            -0.5f, -0.5f,  0.5f,
//            -0.5f,  0.5f,  0.5f,
//
//            0.5f,  0.5f,  0.5f,
//            0.5f,  0.5f, -0.5f,
//            0.5f, -0.5f, -0.5f,
//            0.5f, -0.5f, -0.5f,
//            0.5f, -0.5f,  0.5f,
//            0.5f,  0.5f,  0.5f,
//
//            -0.5f, -0.5f, -0.5f,
//            0.5f, -0.5f, -0.5f,
//            0.5f, -0.5f,  0.5f,
//            0.5f, -0.5f,  0.5f,
//            -0.5f, -0.5f,  0.5f,
//            -0.5f, -0.5f, -0.5f,
//
//            -0.5f,  0.5f, -0.5f,
//            0.5f,  0.5f, -0.5f,
//            0.5f,  0.5f,  0.5f,
//            0.5f,  0.5f,  0.5f,
//            -0.5f,  0.5f,  0.5f,
//            -0.5f,  0.5f, -0.5f,
//    };
//
//    unsigned VBO_light, VAO_light;
//    glGenVertexArrays(1, &VAO_light);
//    glGenBuffers(1, &VBO_light);
//
//    glBindVertexArray(VAO_light);
//
//    glBindBuffer(GL_ARRAY_BUFFER, VBO_light);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_light), vertices_light, GL_STATIC_DRAW);
//
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
//
//    glEnableVertexAttribArray(0);



    //TEKSTURA - pravougaonik i kocka

    int teksture[2];

    glGenTextures(2, (GLuint*)(teksture));

    glBindTexture(GL_TEXTURE_2D, teksture[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(FileSystem::getPath("resources/textures/checkers.jpg").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, teksture[1]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width_kocka, height_kocka, nrChannels_kocka;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data_kocka = stbi_load(FileSystem::getPath("resources/textures/vaporwave.jpg").c_str(), &width_kocka, &height_kocka, &nrChannels_kocka, 0);
    if (data_kocka)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_kocka, height_kocka, 0, GL_RGB, GL_UNSIGNED_BYTE, data_kocka);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data_kocka);

    pravougaonik_shader.use();
    kocka_shader.use();


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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, teksture[0]);

        //PROJEKCIJA - PRAVOUGAONIK

        pravougaonik_shader.use();
        pravougaonik_shader.setInt("texture1", 0);

        mat4 model_pravougaonik = mat4(1.0f);
        model_pravougaonik = rotate(model_pravougaonik,  radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));

        mat4 view_pravougaonik = mat4(1.0f);
        view_pravougaonik = translate(view_pravougaonik, vec3(0.0f, 0.0f, -3.0f));

        mat4 projection_pravougaonik = mat4(1.0f);
        projection_pravougaonik = perspective(radians(kamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        pravougaonik_shader.setMat4("model_pravougaonik", model_pravougaonik);
        pravougaonik_shader.setMat4("view_pravougaonik", pogled);
        pravougaonik_shader.setMat4("projection_pravougaonik", projection_pravougaonik);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // PROJEKCIJA - KOCKA

        kocka_shader.use();

        mat4 model_kocka = mat4(1.0f);
        model_kocka = rotate(model_kocka, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));

        mat4 view_kocka = mat4(1.0f);
        view_kocka = translate(view_kocka, vec3(0.0f, 0.0f, -3.0f));

        mat4 projection_kocka = mat4(1.0f);
        projection_kocka = perspective(radians(kamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);


        kocka_shader.setMat4("model_kocka", model_kocka);
        kocka_shader.setMat4("view_kocka", pogled);
        kocka_shader.setMat4("projection_kocka", projection_kocka);


        kocka_shader.setVec3("lightColor", 1.0, 1.0, 1.0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, teksture[1]);
        kocka_shader.setInt("texture_kocka", 0);

        glBindVertexArray(VAO_kocka);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //svetlost kocka
//        light_source_shader.use();
//
//        mat4 projection_light = perspective(radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//        mat4 view_light = pogled;
//
//        light_source_shader.setMat4("projection", projection_light);
//        light_source_shader.setMat4("view", view_light);
//
//        mat4 model_light = mat4(1.0f);
//        model_light = translate(model_light, lightPos);
//        model_light = scale(model_light, vec3(0.2f));
//
//        light_source_shader.setMat4("model", model_light);
//
//        glBindVertexArray(VAO_light);
//        glDrawArrays(GL_TRIANGLES, 0, 36);


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
    const float camera_speed = 0.05f;
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
