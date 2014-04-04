#pragma once

#include "GameObject.h"
#include "Mesh.h"

class BatteryObject : public GameObject
{
public:
	void init(ID3D10Device* device, ID3D10EffectMatrixVariable* fx,ID3D10EffectMatrixVariable* fx2, float r, Vector3 pos, Vector3 vel, float sp, Vector3 s);
	int getPower() { return power; }
private:
	Mesh battery;
	int power;
};