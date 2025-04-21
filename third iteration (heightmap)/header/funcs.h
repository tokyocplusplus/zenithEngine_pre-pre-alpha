#ifndef FUNCS_H
#define FUNCS_H
#include "data.h" // no header/data.h because its literally in the headers folder next to it

//--------------------FUNCS--------------------//
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

/* //TOO BASIC OF A FUNC void drawChunk(GLuint shaderProgram, GLuint modelLoc) {
    for (float x = 0; x < 5; ++x) {
        for (float y = 0; y < 2; ++y) {
            for (float z = 0; z < 4; ++z) {
                float model[16] = {
                    1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    x, y, z, 1  // translate the cube
                };
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
        }
    }
}*/

/* // you served us well.. now its done for you
void drawChunkHeightMap(GLuint shaderProgram, GLuint modelLoc, int w, int h, float maxHeight, unsigned char* heightData) {
    if (!heightData) {
        printf("Heightmap data is NULL!\n");
        return;
    }

    float scale = 1.0f; // Adjust if you want more space between blocks

    for (int x = 0; x < w; ++x) {
        for (int z = 0; z < h; ++z) {
            // Get brightness from grayscale heightmap
            unsigned char pixel = heightData[z * w + x];
            int maxY = (int)((pixel / 255.0f) * maxHeight); // Normalize and scale

            for (int y = 0; y <= maxY; ++y) {
                float model[16] = {
                    1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    x * scale, y * scale, z * scale, 1
                };

                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
        }
    }
}*/

void drawChunkHeightMapInstanced(GLuint shaderProgram, GLuint modelLoc, GLuint VAO, int w, int h, float maxHeight, unsigned char* heightData, float viewDistance) {
    int instanceCount = 0;
    for (int x = 0; x < w; ++x) {
        for (int z = 0; z < h; ++z) {
            unsigned char pixel = heightData[z * w + x];
            int maxY = (int)((pixel / 255.0f) * maxHeight);
            instanceCount += (maxY + 1);
        }
    }
    float* offsets = (float*)malloc(instanceCount * 3 * sizeof(float));
    if (!offsets) {
        printf("Failed to allocate memory for instance offsets\n");
        return;
    }
    int i = 0;
    for (int x = 0; x < w; ++x) {
        for (int z = 0; z < h; ++z) {
            unsigned char pixel = heightData[z * w + x];
            int maxY = (int)((pixel / 255.0f) * maxHeight);
            for (int y = 0; y <= maxY; ++y) {
                offsets[i++] = x * scale;
                offsets[i++] = y * scale;
                offsets[i++] = z * scale;
            }
        }
    }
    GLuint instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceCount * 3 * sizeof(float), offsets, GL_DYNAMIC_DRAW);
    free(offsets); // done with CPU-side data
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(3); // location = 3 in vertex shader
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glVertexAttribDivisor(3, 1); // This makes it per-instance
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, instanceCount);
    glDeleteBuffers(1, &instanceVBO);
}

void translate(float matrix[16], float x, float y, float z) {
    matrix[12] += x;
    matrix[13] += y;
    matrix[14] += z;
}

void multiplyMatrix4x4(float result[16], const float a[16], const float b[16]) {
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            result[col * 4 + row] = 
                a[0 * 4 + row] * b[col * 4 + 0] +
                a[1 * 4 + row] * b[col * 4 + 1] +
                a[2 * 4 + row] * b[col * 4 + 2] +
                a[3 * 4 + row] * b[col * 4 + 3];
        }
    }
}

void lookAt(float view[16], float eyeX, float eyeY, float eyeZ,
            float centerX, float centerY, float centerZ,
            float upX, float upY, float upZ) {
    float fX = centerX - eyeX, fY = centerY - eyeY, fZ = centerZ - eyeZ;
    float fLen = sqrt(fX*fX + fY*fY + fZ*fZ);
    fX /= fLen; fY /= fLen; fZ /= fLen;

    float sX = fY * upZ - fZ * upY;
    float sY = fZ * upX - fX * upZ;
    float sZ = fX * upY - fY * upX;
    float sLen = sqrt(sX*sX + sY*sY + sZ*sZ);
    sX /= sLen; sY /= sLen; sZ /= sLen;

    float uX = sY * fZ - sZ * fY;
    float uY = sZ * fX - sX * fZ;
    float uZ = sX * fY - sY * fX;

    view[0] = sX; view[4] = sY; view[8]  = sZ;  view[12] = 0;
    view[1] = uX; view[5] = uY; view[9]  = uZ;  view[13] = 0;
    view[2] = -fX;view[6] = -fY;view[10] = -fZ; view[14] = 0;
    view[3] = 0;  view[7] = 0;  view[11] = 0;   view[15] = 1;

    float trans[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -eyeX, -eyeY, -eyeZ, 1
    };
    multiplyMatrix4x4(view, view, trans);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = 0;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    float radYaw = yaw * 3.14159f / 180.0f;
    float radPitch = pitch * 3.14159f / 180.0f;

    camFront[0] = cos(radYaw) * cos(radPitch);
    camFront[1] = sin(radPitch);
    camFront[2] = sin(radYaw) * cos(radPitch);

    float len = sqrt(camFront[0]*camFront[0] + camFront[1]*camFront[1] + camFront[2]*camFront[2]);
    camFront[0] /= len; camFront[1] /= len; camFront[2] /= len;
}

void input(GLFWwindow* window, float deltaTime) {
    float cameraSpeed = speed * deltaTime;
    float camSprintSpeed = sprintSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)==GLFW_PRESS) {
            camX += camFront[0] * camSprintSpeed;
            camY += camFront[1] * camSprintSpeed;
            camZ += camFront[2] * camSprintSpeed;
        } else {
            camX += camFront[0] * cameraSpeed;
            camY += camFront[1] * cameraSpeed;
            camZ += camFront[2] * cameraSpeed;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)==GLFW_PRESS) {
            camX -= camFront[0] * camSprintSpeed;
            camY -= camFront[1] * camSprintSpeed;
            camZ -= camFront[2] * camSprintSpeed;
        } else {
            camX -= camFront[0] * cameraSpeed;
            camY -= camFront[1] * cameraSpeed;
            camZ -= camFront[2] * cameraSpeed;
        }
    }

    float right[3] = {
        camFront[2]*camUp[1] - camFront[1]*camUp[2],
        camFront[0]*camUp[2] - camFront[2]*camUp[0],
        camFront[1]*camUp[0] - camFront[0]*camUp[1]
    };
    float len = sqrt(right[0]*right[0] + right[1]*right[1] + right[2]*right[2]);
    right[0] /= len; right[1] /= len; right[2] /= len;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camX -= right[0] * cameraSpeed;
        camY -= right[1] * cameraSpeed;
        camZ -= right[2] * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camX += right[0] * cameraSpeed;
        camY += right[1] * cameraSpeed;
        camZ += right[2] * cameraSpeed;
    }
    if (glfwGetKey(window,GLFW_KEY_SPACE) == GLFW_PRESS) {
        camY += camUp[1] * cameraSpeed; // camUp[1] is the second element of the array, that would be the Y position (1.0f)
    }
    if (glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camY -= camUp[1] * cameraSpeed;
    }

    if(glfwGetKey(window, GLFW_KEY_ESCAPE)==GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}
//--------------------FUNCS--------------------//

#endif