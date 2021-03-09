 #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aOff;

    void main() {
        gl_Position = vec4(aPos.x + aOff.x, aPos.y + aOff.y, aPos.z 1.0);
    }