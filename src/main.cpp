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

    uniform mat4 model_matrica;
    uniform mat4 view_matrica;
    uniform mat4 projection_matrica;

    void main() {
        gl_Position = projection_matrica * view_matrica * model_matrica * vec4(aPos, 1.0);
        our_color = aColor;
        tex_coord = aTexCoord;
    }
)s";

const char *objekat_fragment_shader_source = R"s(
    #version 330 core
    out vec4 FragColor;
    //uniform vec4 our_color;
    in vec4 our_color;
    in vec2 tex_coord;

    uniform sampler2D texture1;

    void main(){
        FragColor = mix(texture(texture1, tex_coord),our_color, 0.5) ;
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

    ///VERTEX SHADER
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
            -0.5f, 0.5f, 0.0f,       0.9f, 0.2f, 0.7f, 1.0f,        1.0f, 1.0f,  //gore levo
            -0.5f, -0.5f, 0.0f,      0.3f, 0.7f, 0.8f, 1.0f,        1.0f, 0.0f, //dole levo
            0.5f, -0.5f, 0.0f,       0.6f, 0.3f, 0.5f, 1.0f,        0.0f, 0.0f,//gore desno
            0.5f, 0.5f, 0.0f,        1.0f, 0.5f, 0.6f, 1.0f,        0.0f, 1.0f//dole desno
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
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)(7*sizeof (float )));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    ///TEKSTURA

    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
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



    glUseProgram(shader_program);
    glUniform1i(glGetUniformLocation(shader_program, "texture1"), 0);

    //petlja renderovanja
    while(!glfwWindowShouldClose(window)) {

        update(window);

        //postavimo boju pozadine i ocistimo bafere da bi ta boja mogla da se vidi
        glClearColor(0.6f, 1.0f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);


        //crtamo objekat
        glUseProgram(shader_program);

        //PROJEKCIJA

        mat4 model_matrica = mat4(1.0f);
        model_matrica = rotate(model_matrica, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));

        mat4 view_matrica = mat4(1.0f);
        view_matrica = translate(view_matrica, vec3(0.0f, 0.0f, -3.0f));

        mat4 projection_matrica = mat4(1.0f);
        projection_matrica = perspective(radians(45.0f), 1024.0f / 800.0f, 0.1f, 100.0f);

        int model_lokacija = glGetUniformLocation(shader_program, "model_matrica");
        glUniformMatrix4fv(model_lokacija, 1, GL_FALSE, value_ptr(model_matrica));

        int view_lokacija = glGetUniformLocation(shader_program, "view_matrica");
        glUniformMatrix4fv(view_lokacija, 1, GL_FALSE, value_ptr(view_matrica));

        int projection_lokacija = glGetUniformLocation(shader_program, "projection_matrica");
        glUniformMatrix4fv(projection_lokacija, 1, GL_FALSE, value_ptr(projection_matrica));

        // ako hocemo da menja boju uncomment

//        float time_value = glfwGetTime();
//        float green_value = sin(time_value)/2.0 + 0.5f;
//        int vertex_color_location = glGetUniformLocation(shader_program, "our_color");
//        glUniform4f(vertex_color_location, 0.0f, green_value, 0.0f, 1.0f);

        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        //imamo bafer u koji se pisu boje piksela i bafer koji se prikazuje rn,
        //ovde ih menjamo
        glfwSwapBuffers(window);

        //da li se nesto desilo?
        glfwPollEvents();

    }

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
