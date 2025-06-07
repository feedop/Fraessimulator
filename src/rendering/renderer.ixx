export module renderer;

import std;

import <glad/glad.h>;

import <glm/vec3.hpp>;
import <glm/vec4.hpp>;
import <glm/gtc/matrix_transform.hpp>;

import camera;
import canvas;
import colors;
import solidobject;
import scene;
import shader;


/// <summary>
/// Responsible for rendering the scene using OpenGL
/// </summary>
export class Renderer
{
public:
	friend class GuiController;

	Renderer(int windowWidth, int windowHeight, Camera& camera, Scene& scene) :
		windowWidth(windowWidth), windowHeight(windowHeight),
		camera(camera), scene(scene)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_PROGRAM_POINT_SIZE);
	}

	/// <summary>
	/// Render the scene
	/// </summary>
	void draw() const
	{
		// Clear
		glViewport(0, 0, windowWidth, windowHeight);
		glClearColor(0.15f, 0.0f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		/*glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);*/

		drawScene();

		// Grid
		glDisable(GL_CULL_FACE);
		if (drawGrid)
		{
			gridShader->use();
			gridShader->setMatrix("view", camera.getView());
			gridShader->setMatrix("projection", camera.getProjection());
			scene.getGrid()->draw(gridShader.get());
		}
	}

	void setWindowSize(int width, int height)
	{
		windowWidth = width;
		windowHeight = height;
		glViewport(0, 0, windowWidth, windowHeight);
	}

private:
	Camera& camera;
	Scene& scene;

	std::unique_ptr<Shader> flatTextureShader = std::make_unique<FlatTextureShader>();
	std::unique_ptr<Shader> gridShader = std::make_unique<InfiniteGridShader>();
	std::unique_ptr<Shader> uniformColorShader = std::make_unique<UniformColorShader>();
	std::unique_ptr<Shader> materialShader = std::make_unique<MaterialShader>();
	std::unique_ptr<Shader> drillShader = std::make_unique<DrillShader>();

	int windowWidth;
	int windowHeight;

	bool drawGrid = true;
	bool drawPath = true;
	bool drawMaterial = false;

	void drawScene() const
	{
		auto& path = scene.getPath();
		if (path && drawPath)
		{
			uniformColorShader->use();
			uniformColorShader->setMatrix("view", camera.getView());
			uniformColorShader->setMatrix("projection", camera.getProjection());
			path->draw(uniformColorShader.get());
		}
		
		materialShader->use();
		if (drawMaterial)
		{
			materialShader->setVector("viewPos", camera.getPosition());
			materialShader->setMatrix("view", camera.getView());
			materialShader->setMatrix("projection", camera.getProjection());
			scene.getMaterial()->draw(materialShader.get());
		}

		drillShader->use();
		drillShader->setVector("viewPos", camera.getPosition());
		drillShader->setMatrix("view", camera.getView());
		drillShader->setMatrix("projection", camera.getProjection());
		scene.getDrill()->draw(drillShader.get());
	}
};