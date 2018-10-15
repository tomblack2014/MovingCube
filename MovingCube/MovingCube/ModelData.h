#pragma once

#include "Content\ShaderStructures.h"
#include <vector>
#include <DirectXMath.h>

using namespace MovingCube;
using namespace DirectX;

const static std::vector<VertexPositionColor> cubeVertices =
{ {
	{ XMFLOAT3(-0.1f, -0.1f, -0.1f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
{ XMFLOAT3(-0.1f, -0.1f,  0.1f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
{ XMFLOAT3(-0.1f,  0.1f, -0.1f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
{ XMFLOAT3(-0.1f,  0.1f,  0.1f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
{ XMFLOAT3(0.1f, -0.1f, -0.1f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
{ XMFLOAT3(0.1f, -0.1f,  0.1f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
{ XMFLOAT3(0.1f,  0.1f, -0.1f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
{ XMFLOAT3(0.1f,  0.1f,  0.1f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
	} };

const static std::vector<unsigned short> cubeIndices =
{ {
		2,1,0, // -x
		2,3,1,

		6,4,5, // +x
		6,5,7,

		0,1,5, // -y
		0,5,4,

		2,6,7, // +y
		2,7,3,

		0,4,6, // -z
		0,6,2,

		1,3,7, // +z
		1,7,5,
	} };

const static std::vector<VertexPositionColor> pyramidVertices =
{ {
	{ XMFLOAT3(-0.1f, -0.1f, -0.1f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
{ XMFLOAT3(-0.1f,  -0.1f, 0.1f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
{ XMFLOAT3(0.1f, -0.1f, -0.1f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
{ XMFLOAT3(0.1f,  -0.1f, 0.1f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
{ XMFLOAT3(0.f,  0.1f,  0.f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
	} };

const static std::vector<unsigned short> pyramidIndices =
{ {
		2,0,1,//floor
		2,1,3,

		4,1,0,//side
		4,0,2,
		4,3,1,
		4,2,3,
	} };