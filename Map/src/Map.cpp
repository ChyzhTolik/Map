#include "Map.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"
#include "WebReader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

int Map::resol = 1;
int Map::x = 0;
int Map::y = 0;

void Map::SetFiles(std::vector<std::string>* Files, int resol, int x, int y)
{
    std::string path = "Tiles/tile";
    std::string tilename;
    Files->clear();
    WebReader reader;

    tilename = path + std::to_string(resol) + std::to_string(x) + std::to_string(y) + ".png";
    reader.MakeFile(resol, x, y);
    Files->push_back(tilename);
    tilename = path + std::to_string(resol) + std::to_string(x + 1) + std::to_string(y) + ".png";
    reader.MakeFile(resol, x + 1, y);
    Files->push_back(tilename);
    tilename = path + std::to_string(resol) + std::to_string(x) + std::to_string(y + 1) + ".png";
    reader.MakeFile(resol, x, y + 1);
    Files->push_back(tilename);
    tilename = path + std::to_string(resol) + std::to_string(x + 1) + std::to_string(y + 1) + ".png";
    reader.MakeFile(resol, x + 1, y + 1);
    Files->push_back(tilename);
}

void Map::Draw(GLFWwindow* window, std::vector<std::string>Files)
{
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_BLEND));

    float positions[] = {
       -400.0f, -000.0f, 0.0f, 0.0f,
        000.0f, -000.0f, 1.0f, 0.0f,
        000.0f,  400.0f, 1.0f, 1.0f,
       -400.0f,  400.0f,  0.0f, 1.0f
    };

    unsigned int indices[] = {
        0,1,2,
        2,3,0
    };
    Renderer renderer;
    VertexArray va;
    VertexBuffer vb(positions, 4 * 4 * sizeof(float));
    VertexBufferLayout layout;
    layout.Push < float >(2);
    layout.Push < float >(2);
    va.AddBuffer(vb, layout);
    IndexBuffer ib(indices, 6);
    renderer.Clear();
    Shader shader("res/shaders/Basic.shader");

    glm::mat4 proj = glm::ortho(-400.0f, 400.0f, -400.0f, 400.0f, -1.0f, 1.0f);
    glm::vec3 translationA(0, 0, 0);
    glm::vec3 translationB(400, -00, 0);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    shader.Bind();
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
        glm::mat4 mvp = proj * view * model;
        shader.SetUniformMat4f("u_MVP", mvp);
        std::string file;
        file = Files[0];
        Texture texture(file);
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);
        renderer.Draw(va, ib, shader);
    }

    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
        glm::mat4 mvp = proj * view * model;
        shader.SetUniformMat4f("u_MVP", mvp);
        std::string file;
        file = Files[1];
        Texture texture(file);
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);
        renderer.Draw(va, ib, shader);
    }

    {
        glm::vec3 translationB(00, -400, 0);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
        glm::mat4 mvp = proj * view * model;
        shader.SetUniformMat4f("u_MVP", mvp);
        std::string file;
        file = Files[2];
        Texture texture(file);
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);
        renderer.Draw(va, ib, shader);
    }

    {
        glm::vec3 translationB(400, -400, 0);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
        glm::mat4 mvp = proj * view * model;
        shader.SetUniformMat4f("u_MVP", mvp);
        std::string file;
        file = Files[3];
        Texture texture(file);
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);
        renderer.Draw(va, ib, shader);
    }

    glfwSwapBuffers(window);
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS)
    {
        std::vector<std::string>Files;
        Map::x *= 2;
        Map::y *= 2;
        if (Map::resol + 1 == 23)
        {
            return;
        }
        else
            ++Map::resol;
        Map::SetFiles(&Files, Map::resol, Map::x, Map::y);
        Map::Draw(window, Files);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }
    if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS)
    {
        Map::x /= 2;
        if (Map::x == pow(2.0, Map::resol - 1) - 1)
        {
            if (Map::x > 0)
            {
                --Map::x;
            }
        }
        Map::y /= 2;
        if (Map::y == pow(2.0, Map::resol - 1) - 1)
        {
            if (Map::y > 0)
            {
                --Map::x;
            }
        }
        if (Map::resol - 1 < 1)
        {
            return;
        }
        else
            --Map::resol;

        std::vector<std::string>Files;
        Map::SetFiles(&Files, Map::resol, Map::x, Map::y);
        Map::Draw(window, Files);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        if (Map::x + 1 <= pow(2.0, Map::resol) - 2)
        {
            Map::x++;
        }
        else
            return;
        std::vector<std::string>Files;
        Map::SetFiles(&Files, Map::resol, Map::x, Map::y);
        Map::Draw(window, Files);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        if (Map::x == 0)
        {
            return;
        }
        else
            Map::x--;

        std::vector<std::string>Files;
        Map::SetFiles(&Files, Map::resol, Map::x, Map::y);
        Map::Draw(window, Files);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        if (Map::y == 0)
        {
            return;
        }
        else
            Map::y--;

        std::vector<std::string>Files;
        Map::SetFiles(&Files, Map::resol, Map::x, Map::y);
        Map::Draw(window, Files);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        if (Map::y + 1 <= pow(2.0, Map::resol) - 2)
        {
            Map::y++;
        }
        else
            return;

        std::vector<std::string>Files;
        Map::SetFiles(&Files, Map::resol, Map::x, Map::y);
        Map::Draw(window, Files);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }
}

int Map::Run(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 800, "Map", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    {
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glEnable(GL_BLEND));
        std::vector<std::string>Files;
        SetFiles(&Files, 1, 0, 0);
        Draw(window, Files);
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            glfwSetKeyCallback(window, key_callback);
        }
    }
    glfwTerminate();
    return 0;
}