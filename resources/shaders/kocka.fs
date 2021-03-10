#version 330 core
    out vec4 FragColorK;
    //uniform vec4 our_color;
    in vec4 our_colorK;
    in vec2 tex_coordK;

    uniform sampler2D textureK;

    void main(){
        FragColorK = mix(texture(textureK, tex_coordK),our_colorK, 0.5) ;
    }

