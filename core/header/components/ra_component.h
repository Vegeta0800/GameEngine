
#pragma once
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "ra_components.h"

class Component
{

public:
	virtual void Initialize(const char* comName, ComponentType comType);
	virtual void Update(void);
	virtual void Cleanup(void);

	ComponentType GetType(void) const;

	bool operator==(const Component& other);

private:
	ComponentType type;
	const char* name;
};