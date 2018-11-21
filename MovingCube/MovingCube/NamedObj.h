#pragma once

#include <string>
#include <list>
#include <vector>

#include "Content\MovableObject.h"
#include "SimpleFactory.h"

class NamedObj : public MovingCube::MovableObject, public std::enable_shared_from_this<NamedObj>
{
public:
	std::shared_ptr<NamedObj> GetSharedPtr() { return shared_from_this(); };

	NamedObj();

	~NamedObj() {};

	std::string GetName();

	int GetID() { return m_ID; };

	static void Connect(std::shared_ptr<NamedObj> parent, std::shared_ptr<NamedObj> child);

	void Connect(std::shared_ptr<NamedObj> parent);

	int GetBytes();

	virtual void CalMyBytes() = 0;

	void Serialization(std::vector<char>& header, std::vector<std::vector<char>>& body);

	static void Desrialization(const std::vector<char>& header, const std::vector<std::vector<char>>& body, std::vector<std::shared_ptr<NamedObj>>& objs, SimpleFactory* factory);

	int Pack(char** ptr);

	static std::shared_ptr<NamedObj> UnPack(int size, char* ptr, SimpleFactory* factory, std::vector<std::shared_ptr<NamedObj>>& objs);

	virtual void UpdateData(const std::vector<char>& data) = 0;

	std::shared_ptr<NamedObj> Find(int id);

	std::vector<std::shared_ptr<NamedObj>> GetAllNodes();

	virtual Windows::Foundation::Numerics::float3 GetGlobalPos() { if (m_parent) return m_parent->GetGlobalPos() + GetPosition(); else return GetPosition(); };
protected:

	bool CheckChild(std::shared_ptr<NamedObj> node) {
		for (auto i : m_children) {
			if (i == node)
				return true;
		}
		return false;
	};

	virtual std::vector<char> GetDataBytes() = 0;

	virtual int GetType() = 0;

	virtual void UpdateBinding(std::shared_ptr<NamedObj> root) {};

	int m_ID;
	std::shared_ptr<NamedObj> m_parent;
	std::list<std::shared_ptr<NamedObj>> m_children;
	int m_bytes;

	static int NEXT_ID;
};