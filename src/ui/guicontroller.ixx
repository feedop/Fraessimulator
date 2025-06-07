export module gui:controller;

import std;

import <imgui.h>;
import <imgui/backend/imgui_impl_glfw.h>;
import <imgui/backend/imgui_impl_opengl3.h>;
import <GLFW/glfw3.h>;

import <glm/vec2.hpp>;
import <glm/vec3.hpp>;

import camera;
import scene;
import renderer;
import theme;

ImGuiIO& createImguiContext()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    return ImGui::GetIO();
}

/// <summary>
/// Responsible for managing the GUI
/// </summary>
export class GuiController
{
public:
    GuiController(GLFWwindow* window, Camera& camera, Scene& scene, Renderer& renderer) :
        io(createImguiContext()), camera(camera), scene(scene), renderer(renderer)
    {
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowMinSize = ImVec2(500, 500);

        cinder::setTheme();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 420");
    }

    void draw()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // imgui debug
        bool showDemo = false;
        if (showDemo)
        {
            ImGui::ShowDemoWindow(&showDemo);
            render();
            return;
        }

        ImGui::Begin("Configuration window", nullptr, ImGuiWindowFlags_MenuBar); // Create a window and append into it.

        renderMenuBar();

        static bool fpsLimit = true;
        if (ImGui::Checkbox("Limit fps to screen frequency", &fpsLimit))
        {
            glfwSwapInterval(fpsLimit);
        }

        ImGui::Checkbox("Render grid", &renderer.drawGrid);
        ImGui::Checkbox("Render path", &renderer.drawPath);
        ImGui::Checkbox("Render material", &renderer.drawMaterial);

        renderConfig();

        ImGui::End();

        render();
    }
private:
    ImGuiIO& io;
    Camera& camera;
    Scene& scene;
    Renderer& renderer;

    void render()
    {
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    enum class IoOperation
    {
        None,
        Open
    };

    // Components
    void renderMenuBar();
    void renderConfig();
    void renderDialogWindow(IoOperation& operation);
};