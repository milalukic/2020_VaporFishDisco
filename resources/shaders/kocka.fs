    #version 330 core
    out vec4 FragColorK;

    in vec4 our_colorK;
    in vec2 tex_coordK;

    uniform sampler2D texture_kocka;

    void main(){
        FragColorK = texture(texture_kocka, tex_coordK);
    }


