
#pragma once
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "ra_components.h"

class Component
{
public:
	//Initialize for all components
	virtual void Initialize(const char* comName, ComponentType comType);
	//Update for all components
	virtual void Update(void);
	//Cleanup for all components
	virtual void Cleanup(void);


	//GetType
	ComponentType GetType(void) const;

	//Get name of Component
	const char* GetName(void);


	//Compare two Components.
	bool operator==(const Component& other);

private:
	ComponentType type;
	const char* name;
};