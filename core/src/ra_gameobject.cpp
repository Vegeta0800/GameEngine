#include "ra_gameobject.h"
#include "ra_texture.h"

Gameobject::Gameobject() :
	isRoot(false)
{
	this->Initialize();
}

void Gameobject::Initialize(Gameobject* parent, const char* name, const char* meshName, const char* textureName, bool render, bool act, bool instanced)
{
	this->parent = nullptr;
	this->transform = Transform();
	this->oldPosition = Math::Vec3::zero;
	this->oldEulerRotation = Math::Vec3::zero;
	this->transform.position = Math::Vec3::zero;
	this->transform.eulerRotation = Math::Vec3{0, 0, 0};
	this->transform.rotation = Math::Quaternion::identity;
	this->transform.scaling = Math::Vec3::unit_scale;

	this->name = name;
	this->meshName = meshName;
	this->textureName = textureName;
	this->active = act;
	this->renderable = render;
	this->instanced = instanced;

	this->mesh.CreateMesh(meshName);
	this->texture = new Texture;

	//this->material.fragColor = Math::Vec3{ 0.164f, 0.749f, 0.874f };
	this->material.fragColor = fColorRGBA{ 0, 1, 0, 1 };
	this->material.specularColor = fColorRGBA{1.4f, 1.4f, 1.4f, 1};
	this->material.ambientValue = 0.1f;
	this->material.specularValue = 16.0f;

	if (parent != nullptr)
		this->SetParent(parent);
}

void Gameobject::Initialize(Gameobject* copyGb, const char* name, bool render)
{
	this->parent = nullptr;
	this->transform = copyGb->GetTransform();
	this->oldPosition = copyGb->GetTransform().position;
	this->oldEulerRotation = copyGb->GetTransform().eulerRotation;
	this->transform.position = copyGb->GetTransform().position;
	this->transform.eulerRotation = copyGb->GetTransform().eulerRotation;
	this->transform.rotation = copyGb->GetTransform().rotation;
	this->transform.scaling = copyGb->GetTransform().scaling;

	this->name = name;
	this->meshName = copyGb->GetMeshName();
	this->textureName = copyGb->GetTextureName();

	this->mesh.CreateMesh(this->meshName);

	this->active = copyGb->GetIsActive();
	this->renderable = render;
	this->instanced = copyGb->GetIsInstanced();

	//this->material.fragColor = Math::Vec3{ 0.164f, 0.749f, 0.874f };
	this->material.fragColor = fColorRGBA{ 0, 1, 0, 1 };
	this->material.specularColor = fColorRGBA{ 1.4f, 1.4f, 1.4f, 1 };
	this->material.ambientValue = 0.1f;
	this->material.specularValue = 16.0f;

	if (copyGb->GetParent() != nullptr)
		this->SetParent(copyGb->GetParent());
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
		}
	}

	for (Component* component : this->components)
	{
		delete component;
	}

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
	return Math::CreateModelMatrix((this->transform.position * 0.1f), this->transform.scaling, this->transform.eulerRotation);
}

Material& Gameobject::GetMaterial()
{
	return this->material;
}

const char* Gameobject::GetName()
{
	return this->name;
}

const char* Gameobject::GetMeshName()
{
	return this->meshName;
}

const char* Gameobject::GetTextureName()
{
	return this->textureName;
}

Texture* Gameobject::GetTexture()
{
	return this->texture;
}

Mesh& Gameobject::GetMesh()
{
	return this->mesh;
}

ui32& Gameobject::GetModelID()
{
	return this->modelID;
}

bool& Gameobject::GetIsActive()
{
	return this->active;
}

bool& Gameobject::GetIsRenderable()
{
	return this->renderable;
}

bool& Gameobject::GetIsInstanced()
{
	return this->instanced;
}


