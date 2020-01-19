
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "ra_gameobject.h"
#include "components/ra_component.h"
#include "physic/ra_rigidbody.h"
#include "ra_texture.h"

//Initialize Gameobject with a lot of standard values
void Gameobject::Initialize(Gameobject* parent, std::string name, bool active)
{
	this->parent = nullptr;

	this->transform = Transform();
	this->transform.position = Math::Vec3{ 0, 0, 0 };
	this->transform.eulerRotation = Math::Vec3{0, 0, 0};
	this->transform.rotation = Math::Quaternion::identity;
	this->transform.scaling = Math::Vec3::unit_scale;

	this->name = name;
	this->active = active;
	this->tag = "";

	if (parent != nullptr)
		this->SetParent(parent);
}
//Update this gameobject and all childs recursivly.
void Gameobject::Update()
{
	//TODO ADD MATRIX

	for (Gameobject* child : this->children)
	{
		if (child != nullptr)
			child->Update();
	}
}
//Cleanup gameobject and all childs recursivly
void Gameobject::Cleanup()
{
	for (Gameobject* child : this->children)
	{
		if (child != nullptr)
		{
			child->Cleanup();
		}
	}

	delete this;
}


//Set name of gameobject
void Gameobject::SetName(std::string name)
{
	this->name = name;
}

//Set parent
void Gameobject::SetParent(Gameobject* parent)
{
	//Set this node to the parent of the inputed node.
	if (!this->isRoot)
		this->parent = parent;

	//Add this gameobject to parents children
	this->parent->children.push_back(this);
}
//Delete the parent and all parents above that
void Gameobject::DeleteParent()
{
	//If this node has no parent.
	if (!this->parent)
	{
		//Recursive function to delete all children.
		this->DeleteChildren();
		return;
	}

	//Recursive function to do the same for the parent nodes.
	this->parent->DeleteParent();
}

//Add a child to this gameobject
void Gameobject::AddChild(Gameobject* child)
{
	if (child)
	{
		//Add the inputed node into this nodes children list.
		this->children.push_back(child);

		//If the inputed node isnt the root
		if (!child->isRoot)
			//Set the inputed nodes parent to this.
			child->parent = this;
	}
}
//Remove child from list
void Gameobject::DeleteChild(Gameobject* child, Gameobject* parent)
{
	if (child)
	{
		//Add the inputed node into this nodes children list.
		this->children.remove(child);

		//If the inputed node isnt the root
		if (!child->isRoot)
			//Set the inputed nodes parent to this.
			child->parent = parent;
	}
}
//Delete all children from children list
void Gameobject::DeleteChildren()
{
	//for all nodes in this nodes children list.
	for (Gameobject* child : this->children)
	{
		//Recursive function to open all children in all childs of this node.
		child->DeleteChildren();
	}

	//Delete this child. Because all children execute this function, all children with all child objects are deleted.
	delete this;
}


//Return this gameobjects children list
std::list<Gameobject*> Gameobject::GetChildren()
{
	return this->children;
}
//Return this gameobjects and all childrens of children list.
std::list<Gameobject*> Gameobject::GetAllChildren()
{
	std::list<Gameobject*> temp;
	this->ListAllChildren(temp);
	return temp;
}

//Get this gameobjects parent
Gameobject* Gameobject::GetParent()
{
	return this->parent;
}

//Get refrence to transform
Transform& Gameobject::GetTransform()
{
	return this->transform;
}

//Get this gameobjects model matrix
Math::Mat4x4 Gameobject::GetModelMatrix()
{
	return Math::CreateModelMatrix((this->transform.position * 0.1f), this->transform.scaling, this->transform.eulerRotation);
}

//Get model ID used by this gameobject
ui32& Gameobject::GetModelID()
{
	return this->modelID;
}

//Get gameobjects name
std::string Gameobject::GetName()
{
	return this->name;
}
//Get gameobjects tag
std::string& Gameobject::GetTag()
{
	return this->tag;
}

//Get collision radius
float& Gameobject::GetRadius()
{
	return this->collisionRadius;
}

//Is this gameobject the root node.
bool& Gameobject::hasRoot()
{
	return this->isRoot;
}
//Get reference to if collision is active or not
bool& Gameobject::hasCollision()
{
	return this->activeCollision;
}

//Get if the gameobject should be rendered or not.
bool& Gameobject::GetIsRenderable()
{
	return this->renderable;
}
//Get active
bool& Gameobject::GetIsActive()
{
	return this->active;
}
//Get if all buffers are created for the object
bool& Gameobject::GetBufferCreated()
{
	return this->bufferCreated;
}
//Get if gameobject has a trigger collider
bool& Gameobject::GetIsTrigger()
{
	return this->isTriggerCollider;
}


//Compare two nodes if they are equal or not.
bool Gameobject::operator==(Gameobject* other)
{
	//Return true if the bytes between this node and the inputed node are the size of the class Node.
	return memcmp(this, other, sizeof(Gameobject)) == 0;
}


//List all children recursivly
void Gameobject::ListAllChildren(std::list<Gameobject*>& list)
{
	for (Gameobject* node : this->children)
	{
		//Recursive function to open all children in all childs of this node.
		if (node != nullptr)
		{
			node->ListAllChildren(list);
		}
	}

	if (this->parent)
		list.push_back(this);
}