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

         tex_coordK = aTexCoordsK;
     }