#pragma once

#include "Model.h"

class Sphere
{
public:
	static void GetSphere(std::vector<VertexPositionColor>& m_Vertices, std::vector<unsigned short>& m_Indices) {
		const float PI = 3.141592653f;

		const int nLongitudes = 200;	/*经线方向上顶点个数，>2*/
		const int nLatitudes = 200;	/*纬线方向上顶点个数，>2*/


		int m_vertexCount = (nLongitudes - 2) * nLatitudes + 2;
		int m_indexCount = 6 * nLatitudes * (nLongitudes - 2);

		// Create the vertex array.
		m_Vertices = std::vector<VertexPositionColor>(m_vertexCount);

		// Create the index array.
		m_Indices = std::vector<unsigned short>(m_indexCount);

		XMFLOAT3 position = XMFLOAT3(0, 0, 0);
		XMFLOAT3 tPos = position;
		float radius = 0.2f;
		tPos.y += radius;
		m_Vertices[0].pos = tPos;
		for (int i = 1; i <= (nLongitudes - 2) * nLatitudes; i++) {
			float tmpAngleA = PI * ((i - 1) / nLatitudes) / (nLongitudes - 1);
			float tmpAngleB = 2 * PI * ((i - 1) % nLatitudes) / nLatitudes;
			tPos.y = position.y + radius * cos(tmpAngleA);
			tPos.x = position.x + radius * sin(tmpAngleA) * cos(tmpAngleB);
			tPos.z = position.x + radius * sin(tmpAngleA) * sin(tmpAngleB);
			m_Vertices[i].pos = tPos;
			m_Vertices[i].color.x = i * 1.f / ((nLongitudes - 2) * nLatitudes);
			m_Vertices[i].color.y = i * 1.f / ((nLongitudes - 2) * nLatitudes) / 2;
			m_Vertices[i].color.z = i * 1.f / ((nLongitudes - 2) * nLatitudes) / 3;
			if (tmpAngleB >= 2 * PI || tmpAngleA >= PI) {
				tmpAngleB = tmpAngleB;
			}
		}
		tPos = position;
		tPos.y -= radius;
		m_Vertices[m_vertexCount - 1].pos = tPos;

		for (int i = 0; i < nLatitudes; i++) {
			m_Indices[i * 3 + 0] = i + 1;
			m_Indices[i * 3 + 1] = 0;
			if (i != nLatitudes - 1)
				m_Indices[i * 3 + 2] = i + 2;
			else
				m_Indices[i * 3 + 2] = 1;
		}

		int count = nLatitudes * 3;
		for (int i = 1; i <= (nLongitudes - 3) * nLatitudes; i++) {

			m_Indices[count + 0] = i;
			if (i % nLatitudes != 0) {
				m_Indices[count + 1] = i + 1;
				m_Indices[count + 2] = i + 1 + nLatitudes;
				m_Indices[count + 4] = i + 1 + nLatitudes;
			}
			else {
				m_Indices[count + 1] = i - nLatitudes + 1;
				m_Indices[count + 2] = i + 1;
				m_Indices[count + 4] = i + 1;
			}
			m_Indices[count + 3] = i;
			m_Indices[count + 5] = i + nLatitudes;
			count += 6;
		}


		for (int i = (2 * nLongitudes - 5) * nLatitudes; i < (2 * nLongitudes - 4) * nLatitudes; i++) {
			int temp = (2 * nLongitudes - 4) * nLatitudes - i;
			temp = m_vertexCount - (temp + 1);
			m_Indices[i * 3 + 0] = temp;
			m_Indices[i * 3 + 2] = m_vertexCount - 1;
			if (i != (2 * nLongitudes - 4) * nLatitudes - 1)
				m_Indices[i * 3 + 1] = temp + 1;
			else
				m_Indices[i * 3 + 1] = m_vertexCount - (nLatitudes + 1);
		}
	};
};