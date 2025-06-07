export module material;

import std;

import <glad/glad.h>;
import <glm/mat4x4.hpp>;
import <glm/ext/matrix_transform.hpp>;

import drawable;
import colors;
import glutils;
import math;
import shader;

export class Material : public Drawable
{
public:
	Material(int newResolution)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		
		setResolution(newResolution);
		generateHeightMap();
	}

	virtual ~Material()
	{
		glDeleteTextures(1, &texId);
	}

	void setResolution(int newResolution)
	{
		resolution = newResolution;
		points.clear();
		points.reserve(resolution * resolution);

		for (int i = 0; i < resolution; i++)
		{
			for (int j = 0; j < resolution; j++)
			{
				points.push_back(
					{static_cast<float>(i) / (resolution - 1),
					static_cast<float>(j) / (resolution - 1)});
			}
		}

		// Copy offset data
		ScopedBindArray ba(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * points.size(), &points[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	}

	virtual void draw(const Shader* shader) const override
	{
		shader->setMatrix("model", model);
		shader->setVector("color", colors::white);
		shader->setInt("resolution", resolution);
		shader->setFloat("quadSize", 1.0f / (resolution - 1.0f));
		ScopedBindArray ba(VAO);
		glBindTexture(GL_TEXTURE_2D, texId);
		glDrawArrays(GL_POINTS, 0, points.size());
	}

	unsigned int getHeightTexture() const
	{
		return texId;
	}

private:
	std::vector<glm::vec2> points;

	glm::mat4 model = glm::scale(glm::mat4{ 1.0f }, { 15.0f, 5.0f, 15.0f }) * glm::translate(glm::mat4{ 1.0f }, { -0.5f, 0.0f, -0.5f });
	std::vector<glm::vec2> offsets;
	unsigned int texId, instanceVBO;
	int resolution;

	void generateHeightMap()
	{
		glDeleteTextures(1, &texId);
		glGenTextures(1, &texId);

		// Bind the texture as a 2D texture
		glBindTexture(GL_TEXTURE_2D, texId);
		auto initialData = generateHeightMapData();

		// Allocate texture memory for a single-channel 1500x1500 texture with float16 data
		glTexImage2D(
			GL_TEXTURE_2D,      // Target texture type
			0,                  // Mipmap level (0 is the base level)
			GL_R32F,            // Internal format (one channel, float32)
			resolution,			// Width of the texture
			resolution,			// Height of the texture
			0,                  // Border (must be 0)
			GL_RED,             // Format of the pixel data (single channel, red)
			GL_FLOAT,			// Data type (float32)
			initialData.data()	// Initial data
		);

		// Set texture parameters (wrapping and filtering)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Unbind the texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	std::vector<float> generateHeightMapData()
	{
		std::vector<float> heightMapData(resolution * resolution);

		// Loop over each pixel in the texture
		for (int y = 0; y < resolution; ++y)
		{
			for (int x = 0; x < resolution; ++x)
			{
				//// Normalize x and y to the range [-1, 1]
				//float normX = (float(x) / (resolution - 1)) * 2.0f - 1.0f;
				//float normY = (float(y) / (resolution - 1)) * 2.0f - 1.0f;

				// Calculate height value based on z = x^2 + y^2
				float heightValue = 1.0f;// (2.0f - normX * normX - normY * normY) * 0.2f;

				// Store as half float (GLhalf)
				heightMapData[y * resolution + x] = heightValue;
			}
		}

		return heightMapData;
	}
};