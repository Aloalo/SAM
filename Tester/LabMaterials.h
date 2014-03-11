#pragma once

#include <SafeOptix.h>
#include <map>

class LabMaterials
{
public:
	LabMaterials(void);
	~LabMaterials(void);

	enum Materials
	{
		MIRROR = 0,
		GLASS = 1,
		WALL = 2
	};

	optix::Material& getLabyrinthMaterial(int mat);
	void createLabMaterials();
	std::map<int, optix::Material> labmat;
};

