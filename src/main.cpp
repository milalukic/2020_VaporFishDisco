#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>
#include <learnopengl/filesystem.h>
#include <stb_image.h>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

void fb_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow *window, double x_pozicija, double y_pozicija);
void scroll_callback(GLFWwindow *window, double x_offset, double y_offset);
void update(GLFWwindow* window);

const char *objekat_vertex_shader_source = R"s(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec4 aColor;
    layout (location = 2) in vec2 aTexCoord;

    out vec4 our_color;
    out vec2 tex_coord;

    uniform mat4 model_pravougaonik;
    uniform mat4 view_pravougaonik;
    uniform mat4 projection_pravougaonik;

    void main() {
        gl_Position = projection_pravougaonik * view_pravougaonik * model_pravougaonik * vec4(aPos, 1.0);
        our_color = aColor;
        tex_coord = aTexCoord;
    }
)s";

const char *objekat_fragment_shader_source = R"s(
    #version 330 core
    out vec4 FragColor;

    in vec4 our_color;
    in vec2 tex_coord;

    uniform sampler2D texture1;

    void main(){
        FragColor = texture(texture1, tex_coord) ;
        //FragColor = our_color;
    }
)s";

const char *kocka_vertex_shader_source = R"s(
    #version 330 core
    layout (location = 0) in vec3 aPosK;
    layout (location = 1) in vec2 aTexCoordsK;
    layout (location = 2) in vec4 aColorK;

    out vec2 tex_coordK;
    out vec4 our_colorK;

    uniform mat4 model_kocka;
    uniform mat4 view_kocka;
    uniform mat4 projection_kocka;


    void main()
    {
        gl_Position = projection_kocka * view_kocka * model_kocka * vec4(aPosK, 1.0);
        our_colorK = aColorK;
        tex_coordK = vec2(aTexCoordsK.x, aTexCoordsK.y);
    }
)s";

const char *kocka_fragment_shader_source = R"s(
    #version 330 core
    out vec4 FragColorK;

    in vec4 our_colorK;
    in vec2 tex_coordK;

    uniform sampler2D texture_kocka;

    void main(){
        FragColorK = texture(texture_kocka, tex_coordK);
    }


)s";


int main() {

    glfwInit();
    //verzija ce biti 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    //necemo stare verzije
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window;
    window = glfwCreateWindow(1024, 800, "Andjela Niketic, Mila Lukic", nullptr, nullptr);
    if (window == nullptr){
        std::cout << "GPU nije kompatibilna sa 3.3 verzijom\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //da bismo mogli da crtamo u njemu
    glfwMakeContextCurrent(window);

    //namestamo podatke u odnosu na prozor, kad god se velicina promeni
    glfwSetFramebufferSizeCallback(window, fb_size_callback);

    //obradjujemo potencijalno kliktanje misa, tastature ili skrolovanja
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //pozivamo glad lib da nam ucita sve openGL fje
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Ne mozemo inicijalizovati glad\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glEnable(GL_DEPTH_TEST);


    // OBJEKAT EBO //////////////////////

    //VERTEX SHADER
    unsigned vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &objekat_vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);

    int success = 0;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success)
        std::cout<<"greska u kompilaciji\n";


    //FRAGMENT SHADER
    unsigned fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &objekat_fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success)
        std::cout<<"greska u kompilaciji\n";


    //SHADER PROGRAM - linkovanje VS i FS
    unsigned shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success)
        std::cout<<"greska u linkovanju\n";


    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);


    float vertices[] = {
            //pozicije              //boja                          //tekstura koordinate
            -0.7f, 0.7f, -0.5f,       0.7f, 1.0f, 1.0f, 1.0f,        1.0f, 1.0f,  //gore levo
            -0.7f, -0.7f, -0.5f,      1.0f, 0.7f, 1.0f, 1.0f,        1.0f, 0.0f, //dole levo
            0.7f, -0.7f, -0.5f,       1.0f, 1.0f, 0.7f, 1.0f,        0.0f, 0.0f,//gore desno
            0.7f, 0.7f, -0.5f,        1.0f, 1.0f, 1.0f, 1.0f,        0.0f, 1.0f//dole desno
    };


    unsigned int indices[] = {
            1, 2, 3, //prvi trougao
            0, 1, 3 //drugi trougao
    };

    unsigned int VBO; //vertex buffer object, iz RAM-a ucitava podatke na GPU
    unsigned int VAO; //vertex arr obj, govori openglu sta znace objekti u baferu
    unsigned int EBO; //sadrzi indekse VBOa za svako teme

    glGenVertexArrays(1, &VAO); // generise bafere
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); //aktivira bafer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)(3*sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)(7*sizeof (float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //KOCKA

    //VERTEX SHADER
    unsigned vertex_kocka = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_kocka, 1, &kocka_vertex_shader_source, nullptr);
    glCompileShader(vertex_kocka);

    int successK = 0;
    glGetShaderiv(vertex_kocka, GL_COMPILE_STATUS, &successK);
    if(!successK)
        std::cout<<"greska u kompilaciji\n";


    //FRAGMENT SHADER
    unsigned fragment_kocka = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_kocka, 1, &kocka_fragment_shader_source, nullptr);
    glCompileShader(fragment_kocka);

    glGetShaderiv(fragment_kocka, GL_COMPILE_STATUS, &successK);
    if(!successK)
        std::cout<<"greska u kompilaciji\n";


    //SHADER PROGRAM - linkovanje VS i FS
    unsigned shader_program_kocka = glCreateProgram();
    glAttachShader(shader_program_kocka, vertex_kocka);
    glAttachShader(shader_program_kocka, fragment_kocka);
    glLinkProgram(shader_program_kocka);

    glGetProgramiv(shader_program_kocka, GL_LINK_STATUS, &successK);
    if(!successK)
        std::cout<<"greska u linkovanju\n";


    glDeleteShader(vertex_kocka);
    glDeleteShader(fragment_kocka);

    float vertices_kocka[] = {
            //koordinate tacaka     //koordinate teksture
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


    //TEKSTURA - pravougaonik i kocka

    unsigned int texture1, texture_kocka;
    int teksture[2];
    teksture[0] = texture1;
    teksture[1] = texture_kocka;
    glGenTextures(2, (GLuint*)(teksture));

    glBindTexture(GL_TEXTURE_2D, teksture[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
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

    glUseProgram(shader_program);
    glUseProgram(shader_program_kocka);




    //petlja renderovanja
    while(!glfwWindowShouldClose(window)) {

        update(window);

        //postavimo boju pozadine i ocistimo bafere da bi ta boja mogla da se vidi
        glClearColor(0.6f, 1.0f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GLint tekstura_p_lokacija = glGetUniformLocation(shader_program, "texture1");
        GLint tekstura_k_lokacija = glGetUniformLocation(shader_program_kocka, "texture_kocka");

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, teksture[0]);
        glUniform1i(tekstura_p_lokacija, 0);

        //crtamo objekat
        glUseProgram(shader_program);

        //PROJEKCIJA
//
        mat4 model_pravougaonik = mat4(1.0f);
        model_pravougaonik = rotate(model_pravougaonik,  radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));

        mat4 view_pravougaonik = mat4(1.0f);
        view_pravougaonik = translate(view_pravougaonik, vec3(0.0f, 0.0f, -3.0f));

        mat4 projection_pravougaonik = mat4(1.0f);
        projection_pravougaonik = perspective(radians(45.0f), 1024.0f / 800.0f, 0.1f, 100.0f);

        int model_lokacija_P = glGetUniformLocation(shader_program, "model_pravougaonik");
        glUniformMatrix4fv(model_lokacija_P, 1, GL_FALSE, value_ptr(model_pravougaonik));

        int view_lokacija_P = glGetUniformLocation(shader_program, "view_pravougaonik");
        glUniformMatrix4fv(view_lokacija_P, 1, GL_FALSE, value_ptr(view_pravougaonik));

        int projection_lokacija_P = glGetUniformLocation(shader_program, "projection_pravougaonik");
        glUniformMatrix4fv(projection_lokacija_P, 1, GL_FALSE, value_ptr(projection_pravougaonik));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(shader_program_kocka);

        // ZA KOCKU
        mat4 model_kocka = mat4(1.0f);
        model_kocka = rotate(model_kocka, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));

        mat4 view_kocka = mat4(1.0f);
        view_kocka = translate(view_kocka, vec3(0.0f, 0.0f, -3.0f));

        mat4 projection_kocka = mat4(1.0f);
        projection_kocka = perspective(radians(45.0f), 1024.0f / 800.0f, 0.1f, 100.0f);

        int model_lokacija_K = glGetUniformLocation(shader_program_kocka, "model_kocka");
        glUniformMatrix4fv(model_lokacija_K, 1, GL_FALSE, value_ptr(model_kocka));

        int view_lokacija_K = glGetUniformLocation(shader_program_kocka, "view_kocka");
        glUniformMatrix4fv(view_lokacija_K, 1, GL_FALSE, value_ptr(view_kocka));

        int projection_lokacija_K = glGetUniformLocation(shader_program_kocka, "projection_kocka");
        glUniformMatrix4fv(projection_lokacija_K, 1, GL_FALSE, value_ptr(projection_kocka));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, teksture[1]);
        glUniform1i(tekstura_k_lokacija, 0);

        glBindVertexArray(VAO_kocka);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //imamo bafer u koji se pisu boje piksela i bafer koji se prikazuje rn,
        //ovde ih menjamo
        glfwSwapBuffers(window);

        //da li se nesto desilo?
        glfwPollEvents();

    }

    glDeleteVertexArrays(1, &VAO_kocka);
    glDeleteBuffers(1, &VBO_kocka);
    glDeleteProgram(shader_program_kocka);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader_program);

    glfwTerminate();
    return 0;
}

void fb_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0,0,width,height);
}

//TODO: pomeranje u dubinu,
void mouse_callback(GLFWwindow *window, double x_pozicija, double y_pozicija){

}

//TODO: nmp ni sta ovo radi protumacicu ga valjda lmao
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods){

}

//TODO: pomeranje u dubinu
void scroll_callback(GLFWwindow *window, double x_offset, double y_offset){


}


//TODO: kretanje koriscenjem WASD ili LRUD, pomeranje kamere
void update(GLFWwindow* window) {

    //ako kliknemo escape izlazimo iz prozora
    if(glfwGetKey(window, GLFW_KEY_ESCAPE)==GLFW_PRESS)
        glfwSetWindowShouldClose(window,true);


}
