#ifndef _DYNAMICCUBE_H_
#define _DYNAMICCUBE_H_
#include "DynamicVB.h"
#include "vertex.h"
#include <math.h> 
#include "gamedata.h"

//procedurally generate a VBGO Cube
//each side be divided in to _size * _size squares (2 triangles per square)

class DynamicCube : public DynamicVB
{
public:

	float amp = 10.0f;
	float freq = 1.0f;
	float pi = 3.14f;
	float phase = 10.0f;
	float time = 0.0f;

	DynamicCube(){};
	virtual ~DynamicCube(){};

	//initialise the Veretx and Index buffers for the cube
	void init(int _size, ID3D11Device* _GD);
	void Draw(DrawData*_DD);
	void Tick(GameData* GD);

protected:
	//this is to allow custom versions of this which create the basic cube and then distort it
	//see Spirla, SpikedVB and Pillow
	
	virtual void Transform();
	virtual void TransformSize();

	int m_size;
	myVertex* m_vertices;
};

#endif