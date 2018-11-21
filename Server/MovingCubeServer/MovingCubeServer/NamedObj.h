#pragma once

#include <string>
#include <list>
#include <vector>

class NamedObj
{
public:
	NamedObj();

	std::string GetName();

	static void Connect(NamedObj* parent, NamedObj* child);

	void Connect(NamedObj* parent);

	int GetBytes();

	virtual void CalMyBytes() = 0;

	void Serialization(std::vector<char>& header, std::vector<std::vector<char>>& body);

	static void Desrialization(const std::vector<char>& header, const std::vector<std::vector<char>>& body, std::vector<NamedObj*>& objs);

	int Pack(char** ptr);

	static void UnPack(int size, char* ptr, std::vector<NamedObj*>& objs);

	virtual void UpdateData(const std::vector<char>& data) = 0;

	NamedObj* Find(int id);

protected:
	virtual int GetType() = 0;

	bool CheckChild(NamedObj* node) {
		for (auto i : m_children) {
			if (i == node)
				return true;
		}
		return false;
	};

	virtual std::vector<char> GetDataBytes() = 0;

	int m_ID;
	NamedObj* m_parent;
	std::list<NamedObj*> m_children;
	int m_bytes;

	static int NEXT_ID;
};