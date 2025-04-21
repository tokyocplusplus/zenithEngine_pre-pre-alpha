#include <stdlib.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include "header/funcs.h"
#include "header/data.h"
#define STB_IMAGE_IMPLEMENTATION
#include "header/stb_image.h"

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glDepthMask(GL_FALSE);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    //int width, height, nrChannels;
    //unsigned char* data = stbi_load("omnicheeks.png", &width, &height, &nrChannels, 0);
    //if (!data) {
        //printf("Failed to load texture: %s\n", stbi_failure_reason());
        //return -1;
    //}
    int w, h, channels;
    unsigned char* heightData = stbi_load("heightmap.png", &w, &h, &channels, 1);
    if (!heightData) {
        printf("failed to load heightmap.png");
    }
    
    //unsigned int texture;
    //glGenTextures(1, &texture);
    //glBindTexture(GL_TEXTURE_2D, texture);
    
    // Set texture parameters
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Upload texture to GPU
    //GLenum format = nrChannels == 3 ? GL_RGB : GL_RGBA;
    //glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    //glGenerateMipmap(GL_TEXTURE_2D);
    
    //stbi_image_free(heightData); // Clean up
    
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO); glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");

    float lastFrame = 0.0f;

    glfwSwapInterval(0); // disabling vsync

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        input(window, deltaTime);

        //float t = glfwGetTime();
        //float s = sin(t), c = cos(t);

        /*ROTATION MATRICES

        X ROTATION MATRIX
        1, 0, 0, 0,
        0, c,-s, 0,
        0, s, c, 0,
        0, 0, 0, 1

        Y ROTATION MATRIX
        c, 0, s, 0,
        0, 1, 0, 0,
       -s, 0, c, 0,
        0, 0, 0, 1
        
        Z ROTATION MATRIX
        c,-s, 0, 0,
        s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1

        ROTATION MATRICES*/

        float view[16];
        lookAt(view, camX, camY, camZ, camX + camFront[0], camY + camFront[1], camZ + camFront[2], camUp[0], camUp[1], camUp[2]);

        float fov = 90.0f * 3.14159f / 180.0f;
        float aspect = 1280.0f / 720.0f;
        float near = 0.1f, far = 100.0f;
        float f = 1.0f / tan(fov / 2.0f);
        float projection[] = {
            f / aspect, 0, 0, 0,
            0, f, 0, 0,
            0, 0, (far + near) / (near - far), -1,
            0, 0, (2 * far * near) / (near - far), 0
        };
        
        glClearColor(0,0,0, 1.0f); // if you have an OLED or QLED monitor or TV, enjoy.. my laptop doesnt have oled :O
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        //glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0); // texture unit 0
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);
        //glBindVertexArray(VAO);

        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, texture);
        //drawChunkHeightMap(shaderProgram, modelLoc, w, h, 10, heightData);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        drawChunkHeightMapInstanced(shaderProgram,modelLoc,VAO,w,h,15,heightData, 32.0f);
        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, 9);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //END CODE
    glfwTerminate();
    return 0;
}
