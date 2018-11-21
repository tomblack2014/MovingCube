#include "pch.h"
#include "Root.h"

Root::Root() 
{
}

void Root::CalMyBytes()
{
}

void Root::UpdateData(const std::vector<char>& data)
{
}

std::vector<char> Root::GetDataBytes() 
{
	std::vector<char> ret(4);
	*(int*)ret.data() = 0;
	return ret;
}