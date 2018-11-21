#pragma once

#include "NamedObj.h"

class Cube : public NamedObj
{
public:
	Cube(float p[]) { 
		for (int i = 0; i < 3; i++) {
			pos[i] = p[i];
		}
		modelID = 13;
	};

	void CalMyBytes() override { m_bytes = sizeof(pos) + sizeof(modelID) + sizeof(m_ID) + sizeof(GetType()); };

	void UpdateData(const std::vector<char>& data) override {
		int byteCount = sizeof(GetType());
		memcpy_s(&modelID, sizeof(modelID), data.data() + byteCount, sizeof(modelID)); 
		byteCount += sizeof(modelID);
		byteCount += sizeof(m_ID);
		memcpy_s(pos, sizeof(pos), data.data() + byteCount, sizeof(pos));

		modelID = modelID == 11 ? 12 : 11;
	};

	int GetType() override { return 1; }

private:
	std::vector<char> GetDataBytes() override { 
		int byteCount = 0;
		int type = GetType();
		int size = sizeof(pos) + sizeof(modelID) + sizeof(m_ID) + sizeof(type);
		std::vector<char> t(size);
		memcpy_s(t.data(), sizeof(type), &type, sizeof(type));
		byteCount += sizeof(type);
		memcpy_s(t.data() + byteCount, sizeof(modelID), &modelID, sizeof(modelID));
		byteCount += sizeof(modelID);
		byteCount += sizeof(m_ID);
		memcpy_s(t.data() + byteCount, sizeof(pos), pos, sizeof(pos));
		return t; 
	};

	float pos[3];
	int modelID;
};