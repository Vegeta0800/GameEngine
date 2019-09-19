#include "components/ra_component.h"
#include "ra_gameobject.h"

//Void Initialize
void Component::Initialize(const char* comName, ComponentType comType)
{
	//Set name and type.
	this->name = comName;
	this->type = comType;
}

//Void Update
void Component::Update(void)
{

}

//Void Update
void Component::Cleanup(void)
{
	this->name = "";
	delete this;
}

//GetType.
ComponentType Component::GetType(void) const
{
	//Returns this components component type.
	return this->type;
}

//Compare two Components.
bool Component::operator==(const Component & other)
{
	//Return true if the bytes between this component and the inputed component are the size of the class Component.
	return memcmp(this, &other, sizeof(Component)) == 0;
}
