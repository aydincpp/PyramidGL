#define GLAD_IMPLEMENTATION
#include "glad/glad.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <stdio.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
bool shader_compile_succeed(GLuint shader);

int main() {
    if (!glfwInit()) {
        printf("GLFW initialization failed\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "pyramid", NULL, NULL);
    if (!window) {
        printf("GLFWwindow* initialization failed\n");
        glfwTerminate();
        return -1;
    }

    glfwMaximizeWindow(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("gladLoadGLLoader failed\n");
        glfwTerminate();
        return -1;
    }

    /* data */
    GLfloat vertices[] = {
        // positions           // colors
        0.0f,  0.8f, 0.0f,    1,0,0,0.5,  // apex
       -0.5f, -0.5f, -0.5f,   0,1,0,0.5,  // base bottom-left
        0.5f, -0.5f, -0.5f,   0,0,1,0.5,  // base bottom-right
        0.5f, -0.5f,  0.5f,   1,1,0,0.5,  // base top-right
       -0.5f, -0.5f,  0.5f,   0,1,1,0.5   // base top-left
    };

    GLsizei stride = sizeof(GLfloat) * 7;
    void* vertex_attr_position = (void*)0;
    void* vertex_attr_color = (void*)(sizeof(GLfloat) * 3);

    GLuint indices[] = {
        0, 1, 2,  // front
        0, 2, 3,  // right
        0, 3, 4,  // back
        0, 4, 1,  // left

        // base
        1, 2, 3,
        1, 3, 4
    };

    const GLchar* vertex_shader_source = R"(
        #version 410

        layout (location = 0) in vec3 aPos; 
        layout (location = 1) in vec4 aColor;

        uniform float uTime;
        uniform float uScale;
        uniform float uAspect;

        out vec4 vertexColor;

        void main()
        {
            float angle = uTime * 0.8;

            // 3D rotation
            mat4 rotationX = mat4(
                1.0, 0.0,         0.0,        0.0,
                0.0, cos(angle), -sin(angle), 0.0,
                0.0, sin(angle),  cos(angle), 0.0,
                0.0, 0.0,         0.0,        1.0
            );

            mat4 rotationY = mat4(
                cos(angle), 0.0, sin(angle), 0.0,
                0.0,        1.0, 0.0,        0.0,
               -sin(angle), 0.0, cos(angle), 0.0,
                0.0,        0.0, 0.0,        1.0
            );

            mat4 rotationZ = mat4(
                cos(angle), -sin(angle), 0.0, 0.0,
                sin(angle),  cos(angle), 0.0, 0.0,
                0.0,         0.0,        1.0, 0.0,
                0.0,         0.0,        0.0, 1.0
            );

            // Perspective projection
            float fov = radians(60.0);
            float nearZ = 0.1;
            float farZ  = 10.0;
            float f = 1.0 / tan(fov / 2.0);
            mat4 projection = mat4(
                f / uAspect, 0.0, 0.0, 0.0,
                0.0, f, 0.0, 0.0,
                0.0, 0.0, (farZ + nearZ) / (nearZ - farZ), -1.0,
                0.0, 0.0, (2.0 * farZ * nearZ) / (nearZ - farZ), 0.0
            );

            // Translate the shape backward
            mat4 translate = mat4(
                1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0,-2.0, 1.0
            );

            vec4 pos = vec4(aPos * uScale, 1.0);
            gl_Position = projection * translate * rotationY * pos;

            vertexColor = aColor;
        }
    )";

    const GLchar* fragment_shader_source = R"(
        #version 410

        in vec4 vertexColor;
        out vec4 fragColor;

        uniform float uTime;
        uniform float uAlphaOverride;

        void main()
        {
            // Pulsating glow
            float pulse = 0.5 + 0.5 * sin(uTime * 3.0);

            // Smooth rainbow cycle using sine waves
            vec3 rainbow = 0.5 + 0.5 * cos(uTime + vertexColor.rgb * 6.2831);

            // Combine vertex color, rainbow effect, and pulse
            vec3 color = mix(vertexColor.rgb, rainbow, 0.6) * (0.7 + 0.3 * pulse);

            // Add a subtle flicker/glow effect
            float glow = 0.5 + 0.5 * sin(uTime * 10.0 + length(vertexColor.rgb));
            color += glow * 0.1;

            float alpha = mix(vertexColor.a, 1.0, uAlphaOverride);
            fragColor = vec4(color, alpha);
        }
    )";


    /* buffer/state configs */
    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, vertex_attr_position);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, vertex_attr_color);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* shader setup */
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);

    glCompileShader(vertex_shader);
    if (!shader_compile_succeed(vertex_shader)){
        glfwTerminate();
        return -1;
    }

    glCompileShader(fragment_shader);
    if (!shader_compile_succeed(fragment_shader)) {
        glfwTerminate();
        return -1;
    }

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    GLint program_link_status;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &program_link_status);
    if (!program_link_status) {
        GLchar log[512];
        glGetProgramInfoLog(shader_program, 512, NULL, log);
        printf("Program link error: %s\n", log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    /* main loop */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.05, 0.05, 0.05, 1.0);
    glViewport(0, 0, mode->width, mode->height);

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program);

        float time = (float)glfwGetTime();
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        float aspect = (float)width / height;
        glUniform1f(glGetUniformLocation(shader_program, "uAspect"), aspect);

        GLuint time_loc = glGetUniformLocation(shader_program, "uTime");
        GLuint scale_loc = glGetUniformLocation(shader_program, "uScale");
        GLuint alpha_loc = glGetUniformLocation(shader_program, "uAlphaOverride");

        glBindVertexArray(vao);

        // wireframe for outer pyramid
        glUniform1f(time_loc, time);
        glUniform1f(scale_loc, 1.05f);
        glUniform1f(alpha_loc, 1.0f);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(1.0);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // wireframe for inner pyramid
        glUniform1f(time_loc, -time);
        glUniform1f(scale_loc, 0.55f);
        glUniform1f(alpha_loc, 1.0f);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(1.0);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // outer pyramid
        glUniform1f(time_loc, -time);
        glUniform1f(scale_loc, 1.0f);
        glUniform1f(alpha_loc, 0.0f);

        glDepthMask(GL_FALSE);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

        // inner pyramid
        glUniform1f(time_loc,  time);
        glUniform1f(scale_loc, 0.5f);
        glUniform1f(alpha_loc, 1.0f);

        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE);

        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

bool shader_compile_succeed(GLuint shader) {
    GLint success;
    GLchar info_log[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        printf("Shader compile error: %s\n", info_log);

        return false;
    }

    return true;
}

