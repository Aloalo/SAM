#pragma once

#include <Updateable.h>
#include <glm/glm.hpp>
#include "SafeOptix.h"

namespace trayc
{
	class Entity
	{
	public:
		Entity(void);
		~Entity(void);

		const glm::vec3& getPos() const;
		void setPos(const glm::vec3 &p);

		const glm::vec3& getScale() const;
		void setScale(const glm::vec3 &s);

		optix::Transform trans;
		optix::Group node;
		
	protected:
		void recalcMatrix();

		glm::vec3 scale;
		glm::vec3 pos;
	};
}
