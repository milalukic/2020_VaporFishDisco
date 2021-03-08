#version 330 core

out vec4 FragColor;

void main(){
    FragColor = vec4(aPos.x, aPos.y, aPos.z, 1.0f);
}