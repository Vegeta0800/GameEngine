
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "components/ra_component.h"

//Initialize for all components
void Component::Initialize(const char* comName, ComponentType comType)
{
	//Set name and type.
	this->name = comName;
	this->type = comType;
}
//Update for all components
void Component::Update(void)
{

}
//Cleanup for all components
void Component::Cleanup(void)
{
	this->name = "";
	delete this;
}

//GetType
ComponentType Component::GetType(void) const
{
	//Returns this components component type.
	return this->type;
}

//Get name of Component
const char* Component::GetName(void)
{
	return this->name;
}


//Compare two Components.
bool Component::operator==(const Component & other)
{
	//Return true if the bytes between this component and the inputed component are the size of the class Component.
	return memcmp(this, &other, sizeof(Component)) == 0;

}