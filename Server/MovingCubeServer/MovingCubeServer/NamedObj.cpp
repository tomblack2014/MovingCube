#include "NamedObj.h"
#include "Cube.h"

int NamedObj::NEXT_ID = 0;

NamedObj::NamedObj()
{
	m_ID = NEXT_ID++;
	m_bytes = 0;
	m_parent = nullptr;
}

void NamedObj::Connect(NamedObj* parent, NamedObj* child)
{
	if (!parent || !child)
		throw "NamedObj::Connect : parent | child is nullptr.";
	parent->m_children.push_back(child);
	child->m_parent = parent;
}

void NamedObj::Connect(NamedObj* parent)
{
	if (!parent)
		throw "NamedObj::Connect : parent is nullptr.";
	parent->m_children.push_back(this);
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
	std::vector<char> typeByte(4);
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

void NamedObj::Desrialization(const std::vector<char>& header, const std::vector<std::vector<char>>& body, std::vector<NamedObj*>& objs)
{
	std::vector<char> desStack;
	//objs.clear();

	for (int i = 0; i < header.size(); i++) {
		char num[2] = { 0 };
		num[0] = '0' + i;

		//need to add a factory like MovingCube, but not yet(cause now cliet cannot create a new object, just update data)
		/*if (i >= objs.size()) {
			float pos[3];
			memcpy_s(pos, sizeof(pos), body[i].data(), body[i].size());
			NamedObj* obj = new Cube(pos);
			objs.push_back(obj);
		}*/

		objs[i]->UpdateData(body[i]);
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

NamedObj* NamedObj::Find(int id)
{
	if (this->m_ID == id)
		return this;

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

void NamedObj::UnPack(int size, char* ptr, std::vector<NamedObj*>& objs)
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

	Desrialization(header, body, objs);
	/*auto start = objs[0];
	while (true) {
		if (!start->m_parent)
			return start;
		start = start->m_parent;
	}*/

}