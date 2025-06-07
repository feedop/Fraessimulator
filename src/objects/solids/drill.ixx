export module drill;

import std;
import <glad/glad.h>;
import <glm/glm.hpp>;
import <glm/ext/matrix_transform.hpp>;

import colors;
import glutils;
import math;
import solidobject;
import shader;

inline constexpr int segmentCount = 20;

// Vertex struct
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
};

// Function to create vertices for a flat-top drill cylinder
std::vector<Vertex> createFlatDrillVertices()
{
    std::vector<Vertex> vertices;
    static constexpr float radius = 0.5f;
    static constexpr float height = 1.0f;

    // Step size for each segment angle in radians
    float angleStep = 2.0f * math::pi / static_cast<float>(segmentCount);
    float halfHeight = height / 2.0f;

    // Top center
    vertices.push_back({
            glm::vec3(0, halfHeight, 0),
            glm::vec3(0, 1, 0)  // Normal pointing up
                       });

    // Top circle vertices
    for (int i = 0; i < segmentCount; ++i)
    {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Top circle vertex
        vertices.push_back({
            glm::vec3(x, halfHeight, z),
            glm::vec3(0, 1, 0)  // Normal pointing up
                           });
    }

    // Bottom center
    vertices.push_back({
            glm::vec3(0, -halfHeight, 0),
            glm::vec3(0, -1, 0)  // Normal pointing up
                       });

    // Bottom circle vertices
    for (int i = 0; i < segmentCount; ++i)
    {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Bottom circle vertex
        vertices.push_back({
            glm::vec3(x, -halfHeight, z),
            glm::vec3(0, -1, 0)  // Normal pointing down
                           });
    }

    // Side vertices (two per segment)
    for (int i = 0; i < segmentCount; ++i)
    {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Top side vertex
        vertices.push_back({
            glm::vec3(x, halfHeight, z),
            glm::normalize(glm::vec3(x, 0, z))  // Outward normal
                           });

        // Bottom side vertex
        vertices.push_back({
            glm::vec3(x, -halfHeight, z),
            glm::normalize(glm::vec3(x, 0, z))  // Outward normal
                           });
    }

    return vertices;
}

// Function to create indices for a flat-top drill cylinder
std::vector<unsigned int> createFlatDrillIndices()
{
    std::vector<unsigned int> indices;

    // Top cap indices
    for (int i = 1; i < segmentCount + 1; ++i)
    {
        int next = (i + 1) % segmentCount + 1;
        indices.push_back(0);  // Center of top cap
        indices.push_back(next); // Next vertex
        indices.push_back(i);     // Current vertex
    }

    // Bottom cap indices
    int bottomOffset = segmentCount + 2;
    for (int i = 0; i < segmentCount; ++i)
    {
        int next = (i + 1) % segmentCount;
        indices.push_back(bottomOffset + i);       // Current vertex
        indices.push_back(bottomOffset + next);     // Next vertex
        indices.push_back(bottomOffset);            // Center of bottom cap
    }

    // Side indices
    int sideOffset = 2 * segmentCount + 2;
    for (int i = 0; i < segmentCount; ++i)
    {
        int next = (i + 1) % segmentCount;

        // Quad split into two triangles
        indices.push_back(sideOffset + i * 2);
        indices.push_back(sideOffset + next * 2);
        indices.push_back(sideOffset + i * 2 + 1);

        indices.push_back(sideOffset + i * 2 + 1);
        indices.push_back(sideOffset + next * 2);
        indices.push_back(sideOffset + next * 2 + 1);
    }

    return indices;
}

std::vector<Vertex> createSphericalDrillVertices()
{
    std::vector<Vertex> vertices;
    static constexpr float radius = 0.5f;

    // Loop through stacks (latitude)
    for (int i = 0; i <= segmentCount; ++i)
    {
        float stackAngle = (math::pi / 2.0f) - (math::pi * i / segmentCount); // from pi/2 to 0 to -pi/2
        float xy = radius * cos(stackAngle);  // radius * cos(lat)
        float y = radius * sin(stackAngle);   // radius * sin(lat)

        // Loop through sectors (longitude)
        for (int j = 0; j <= segmentCount; ++j)
        {
            float sectorAngle = 2.0f * math::pi * j / segmentCount; // 0 to 2pi

            // Vertex position (x, y, z)
            float x = xy * cos(sectorAngle);
            float z = xy * sin(sectorAngle);

            // Vertex normal
            glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));

            vertices.push_back({
                glm::vec3(x, y, z),  // Position
                normal               // Normal
                               });
        }
    }

    return vertices;
}

std::vector<unsigned int> createSphericalDrillIndices()
{
    std::vector<unsigned int> indices;

    // Generate indices
    for (int i = 0; i < segmentCount; ++i)
    {
        int k1 = i * (segmentCount + 1);     // beginning of current stack
        int k2 = k1 + segmentCount + 1;      // beginning of next stack

        for (int j = 0; j < segmentCount; ++j, ++k1, ++k2)
        {
            if (i != (segmentCount - 1)) [[likely]]
            {
                // Quad split into two triangles
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);

                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    return indices;
}


export class Drill : public SolidObject
{
public:
	inline float getRadius() const
	{
		return radius;
	}

	inline float getHeight() const
	{
		return height;
	}

	inline void setHeight(float newHeight)
	{
		height = newHeight;
	}

	inline void setRadius(float newRadius)
	{
		radius = newRadius;
	}

	virtual bool isFlat() const = 0;

    virtual void draw(const Shader* shader) const
    {
        SolidObject::draw(shader);
        shader->setVector("color", colors::cyan);
    }

protected:
	float radius;
	float height;

    Drill(int diameter, float height) : radius(diameter * 0.05f), height(height)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    }
};

export class SphericalDrill : public Drill
{
public:
    SphericalDrill(int diameter, float height) : Drill(diameter, height)
    {
        setBaseModel(glm::translate(glm::mat4{ 1.0f }, { 0, radius, 0 }) * 
                     glm::scale(glm::mat4{ 1.0f }, { radius * 2, height, radius * 2 }) * 
                     glm::translate(glm::mat4{ 1.0f }, { 0, 0.5f, 0}));

        sphereBaseModel = glm::translate(glm::mat4{ 1.0f }, { 0, radius, 0 }) * glm::scale(glm::mat4{ 1.0f }, { radius * 2, radius * 2, radius * 2 });
        setPosition({ 0.0f, 7.0f, 0.0f });

        glBufferData(GL_ARRAY_BUFFER, flatVertices.size() * sizeof(Vertex), &flatVertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, flatIndices.size() * sizeof(unsigned int), &flatIndices[0], GL_STATIC_DRAW);
        glBindVertexArray(0);

        // Create a buffer for the hemisphere part of the drill

        glGenVertexArrays(1, &sphereVAO);
        glGenBuffers(1, &sphereVBO);
        glGenBuffers(1, &sphereEBO);

        ScopedBindArray ba(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(Vertex), &sphereVertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), &sphereIndices[0], GL_STATIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    }

    virtual ~SphericalDrill()
    {
        ScopedBindArray ba(sphereVAO);

        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glDeleteBuffers(1, &sphereVBO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
        glDeleteBuffers(1, &sphereEBO);

        glDeleteVertexArrays(1, &sphereVAO);
    }

    virtual void setPosition(const glm::vec3& newPosition) override
    {
        SolidObject::setPosition(newPosition);
        sphereModel = glm::translate(glm::mat4{ 1.0f }, newPosition) * sphereBaseModel;
    }

	virtual bool isFlat() const override
	{
		return false;
	}

	virtual void draw(const Shader* shader) const
	{     
        {
            Drill::draw(shader);
            ScopedBindArray ba(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(flatIndices.size()), GL_UNSIGNED_INT, 0);
        }
        {
            shader->setMatrix("model", sphereModel);
            ScopedBindArray ba(sphereVAO);      
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sphereIndices.size()), GL_UNSIGNED_INT, 0);
        } 
	}

private:
    inline static const std::vector<Vertex> flatVertices = createFlatDrillVertices();
    inline static const std::vector<unsigned int> flatIndices = createFlatDrillIndices();
    inline static const std::vector<Vertex> sphereVertices = createSphericalDrillVertices();
    inline static const std::vector<unsigned int> sphereIndices = createSphericalDrillIndices();

    unsigned int sphereVAO, sphereVBO, sphereEBO;
    glm::mat4 sphereBaseModel;
    glm::mat4 sphereModel{ 1.0f };
};

export class FlatDrill : public Drill
{
public:
    FlatDrill(int diameter, float height) : Drill(diameter, height)
    {
        setBaseModel(glm::scale(glm::mat4{ 1.0f }, { radius * 2, height, radius * 2 }) *
                     glm::translate(glm::mat4{ 1.0f }, { 0, 0.5f, 0 }));
        setPosition({ 0.0f, 7.0f, 0.0f });

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        glBindVertexArray(0);
    }

	virtual bool isFlat() const override
	{
		return true;
	}

	virtual void draw(const Shader* shader) const
	{
        Drill::draw(shader);
        ScopedBindArray ba(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
	}

private:
    inline static const std::vector<Vertex> vertices = createFlatDrillVertices();
    inline static const std::vector<unsigned int> indices = createFlatDrillIndices();
};