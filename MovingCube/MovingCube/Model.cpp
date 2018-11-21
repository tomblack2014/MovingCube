#include "pch.h"
#include "Model.h"

Model::Model()
{

}

void Model::CalMyBytes()
{
	m_bytes = m_vertics.size() * sizeof(VertexPositionColor) + m_indices.size() * sizeof(unsigned short);
}

void Model::UpdateData(const std::vector<char>& data)
{
	int byteCount = sizeof(GetType());

	memcpy_s(&m_ID, sizeof(m_ID), data.data() + byteCount, sizeof(m_ID));
	byteCount += sizeof(m_ID);

	int vSize, iSize;
	memcpy_s(&vSize, sizeof(vSize), data.data() + byteCount, sizeof(vSize));
	byteCount += sizeof(vSize);
	m_vertics.resize(vSize);
	memcpy_s(m_vertics.data(), m_vertics.size() * sizeof(VertexPositionColor), data.data() + byteCount, m_vertics.size() * sizeof(VertexPositionColor));
	byteCount += m_vertics.size() * sizeof(VertexPositionColor);
	memcpy_s(&iSize, sizeof(iSize), data.data() + byteCount, sizeof(iSize));
	byteCount += sizeof(iSize);
	m_indices.resize(iSize);
	memcpy_s(m_indices.data(), m_indices.size() * sizeof(unsigned short), data.data() + byteCount, m_indices.size() * sizeof(unsigned short));
}

std::vector<char> Model::GetDataBytes()
{
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
	memcpy_s(t.data() + byteCount, m_vertics.size() * sizeof(VertexPositionColor), m_vertics.data(), m_vertics.size() * sizeof(VertexPositionColor));
	byteCount += m_vertics.size() * sizeof(VertexPositionColor);
	memcpy_s(t.data() + byteCount, sizeof(iSize), &iSize, sizeof(iSize));
	byteCount += sizeof(iSize);
	memcpy_s(t.data() + byteCount, m_indices.size() * sizeof(unsigned short), m_indices.data(), m_indices.size() * sizeof(unsigned short));
	return t;
}