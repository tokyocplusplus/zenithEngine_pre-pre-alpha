#include <stdlib.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>

float vertices[] = {
    -0.5, -0.5, -0.5,  1.0, 0.0, 0.0,
     0.5, -0.5, -0.5,  0.0, 1.0, 0.0,
     0.5,  0.5, -0.5,  0.0, 0.0, 1.0,
    -0.5,  0.5, -0.5,  1.0, 1.0, 0.0,
    -0.5, -0.5,  0.5,  1.0, 0.0, 1.0,
     0.5, -0.5,  0.5,  0.0, 1.0, 1.0,
     0.5,  0.5,  0.5,  1.0, 1.0, 1.0,
    -0.5,  0.5,  0.5,  0.0, 0.0, 0.0
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


const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec3 aNormal;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec3 ourColor;\n"
"void main() {\n"
"   gl_Position = projection * view * model * vec4(aPos, 1);\n"
"   ourColor = aColor;\n"
"}\n";

const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 ourColor;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"   FragColor = vec4(ourColor, 1);\n"
"}\n";

// Camera stuff
float camX = 0.0f, camY = 0.0f, camZ = 3.0f;
float camFront[3] = {0.0f, 0.0f, -1.0f};
float camUp[3] = {0.0f, 1.0f, 0.0f};
float yaw = -90.0f, pitch = 0.0f;
float lastX = 640.0f, lastY = 360.0f;
int firstMouse = 1;
float speed = 2.5f;
float sensitivity = 0.1f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camX += camFront[0] * cameraSpeed;
        camY += camFront[1] * cameraSpeed;
        camZ += camFront[2] * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camX -= camFront[0] * cameraSpeed;
        camY -= camFront[1] * cameraSpeed;
        camZ -= camFront[2] * cameraSpeed;
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

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO); glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");

    float lastFrame = 0.0f;

    glfwSwapInterval(0);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        input(window, deltaTime);

        float t = glfwGetTime();
        float s = sin(t), c = cos(t);

        float model[] = {
            c, 0, -s, 0,
            0, 1, 0, 0,
            s, 0, c, 0,
            0, 0, 0, 1
        };

        float view[16];
        lookAt(view, camX, camY, camZ,
                    camX + camFront[0], camY + camFront[1], camZ + camFront[2],
                    camUp[0], camUp[1], camUp[2]);

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

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
