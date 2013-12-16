#pragma once

struct Mouse
{
	Mouse(void)
		: speed(0.005f), locked(true)
	{
	}

	float speed;
	bool locked;
};

