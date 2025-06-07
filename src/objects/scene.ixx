export module scene;

import std;

import <glm/vec3.hpp>;
import <glm/vec4.hpp>;
import <glm/gtc/constants.hpp>;

import camera;
import cube;
import drill;
import drawable;
import grid;
import material;
import path;
import shader;
import solidobject;
import shader;
import simulation;


export class Scene
{
public:
	Scene(const Camera& camera) : camera(camera)
	{
		
	}

	inline auto& getGrid() const
	{
		return grid;
	}

	inline const auto& getPath() const
	{
		return path;
	}

	inline const auto& getMaterial() const
	{
		return material;
	}

	void setPath(std::vector<glm::vec3>&& pathPositions)
	{
		path.reset(new Path(std::move(pathPositions)));
	}

	inline int getResolution() const
	{
		return resolution;
	}

	inline void setResolution(int newResolution)
	{
		resolution = newResolution;
		material->setResolution(resolution);
	}

	void instantMill()
	{
		simulation.reset(new Simulation(drill.get(), material.get(), path.get(), millingShader.get(), resolution, baseHeight));
		try
		{
			simulation->instantMill();
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << "\n";
		}	
		simulation.reset(nullptr);
	}

	void beginGradualMill()
	{
		simulation.reset(new Simulation(drill.get(), material.get(), path.get(), millingShader.get(), resolution, baseHeight));
		try
		{
			simulation->beginGradualMill(velocity);
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << "\n";
		}	
	}

	void performGradualMill()
	{
		if (!simulation)
			return;

		if (!simulation->isInProgress())
		{
			simulation.reset(nullptr);
			return;
		}

		simulation->performGradualMill();
	}

	bool isMillable() const
	{
		return drill && path && !simulation;
	}

	bool isSimulationInProgress() const
	{
		return simulation.get();
	}

	inline void stopSimulation()
	{
		simulation->stop();
	}

	void setDrill(int size, bool flat)
	{
		if (flat)
			drill.reset(new FlatDrill(size, drillHeight));
		else
			drill.reset(new SphericalDrill(size, drillHeight));
	}

	void setDrillHeight()
	{
		drill->setHeight(drillHeight);
	}

	void setDrillRadius()
	{
		drill->setRadius(drillRadius);
	}

	const auto& getDrill()
	{
		return drill;
	}

private:
	friend class GuiController;

	int resolution = 1500;
	float baseHeight = 1.5f;
	float drillHeight = 2.0f;
	float drillRadius = 0.8f;
	float velocity = 0.2f;

	const Camera& camera;
	const std::unique_ptr<SolidObject> grid = std::make_unique<Grid>();
	
	std::unique_ptr<Drill> drill;
	std::unique_ptr<Material> material = std::make_unique<Material>(resolution);
	std::unique_ptr<Path> path;

	std::unique_ptr<Shader> millingShader = std::make_unique<MillingShader>();

	std::unique_ptr<Simulation> simulation;
};