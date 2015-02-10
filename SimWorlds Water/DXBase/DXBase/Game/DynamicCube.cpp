#include "DynamicCube.h"
#include "drawdata.h"
#include "gamedata.h"


void DynamicCube::init(int _size, ID3D11Device* GD)
{
	m_size = _size;

	//calculate number of vertices and primitives
	int numVerts = 6 * (m_size - 1) * (m_size - 1);
	m_numPrims = numVerts / 3;
	m_vertices = new myVertex[numVerts];
	WORD* indices = new WORD[numVerts];

	//as using the standard VB shader set the tex-coords somewhere safe
	for (int i = 0; i<numVerts; i++)
	{
		indices[i] = i;
		m_vertices[i].texCoord = Vector2::One;
	}

	//in each loop create the two traingles for the matching sub-square on each of the six faces
	int vert = 0;
	for (int i = -(m_size - 1) / 2; i<(m_size - 1) / 2; i++)
	{
		for (int j = -(m_size - 1) / 2; j<(m_size - 1) / 2; j++)
		{
			//top
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, 0.5f * (float)(m_size - 1), (float)j);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, 0.5f * (float)(m_size - 1), (float)(j + 1));
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), 0.5f * (float)(m_size - 1), (float)j);

			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), 0.5f * (float)(m_size - 1), (float)j);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, 0.5f * (float)(m_size - 1), (float)(j + 1));
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), 0.5f * (float)(m_size - 1), (float)(j + 1));
		}
	}

	//carry out some kind of transform on these vertices to make this object more interesting
	Transform();

	//calculate the normals for the basic lighting in the base shader
	for (int i = 0; i<m_numPrims; i++)
	{
		WORD V1 = 3 * i;
		WORD V2 = 3 * i + 1;
		WORD V3 = 3 * i + 2;

		//build normals
		Vector3 norm;
		Vector3 vec1 = m_vertices[V1].Pos - m_vertices[V2].Pos;
		Vector3 vec2 = m_vertices[V3].Pos - m_vertices[V2].Pos;
		norm = vec1.Cross(vec2);
		norm.Normalize();

		m_vertices[V1].Norm = norm;
		m_vertices[V2].Norm = norm;
		m_vertices[V3].Norm = norm;
	}


	BuildIB(GD, indices);
	BuildVB(GD, numVerts, m_vertices);

	//delete[] m_vertices; //this is no longer needed as this is now in the Vertex Buffer
}

void DynamicCube::Tick(GameData* GD)
{
	Transform();
}

void DynamicCube::Draw(DrawData* _DD)
{

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	//Disable GPU access to the vertex buffer data.
	_DD->pd3dImmediateContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//Update the vertex buffer here.
	memcpy(mappedResource.pData, m_vertices, sizeof(m_vertices));
	//Reenable GPU access to the vertex buffer data.
	_DD->pd3dImmediateContext->Unmap(m_VertexBuffer, 10);

	DynamicVB::Draw(_DD);
}


// A * Sin (w t + y)
//A = the amplitude, is the peak deviation of the function from zero.
//f, = the ordinary frequency, is the number of oscillations(cycles) that occur each second of time.
//ω = 2πf, the angular frequency, is the rate of change of the function argument in units of radians per second 
//y = the phase, specifies(in radians) where in its cycle the oscillation is at t = 0.

void DynamicCube::Transform()
{	
		for (int i = 0; i < m_numPrims * 3; i++)
		{
			Vector3 vertPos = m_vertices[i].Pos;

			Matrix scaleMat = Matrix::CreateScale(10.0f, 1.0f, 10.0f);

			Vector3 newPos = Vector3::Transform(vertPos, scaleMat);

			m_vertices[i].Pos = newPos;
		}


		for (int i = 0; i < m_numPrims * 3; i++)
		{
			float sineWave = amp * sin(((2 * pi*freq)*time) + phase);

			float vertPos = m_vertices[i].Pos.y;

			float newPos = vertPos + sineWave;

			m_vertices[i].Pos.y = newPos;

			time = time + 10;
			phase = phase + (3 * i);
		}
};