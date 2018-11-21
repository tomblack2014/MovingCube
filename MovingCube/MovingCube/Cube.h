#pragma once

#include "NamedObj.h"

class Cube : public NamedObj
{
public:
	Cube(std::string name, float p[]) : NamedObj(name) {
		for (int i = 0; i < 3; i++) {
			pos[i] = p[i];
		}
	};

	void CalMyBytes() override { m_bytes = sizeof(pos); };

	void UpdateData(const std::vector<char>& data) override { pos[0] = data[0]; };

private:
	std::vector<char> GetDataBytes() override { std::vector<char> t(12); t[0] = pos[0]; return t; };

	float pos[3];
};