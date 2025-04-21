#ifndef DATA_H
#define DATA_H

//--------------------CUBE DATA--------------------//
float vertices[] = {
    //        position            color              texcoord
    -0.5, -0.5, -0.5,   1.0, 0.0, 0.0,   0.0f, 0.0f, // 0
     0.5, -0.5, -0.5,   0.0, 1.0, 0.0,   1.0f, 0.0f, // 1
     0.5,  0.5, -0.5,   0.0, 0.0, 1.0,   1.0f, 1.0f, // 2
    -0.5,  0.5, -0.5,   1.0, 1.0, 0.0,   0.0f, 1.0f, // 3
    -0.5, -0.5,  0.5,   1.0, 0.0, 1.0,   0.0f, 0.0f, // 4
     0.5, -0.5,  0.5,   0.0, 1.0, 1.0,   1.0f, 0.0f, // 5
     0.5,  0.5,  0.5,   1.0, 1.0, 1.0,   1.0f, 1.0f, // 6
    -0.5,  0.5,  0.5,   0.0, 0.0, 0.0,   0.0f, 1.0f  // 7
};

unsigned int indices[] = {
    // back face (CCW when looking at it)
    0, 1, 2, 2, 3, 0,
    // front face
    4, 6, 5, 6, 4, 7,
    // left face
    4, 0, 3, 3, 7, 4,
    // right face
    1, 5, 6, 6, 2, 1,
    // top face
    3, 2, 6, 6, 7, 3,
    // bottom face
    4, 5, 1, 1, 0, 4
};
//--------------------CUBE DATA--------------------//

unsigned int fbo, fboTexture, fboRBO, quadVAO, quadVBO;

//--------------------SHADERS--------------------//
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec2 aTexCoord;\n" // Added texcoord
"layout (location = 3) in vec3 offset;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec3 ourColor;\n"
"out vec2 TexCoord;\n" // Passing texcoord to fragment shader
"void main() {\n"
"   gl_Position = projection * view * model * vec4(aPos + offset, 1.0);\n"
"   ourColor = aColor;\n"
"   TexCoord = aTexCoord;\n" // Assign texcoord to output
"}\n";
const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 ourColor;\n"
"in vec2 TexCoord;\n"
"out vec4 FragColor;\n"

"float far = 50.0;\n"
"float near = 0.1;\n"
"vec3 fogColor = vec3(0.5, 0.6, 0.7);\n" // Fog color

"float linearizeDepth(float depth) {\n"
"   float z = depth * 2.0 - 1.0;\n"
"   return (2.0 * near * far) / (far + near - z * (far - near));\n"
"}\n"

"void main() {\n"
"   float depth = linearizeDepth(gl_FragCoord.z);\n"
"   float fogFactor = (far - depth) / (far - near);\n"
"   fogFactor = clamp(fogFactor, 0.0, 1.0);\n"

"   vec3 finalColor = mix(fogColor, ourColor, fogFactor);\n"
"   FragColor = vec4(finalColor, 1.0);\n"
"}\n";
//--------------------SHADERS--------------------//


//--------------------CAMERA SETTINGS--------------------//
float camX = 0.0f, camY = 0.0f, camZ = 3.0f;
float camFront[3] = {0.0f, 0.0f, -1.0f};
float camUp[3] = {0.0f, 1.0f, 0.0f};
float yaw = -90.0f, pitch = 0.0f;
float lastX = 640.0f, lastY = 360.0f;
int firstMouse = 1;
float speed = 2.5f;
float sprintSpeed = 5.0f;
float sensitivity = 0.1f;
//--------------------CAMERA SETTINGS--------------------//

//--------------------CHUNK SETTINGS--------------------//
float scale = 1.0f; // scale of cubes
float model[16] = { // model matrix
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};
//--------------------CHUNK SETTINGS--------------------//

#endif