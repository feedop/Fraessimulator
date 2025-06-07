import std;

import <glad/glad.h>;
import <GLFW/glfw3.h>;

import application;
import camera;
import config;
import gui;
import inputhandler;
import scene;
import renderer;
import parser;

#ifdef _WIN32
// Select an NVIDIA GPU if possible
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
#endif


int main()
{
    Application application(cfg::initialWidth, cfg::initialHeight);
    GLFWwindow* window = application.getWindowPtr();

    Camera camera(15.0f);

    Scene scene(camera);

    // Create a graphics controller
    Renderer renderer(cfg::initialWidth, cfg::initialHeight, camera, scene);

    // Setup input handling
    InputHandler inputHandler(window, camera, scene, renderer);

    // Create a GUI controller
    GuiController guiController(window, camera, scene, renderer);

    scene.setDrill(8, false);

    // MAIN LOOP HERE - dictated by glfw
    double lastTime = glfwGetTime();
    int frameCount = 0;

    while (!glfwWindowShouldClose(window))
    {   
        // Update scene if necessary
        scene.performGradualMill();

        // OpenGL render
        renderer.draw();

        // GUI rendering
        guiController.draw();
        glfwSwapBuffers(window);

        // Show FPS in the title bar
        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
        if (delta >= 1.0)
        {
            double fps = double(frameCount) / delta;
            std::stringstream ss;
            ss << "Fraessimulator" << " " << " [" << fps << " FPS]";

            glfwSetWindowTitle(window, ss.str().c_str());
            lastTime = currentTime;
            frameCount = 0;
        }
        else
        {
            frameCount++;
        }

        // Handle user input
        glfwPollEvents();
        inputHandler.handleMouseMovement(window);
    }

    return 0;
}