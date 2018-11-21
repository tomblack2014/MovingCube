#pragma once

#include "NamedObj.h"
#include "ModelData.h"

class Model : public NamedObj
{
public:
	Model();

	void CalMyBytes() override;

	void UpdateData(const std::vector<char>& data) override;

	std::vector<VertexPositionColor> GetVertices() const { return m_vertics; };
	std::vector<unsigned short> GetIndices() const { return m_indices; };

protected:
	std::vector<char> GetDataBytes() override;

	int GetType() override { return 2; };

	std::vector<VertexPositionColor> m_vertics;
	std::vector<unsigned short> m_indices;
};