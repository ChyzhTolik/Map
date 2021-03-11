#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
class Map
{
public:
    static int Run(void);
    static int resol;
    static int x;
    static int y;
    static void SetFiles(std::vector<std::string>* Files, int resol, int x, int y);
    static void Draw(GLFWwindow* window, std::vector<std::string>Files);
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

