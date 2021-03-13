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