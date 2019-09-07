
#pragma once
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "ra_components.h"

class Gameobject;

class Component
{

public:
	virtual void Initialize(const char* comName, ComponentType comType);
	virtual void Update(void);
	virtual void Cleanup(void);
	virtual void SetGameObject(Gameobject* gb);

	ComponentType GetType(void) const;
	Gameobject* GetGameObject(void) const;

	bool operator==(const Component& other);

private:
	Gameobject* gameObject;

	ComponentType type;
	const char* name;

};