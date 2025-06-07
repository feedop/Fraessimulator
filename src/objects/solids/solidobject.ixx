export module solidobject;

import std;

import <glad/glad.h>;
import <glm/mat4x4.hpp>;

import drawable;
import math;
import shader;


export class SolidObject : public Drawable
{
public:
	inline const glm::vec3& getPosition() const
	{
		return position;
	}

	virtual void setPosition(const glm::vec3& newPosition)
	{
		position = newPosition;
		update();
	}

	inline void setBaseModel(const glm::mat4& newModel)
	{
		baseModel = newModel;
		update();
	}

	virtual void draw(const Shader* shader) const override
	{
		shader->setMatrix("model", model);
	}

protected:
	glm::mat4 baseModel{ 1.0f };
	glm::mat4 model{ 1.0f };

	glm::vec3 position{ 0.0f, 0.0f, 0.0f };

	SolidObject() = default;
	SolidObject(const glm::vec3& position) : position(position)
	{
		update();
	}

	virtual void update()
	{
		model = math::translate(position) * baseModel;
	}
};