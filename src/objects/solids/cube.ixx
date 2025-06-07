export module cube;

import std;

import <glm/vec3.hpp>;

import shader;
import texturedobject;

export struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
};

export class Cube
{
public:
	static std::vector<Vertex> createVertices(float cuboidSize)
	{
        std::vector<Vertex> vertices;

        // Cube has 6 faces, with each face made of 2 triangles (6 vertices per face)
        glm::vec3 faceNormals[6] = {
            glm::vec3(0.0f,  0.0f, -1.0f),  // Front face normal (negative Z)
            glm::vec3(0.0f,  0.0f,  1.0f),  // Back face normal (positive Z)
            glm::vec3(-1.0f,  0.0f,  0.0f),  // Left face normal (negative X)
            glm::vec3(1.0f,  0.0f,  0.0f),  // Right face normal (positive X)
            glm::vec3(0.0f, -1.0f,  0.0f),  // Bottom face normal (negative Y)
            glm::vec3(0.0f,  1.0f,  0.0f)   // Top face normal (positive Y)
        };

        glm::vec3 positions[8] = {
            // Front bottom-left (0), front bottom-right (1), front top-right (2), front top-left (3)
            glm::vec3(cuboidSize * -0.5f, 0.0f, cuboidSize * -0.5f),
            glm::vec3(cuboidSize * 0.5f, 0.0f, cuboidSize * -0.5f),
            glm::vec3(cuboidSize * 0.5f,  1.0f, cuboidSize * -0.5f),
            glm::vec3(cuboidSize * -0.5f,  1.0f, cuboidSize * -0.5f),
            // Back bottom-left (4), back bottom-right (5), back top-right (6), back top-left (7)
            glm::vec3(cuboidSize * -0.5f, 0.0f, cuboidSize * 0.5f),
            glm::vec3(cuboidSize * 0.5f, 0.0f, cuboidSize * 0.5f),
            glm::vec3(cuboidSize * 0.5f,  1.0f, cuboidSize * 0.5f),
            glm::vec3(cuboidSize * -0.5f,  1.0f, cuboidSize * 0.5f)
        };

        // Add front face (negative Z)
        vertices.push_back({ positions[0], faceNormals[0] });
        vertices.push_back({ positions[1], faceNormals[0] });
        vertices.push_back({ positions[2], faceNormals[0] });
        vertices.push_back({ positions[2], faceNormals[0] });
        vertices.push_back({ positions[3], faceNormals[0] });
        vertices.push_back({ positions[0], faceNormals[0] });

        // Add back face (positive Z)
        vertices.push_back({ positions[5], faceNormals[1] });
        vertices.push_back({ positions[4], faceNormals[1] });
        vertices.push_back({ positions[7], faceNormals[1] });
        vertices.push_back({ positions[7], faceNormals[1] });
        vertices.push_back({ positions[6], faceNormals[1] });
        vertices.push_back({ positions[5], faceNormals[1] });

        // Add left face (negative X)
        vertices.push_back({ positions[4], faceNormals[2] });
        vertices.push_back({ positions[0], faceNormals[2] });
        vertices.push_back({ positions[3], faceNormals[2] });
        vertices.push_back({ positions[3], faceNormals[2] });
        vertices.push_back({ positions[7], faceNormals[2] });
        vertices.push_back({ positions[4], faceNormals[2] });

        // Add right face (positive X)
        vertices.push_back({ positions[1], faceNormals[3] });
        vertices.push_back({ positions[5], faceNormals[3] });
        vertices.push_back({ positions[6], faceNormals[3] });
        vertices.push_back({ positions[6], faceNormals[3] });
        vertices.push_back({ positions[2], faceNormals[3] });
        vertices.push_back({ positions[1], faceNormals[3] });

        // Add bottom face (negative Y)
        vertices.push_back({ positions[4], faceNormals[4] });
        vertices.push_back({ positions[5], faceNormals[4] });
        vertices.push_back({ positions[1], faceNormals[4] });
        vertices.push_back({ positions[1], faceNormals[4] });
        vertices.push_back({ positions[0], faceNormals[4] });
        vertices.push_back({ positions[4], faceNormals[4] });

        // Add top face (positive Y)
        vertices.push_back({ positions[3], faceNormals[5] });
        vertices.push_back({ positions[2], faceNormals[5] });
        vertices.push_back({ positions[6], faceNormals[5] });
        vertices.push_back({ positions[6], faceNormals[5] });
        vertices.push_back({ positions[7], faceNormals[5] });
        vertices.push_back({ positions[3], faceNormals[5] });

        return vertices;
	}
};