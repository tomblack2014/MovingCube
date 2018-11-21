#pragma once

#include "NamedObj.h"

class Root : public NamedObj
{
public:
	Root();

	void CalMyBytes() override;

	void UpdateData(const std::vector<char>& data) override;

protected:
	std::vector<char> GetDataBytes() override;

	int GetType() override { return 0; };
};