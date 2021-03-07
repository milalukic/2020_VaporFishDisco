#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace glm;

void fb_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow *window, double x_pozicija, double y_pozicija);
void scroll_callback(GLFWwindow *window, double x_offset, double yoffset);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

void update(GLFWwindow* window);

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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    //pozivamo glad lib da nam ucita sve openGL fje
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Ne mozemo inicijalizovati glad\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //petlja renderovanja
    while(!glfwWindowShouldClose(window)) {
        //da li se nesto desilo?
        glfwPollEvents();
        update(window);
        //imamo bafer u koji se pisu boje piksela i bafer koji se prikazuje rn,
        //ovde ih menjamo
        glfwSwapBuffers(window);


    }

    return 0;
}

void fb_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0,0,width,height);
}

void mouse_callback(GLFWwindow *window, double x_pozicija, double y_pozicija){

}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods){

}

void update(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE)==GLFW_PRESS){
        glfwSetWindowShouldClose(window,true);
    }
}

