    #version 330 core
    out vec4 FragColor;

    in vec4 our_color;
    in vec2 tex_coord;

    uniform sampler2D texture1;

    void main(){
        FragColor = texture(texture1, tex_coord) ;

    }