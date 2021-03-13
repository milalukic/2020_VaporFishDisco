    #version 330 core

    out vec4 FragColorK;

    in vec4 our_colorK;
    in vec2 tex_coordK;

    uniform sampler2D texture_kocka;
    uniform vec3 lightColor;


void main(){
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    ambient = vec4(ambient, 1.0);
    FragColorK = texture(texture_kocka, tex_coordK);
    vec3 result = ambient * FragColorK;
    FragColor = result
}

