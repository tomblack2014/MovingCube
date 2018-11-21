#pragma once

#include "NamedObj.h"

struct XMFLOAT3
{
	float x;
	float y;
	float z;

	explicit XMFLOAT3(_In_reads_(3) const float *pArray) : x(pArray[0]), y(pArray[1]), z(pArray[2]) {}

	XMFLOAT3& operator= (const XMFLOAT3& Float3) { x = Float3.x; y = Float3.y; z = Float3.z; return *this; }

	XMFLOAT3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

struct VertexPositionColor
{
public:
	VertexPositionColor() : pos(0.f, 0.f, 0.f), color(0.f, 0.f, 0.f) {};
	VertexPositionColor(XMFLOAT3 p, XMFLOAT3 c):pos(p),color(c){};
	VertexPositionColor(const VertexPositionColor& vertex) : pos(vertex.pos), color(vertex.color) {};
	VertexPositionColor& operator= (const VertexPositionColor& vertex) { pos = vertex.pos; color = vertex.color; return *this; }
	XMFLOAT3 pos;
	XMFLOAT3 color;
};

class Model : public NamedObj
{
public:
	Model(const std::vector<VertexPositionColor>& vertics, const std::vector<unsigned short>& indices){
		m_vertics = vertics;
		m_indices = indices;
	};

	void CalMyBytes() override { 
		m_bytes = m_vertics.size() * sizeof(VertexPositionColor) + m_indices.size() * sizeof(unsigned short) + sizeof(m_ID) + sizeof(GetType());
	};

	void UpdateData(const std::vector<char>& data) override {};

	int GetType() override { return 2; };

private:
	std::vector<char> GetDataBytes() override {
		int vSize = m_vertics.size();
		int iSize = m_indices.size();
		int size = sizeof(m_ID) + sizeof(GetType()) + sizeof(vSize) + sizeof(iSize) + m_vertics.size() * sizeof(VertexPositionColor) + m_indices.size() * sizeof(unsigned short);
		std::vector<char> t(size);

		int byteCount = 0;
		int type = GetType();
		memcpy_s(t.data() + byteCount, sizeof(type), &type, sizeof(type));
		byteCount += sizeof(type);
		memcpy_s(t.data() + byteCount, sizeof(m_ID), &m_ID, sizeof(m_ID));
		byteCount += sizeof(m_ID);
		
		memcpy_s(t.data() + byteCount, sizeof(vSize), &vSize, sizeof(vSize));
		byteCount += sizeof(vSize);
		memcpy_s(t.data() + byteCount, m_vertics.size() * sizeof(VertexPositionColor), m_vertics.data() , m_vertics.size() * sizeof(VertexPositionColor));
		byteCount += m_vertics.size() * sizeof(VertexPositionColor);
		memcpy_s(t.data() + byteCount, sizeof(iSize), &iSize, sizeof(iSize));
		byteCount += sizeof(iSize);
		memcpy_s(t.data() + byteCount, m_indices.size() * sizeof(unsigned short), m_indices.data(), m_indices.size() * sizeof(unsigned short));
		return t; 
	};

	std::vector<VertexPositionColor> m_vertics;
	std::vector<unsigned short> m_indices;
};