#pragma once

#include "common.hpp"

class PointBatchRenderer
{
	struct Point
	{
		Vector2 position;
		float size;
		Color color;
	};

	uint32_t vbo, vao, shader, projectionShaderLoc;

	Matrix projection;

	uint32_t maxCapacity;
	uint32_t count;

	std::vector<Point> points;

public:
	PointBatchRenderer(uint32_t maxCapacity) :
		maxCapacity(maxCapacity),
		count(0),
		vao(0),
		vbo(0), 
		shader(rlLoadShaderCode(LoadFileText("shaders/pointbatch.vert"), LoadFileText("shaders/pointbatch.frag"))),
		projectionShaderLoc(rlGetLocationUniform(shader, "uProjection")),
		projection()
	{
		vao = rlLoadVertexArray();
		rlEnableVertexArray(vao);

		vbo = rlLoadVertexBuffer(nullptr, maxCapacity * sizeof(Point), true);

		rlEnableVertexAttribute(0);
		rlEnableVertexAttribute(1);
		rlEnableVertexAttribute(2);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (GLvoid*)offsetof(Point, position));
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Point), (GLvoid*)offsetof(Point, size));
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Point), (GLvoid*)offsetof(Point, color));

		rlDisableVertexArray();

		glEnable(GL_PROGRAM_POINT_SIZE);

		points.reserve(maxCapacity);
	}

	void SetProjectionMatrix(const Matrix projection)
	{
		this->projection = projection;
	}

	void Add(const Vector2 position, const float size, const Color color)
	{
		if (count + 1 > maxCapacity)
		{
			Draw();
		}

		points.push_back({position, size, color});
		count++;
	}

	void Draw()
	{
		rlEnableShader(shader);
		rlEnableVertexArray(vao);
		rlEnableVertexBuffer(vbo);
		rlSetUniformMatrix(projectionShaderLoc, projection);
		rlUpdateVertexBuffer(vbo, points.data(), count * sizeof(Point), 0);
		
		glDrawArrays(GL_POINTS, 0, count);

		rlDisableVertexBuffer();
		rlDisableVertexArray();
		rlDisableShader();

		points.clear();
		count = 0;
	}

	~PointBatchRenderer()
	{
		rlUnloadShaderProgram(shader);
		rlUnloadVertexBuffer(vbo);
		rlUnloadVertexArray(vao);
	}
};