export module simulation;

import std;

import <glad/glad.h>;
import <glm/vec3.hpp>;
import <glm/gtc/matrix_transform.hpp>;

import drill;
import glutils;
import material;
import path;
import shader;
import math;

export class Simulation
{
public:
	Simulation(Drill* drill, Material* material, const Path* path, const Shader* shader, int resolution, float baseHeight) :
		drill(drill), material(material), path(path), shader(shader), resolution(resolution), baseHeight(baseHeight)
	{
		createAtomicCounterBuffer();
	}

	~Simulation()
	{
		glDeleteBuffers(1, &atomicCounterBuffer);
	}

	void stop()
	{
		inProgress = false;
		index = 0;
		if (velocityBoundPath.size() > 0)
			drill->setPosition(velocityBoundPath[0]);
	}

	void instantMill()
	{
		validatePath();
		resetAtomicCounter();
		bindResources();

		auto&& points = path->getPath();
		for (int i = 0; i < points.size() - 1; i++)
		{
			performMill(points[i], points[i + 1]);
		}
	}

	// Velocity is cm/frame
	void beginGradualMill(float newVelocity)
	{
		validatePath();
		resetAtomicCounter();
		velocity = newVelocity;
		matchPathToVelocity();
		drill->setPosition(velocityBoundPath[0]);
		index = 0;
		inProgress = true;
	}

	void performGradualMill()
	{
		if (!inProgress)
			return;

		if (index >= velocityBoundPath.size() - 1)
		{
			velocityBoundPath.clear();
			inProgress = false;
			return;
		}

		bindResources();
		performMill(velocityBoundPath[index], velocityBoundPath[index + 1]);
		drill->setPosition(velocityBoundPath[index + 1]);
		index++;
	}

	inline bool isInProgress() const
	{
		return inProgress;
	}

private:
	Drill* drill;
	Material* material;
	const Path* path;
	const Shader* shader;
	int resolution;
	float baseHeight;

	float velocity;
	bool inProgress;
	std::vector<glm::vec3> velocityBoundPath;
	int index;
	unsigned int atomicCounterBuffer;

	void bindResources()
	{
		shader->use();
		shader->setInt("resolution", resolution);
		shader->setFloat("radius", drill->getRadius());
		shader->setInt("flatMill", drill->isFlat());
		shader->setFloat("millHeight", drill->getHeight());
		glBindImageTexture(0, material->getHeightTexture(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	}

	void performMill(const glm::vec3& p1, const glm::vec3& p2)
	{
		shader->setVector("p1", p1);
		shader->setVector("p2", p2);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicCounterBuffer);
		glDispatchCompute(resolution, resolution, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
		if (checkForErrors())
		{
			std::cerr << "An illegal milling attempt has been detected during the simulation. Aborting.\n";
			stop();
		}
	}

	void matchPathToVelocity()
	{
		auto&& points = path->getPath();
		if (points.size() < 2)
		{
			// No segments to process if less than 2 points
			velocityBoundPath = points;
		}

		// Iterate over each segment in the line strip
		for (size_t i = 0; i < points.size() - 1; ++i)
		{
			glm::vec3 p1 = points[i];
			glm::vec3 p2 = points[i + 1];

			float segmentLength = glm::distance(p1, p2);

			// Check if this segment needs subdivision
			if (segmentLength <= velocity)
			{
				// No subdivision needed, add the first point
				velocityBoundPath.push_back(p1);
			}
			else
			{
				// Calculate the number of subdivisions needed
				int numSubdivisions = std::ceil(segmentLength / velocity);

				// Calculate the subdivision step size
				glm::vec3 step = (p2 - p1) / static_cast<float>(numSubdivisions);

				// Add all the intermediate points
				for (int j = 0; j < numSubdivisions; ++j)
				{
					velocityBoundPath.push_back(p1 + step * static_cast<float>(j));
				}
			}
		}

		// Add the last point of the line strip to complete the sequence
		velocityBoundPath.push_back(points.back());
	}

	void createAtomicCounterBuffer()
	{
		glGenBuffers(1, &atomicCounterBuffer);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounterBuffer);
		GLuint initialValue = 0;
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &initialValue, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	}

	// Reset atomic counter buffer to zero
	void resetAtomicCounter()
	{
		GLuint resetValue = 0;
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounterBuffer);
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &resetValue);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	}

	// Check if an error occurred by reading the atomic counter value
	bool checkForErrors()
	{
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounterBuffer);
		GLuint* ptr = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT);
		bool errorOccurred = (*ptr > 0);
		glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
		return errorOccurred;
	}

	void validatePath()
	{
		auto&& points = path->getPath();
		for (auto&& point : points)
		{
			if (point.y < baseHeight  - math::eps)
				throw std::runtime_error("An illegal milling attempt has been detected during preprocessing: trying to mill in the base");
		}
	}
};