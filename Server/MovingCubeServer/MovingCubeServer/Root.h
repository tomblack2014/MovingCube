#pragma once

#include "NamedObj.h"

class Root : public NamedObj
{
public:
	Root(){};

	void CalMyBytes() override {};

	void UpdateData(const std::vector<char>& data) override {};

	int GetType() override { return 0; };

private:
	std::vector<char> GetDataBytes() override {
		std::vector<char> ret(4);
		*(int*)ret.data() = 0;
		return ret;
	};

};