#include "pch.h"
#include "NamedObj.h"
#include "Content\SpinningCubeRenderer.h"

using namespace MovingCube;

int NamedObj::NEXT_ID = 0;

NamedObj::NamedObj()
{
	m_bytes = 0;
	m_ID = NEXT_ID++;
	m_parent = nullptr;
}

void NamedObj::Connect(std::shared_ptr<NamedObj> parent, std::shared_ptr<NamedObj> child)
{
	if (!parent || !child)
		throw "NamedObj::Connect : parent | child is nullptr.";
	parent->m_children.push_back(child);
	child->m_parent = parent;
}

void NamedObj::Connect(std::shared_ptr<NamedObj> parent)
{
	if (!parent)
		throw "NamedObj::Connect : parent is nullptr.";
	parent->m_children.push_back(this->GetSharedPtr());
	m_parent = parent;
}
std::string NamedObj::GetName()
{
	if (m_parent) {
		return m_parent->GetName() + std::string("/") + std::to_string(m_ID);
	}
	else
		return std::string("SceneGraph/") + std::to_string(m_ID);
}

int NamedObj::GetBytes()
{
	int byteSum = 0;

	if (m_children.size() == 0) {
		if (m_bytes > 0)
			return m_bytes;
		else {
			CalMyBytes();
			if (m_bytes <= 0) {
				throw "NamedObj bytes <= 0";
			}
			else
				byteSum += m_bytes;
		}
	}
	else {
		for (auto i : m_children) {
			byteSum += i->GetBytes();
		}
	}

	return byteSum;
}

void NamedObj::Serialization(std::vector<char>& header, std::vector<std::vector<char>>& body)
{
	std::vector<char> m_h;
	std::vector<std::vector<char>> m_b;
	m_b.push_back(GetDataBytes());
	m_h.push_back(1);				//add itself

	for (auto i : m_children) {
		std::vector<char> t_h;
		std::vector<std::vector<char>> t_b;

		i->Serialization(t_h, t_b);

		for (auto& j : t_h) {
			j++;
		}

		m_h.insert(m_h.end(), t_h.begin(), t_h.end());
		m_b.insert(m_b.end(), t_b.begin(), t_b.end());
	}

	header = m_h;
	body = m_b;
}

void NamedObj::Desrialization(const std::vector<char>& header, const std::vector<std::vector<char>>& body, std::vector<std::shared_ptr<NamedObj>>& objs, SimpleFactory* factory)
{
	std::vector<char> desStack;
	//objs.clear();

	for (int i = 0; i < header.size(); i++) {
		char num[2] = { 0 };
		num[0] = '0' + i;

		//temporary
		if (i >= objs.size()) {
			int type = *((int*)body[i].data());
			auto obj = factory->createProduct(type);
			objs.push_back(obj);
		}
		if (objs[i]->GetStat() == 0)
			objs[i]->UpdateData(body[i]);
	}

	//erase needless nodes
	while (header.size() < objs.size()) {
		objs.pop_back();
	}

	int count = 0;
	for (auto i : header) {
		if (i <= desStack.size()) {
			while (i <= desStack.size())
				desStack.pop_back();
		}
		if (desStack.size() != 0 && !objs[desStack[desStack.size() - 1]]->CheckChild(objs[count]))
			objs[count]->Connect(objs[desStack[desStack.size() - 1]]);
		desStack.push_back(count);
		count++;
	}

	return;
}

std::shared_ptr<NamedObj> NamedObj::Find(int id)
{
	if (this->m_ID == id)
		return this->GetSharedPtr();

	for (auto i : m_children) {
		auto ret = i->Find(id);
		if (ret)
			return ret;
	}

	return nullptr;
}

int NamedObj::Pack(char** ptr)
{
	std::vector<char> header;
	std::vector<std::vector<char>> body;
	Serialization(header, body);

	int ret = 1 + header.size();
	for (int i = 0; i < body.size(); i++) {
		ret += sizeof(int);
		ret += body[i].size();
	}

	(*ptr) = new char[ret];
	int pos = 0;

	(*ptr)[0] = header.size();
	pos++;
	memcpy_s((*ptr) + pos, (*ptr)[0], header.data(), header.size());
	pos += header.size();

	char buf2[2] = { 0 };
	for (int i = 0; i < header.size(); i++) {
		*((int*)((*ptr) + pos)) = body[i].size();
		pos += sizeof(int);

		memcpy_s((*ptr) + pos, body[i].size(), body[i].data(), body[i].size());
		pos += body[i].size();
	}

	return ret;
}

std::shared_ptr<NamedObj> NamedObj::UnPack(int size, char* ptr, SimpleFactory* factory, std::vector<std::shared_ptr<NamedObj>>& objs)
{
	int nodeSize = ptr[0];
	std::vector<char> header(nodeSize);
	std::vector<std::vector<char>> body;

	int pos = 1;

	for (int i = 0; i < nodeSize; i++) {
		header[i] = ptr[pos];
		pos++;
	}

	for (int i = 0; i < nodeSize; i++) {
		int subSize = *((int*)(ptr + pos));
		pos += sizeof(int);
		std::vector<char> t(subSize);
		memcpy_s(t.data(), subSize, ptr + pos, subSize);
		pos += subSize;
		body.push_back(t);
	}

	Desrialization(header, body, objs, factory);
	auto start = objs[0];
	while (true) {
		if (!start->m_parent)
			break;
		start = start->m_parent;
	}

	for (auto i : objs) {
		i->UpdateBinding(start);
	}

	return start;
}

std::vector<std::shared_ptr<NamedObj>> NamedObj::GetAllNodes()
{
	std::vector<std::shared_ptr<NamedObj>> ret;
	ret.push_back(this->GetSharedPtr());

	for (auto i : m_children) {
		auto tmp = i->GetAllNodes();
		ret.insert(ret.end(), tmp.begin(), tmp.end());
	}

	return ret;
}