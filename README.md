# Vaporfishdisco 

Computer Graphics project, implemented in OpenGL/C++ using GLFW library<br>

Contents:<br>

- **Rectangle** with texture *checkers.jpg*<br>
- **Cube** with texture *vaporwave.jpg*, and **specular** component *vaporwave_specular.jpg* <br>
- **Model**:  the blue Fish<br>
- **Lighting**: directional and 3 point lights, implemented using Blinn-Phong lighting model, combining diffuse, specular, and ambiental components<br>
- **Shaders**: cube, rectangle, model, point lights <br>
- **Gamma correction** enabled for the cube, rectangle, and fish model; used to correct the colors to get more realistic and good-looking results<br>
- **Anti-aliasing** effect implemented on the cube, used to reduce the aliasing effect (jagged edges)<br>
- **Stencil testing**: used to outline the light sources<br>
- **Key** binds:<br>
  - **ESC** to exit the project window<br>
  - **WASD** for movement<br>
- **Mouse** controls:<br>
  - camera **movement**<br>
  - **scroll** to zoom in/out<br>
