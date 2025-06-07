export module path;

import std;

import <glad/glad.h>;
import <glm/vec3.hpp>;

import colors;
import drawable;
import glutils;

export class Path : public Drawable
{
public:
	Path(std::vector<glm::vec3>&& path) : path(path)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		ScopedBindArray ba(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, path.size() * sizeof(glm::vec3), &path[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	}

	virtual void draw(const Shader* shader) const override
	{
		shader->setMatrix("model", glm::mat4{ 1.0f });
		shader->setVector("color", colors::green);		
		ScopedBindArray ba(VAO);
		glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(path.size()));
	}

	inline const std::vector<glm::vec3>& getPath() const
	{
		return path;
	}

private:
	const std::vector<glm::vec3> path;
};