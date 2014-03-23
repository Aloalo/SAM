#include "Entity.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

namespace trayc
{
	Entity::Entity(void)
	{
	}

	Entity::~Entity(void)
	{
	}

	const vec3& Entity::getPos() const
	{
		return pos;
	}

	void Entity::setPos(const vec3 &p)
	{
		pos = p;
		recalcMatrix();
	}

	const vec3& Entity::getScale() const
	{
		return scale;
	}
	
	void Entity::setScale(const vec3 &s)
	{
		scale = s;
		recalcMatrix();
	}
	
	void Entity::recalcMatrix()
	{
		mat4 mat(translate(glm::scale(mat4(1.0f), scale), pos));
		trans->setMatrix(false, (float*)&mat, (float*)&inverse(mat));
	}
}
