#include "ra_gameobject.h"
#include "ra_rendering.h"
#include "ra_application.h"

Gameobject::Gameobject() :
	isRoot(false),
	parent(nullptr)
{
}

void Gameobject::Initialize()
{
	this->parent = nullptr;
	this->transform = Transform();
	this->oldPosition = Math::Vec3::zero;
	this->oldEulerRotation = Math::Vec3::zero;
	this->transform.position = Math::Vec3::zero;
	this->transform.eulerRotation = Math::Vec3{0, 0, 0};
	this->transform.rotation = Math::Quaternion::identity;
	this->transform.scaling = Math::Vec3::unit_scale;

	this->mesh = new Mesh;
	this->mesh->indicesCount = 6;

	this->mesh->vertices =
	{
		Vertex{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		Vertex{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		Vertex{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		Vertex{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
	};

	this->mesh->indices = { 0, 1, 2, 2, 3, 0 };
}

void Gameobject::Update()
{
	this->moved = false;
	this->oldPosition = this->transform.position;
	this->oldEulerRotation = this->transform.eulerRotation;

	for (Gameobject* child : this->children)
	{
		if (child != nullptr)
			child->Update();
	}

	for (Component* component : this->components)
	{
		this->transform.position.x += 10.0f;
		this->transform.eulerRotation.x += 10.0f;
	}

	if (!(this->transform.position == this->oldPosition))
	{
		this->moved = true;
		for (Gameobject* child : this->GetAllChildren())
		{
			if (!child->isMoved())
			{
				child->transform.position += (this->transform.position - this->oldPosition);
				child->transform.eulerRotation += (this->transform.eulerRotation - this->oldEulerRotation);
			}
		}	
	}
}

void Gameobject::Cleanup()
{
	for (Gameobject* child : this->children)
	{
		if (child != nullptr)
		{
			child->Cleanup();
			delete child;
		}
	}

	for (Component* component : this->components)
	{
		delete component;
	}

	delete this->mesh;

	delete this;
}

void Gameobject::SetParent(Gameobject* parent)
{
	//Set this node to the parent of the inputed node.
	if (!this->isRoot)
		this->parent = parent;

	this->parent->children.push_back(this);
}

void Gameobject::SetName(const char* name)
{
	this->name = name;
}

void Gameobject::AddComponent(Component* component)
{
	this->components.push_back(component);
}


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

void Gameobject::MakeRoot()
{
	//if this node has a parent.
	if (this->parent)
	{
		//Remove this node from its parent children list.
		this->parent->children.remove(this);

		//Delete all Parents with its child objects.
		this->DeleteParent();

		//Set this nodes parent to null pointer.
		this->parent = nullptr;

		this->isRoot = true;
	}

	this->isRoot = true;

	//Define this node as the root.
}

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

std::list<Gameobject*> Gameobject::GetChildren()
{
	return this->children;
}

std::list<Gameobject*> Gameobject::GetAllChildren()
{
	std::list<Gameobject*> temp;
	this->ListAllChildren(temp);
	return temp;
}

Gameobject* Gameobject::GetParent()
{
	return this->parent;
}

bool Gameobject::hasRoot()
{
	return this->isRoot;
}

bool Gameobject::isMoved()
{
	return this->moved;
}

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

//Compare two nodes if they are equal or not.
bool Gameobject::operator==(Gameobject* other)
{
	//Return true if the bytes between this node and the inputed node are the size of the class Node.
	return memcmp(this, other, sizeof(Gameobject)) == 0;
}

Transform& Gameobject::GetTransform()
{
	return this->transform;
}

Math::Mat4x4 Gameobject::GetModelMatrix()
{
	this->modelMatrix = Math::Mat4x4::identity;
	this->modelMatrix = this->modelMatrix * Math::CreateRotationMatrix(this->transform.eulerRotation);
	this->modelMatrix = this->modelMatrix * Math::CreateScalingMatrix(this->transform.scaling);
	this->modelMatrix = this->modelMatrix * Math::Transpose(Math::CreateTranslationMatrix(this->transform.position));

	return this->modelMatrix;
}

Mesh* Gameobject::GetMesh()
{
	return this->mesh;
}
