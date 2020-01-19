
#pragma once
//EXTERNAL INCLUDES
#include <list>
//INTERNAL INCLUDES
#include "math/ra_mat4x4.h"
#include "ra_transform.h"

//Class that stores object information and updates scene graph.
class Gameobject
{
public:
	//Initialize Gameobject with a lot of standard values
	void Initialize(Gameobject* parent = nullptr, std::string name = "", bool active = true);
	//Update this gameobject and all childs recursivly.
	void Update();
	//Cleanup gameobject and all childs recursivly
	void Cleanup();
	

	//Set name of gameobject
	void SetName(std::string name = "");

	//Set parent
	void SetParent(Gameobject* parent);
	//Delete the parent and all parents above that
	void DeleteParent();

	//Add a child to this gameobject
	void AddChild(Gameobject* child);
	//Remove child from list
	void DeleteChild(Gameobject* child, Gameobject* parent = nullptr);
	//Delete all children from children list
	void DeleteChildren();


	//Return this gameobjects children list
	std::list<Gameobject*> GetChildren();
	//Return this gameobjects and all childrens of children list.
	std::list<Gameobject*> GetAllChildren();

	//Get this gameobjects parent
	Gameobject* GetParent();

	//Get refrence to transform
	Transform& GetTransform();

	//Get this gameobjects model matrix
	Math::Mat4x4 GetModelMatrix();

	//Get model ID used by this gameobject
	ui32& GetModelID();

	//Get gameobjects name
	std::string GetName();
	//Get gameobjects tag
	std::string& GetTag();

	//Get collision radius
	float& GetRadius();

	//Is this gameobject the root node.
	bool& hasRoot();
	//Get reference to if collision is active or not
	bool& hasCollision();

	//Get if the gameobject should be rendered or not.
	bool& GetIsRenderable();
	//Get active
	bool& GetIsActive();
	//Get if all buffers are created for the object
	bool& GetBufferCreated();
	//Get if gameobject has a trigger collider
	bool& GetIsTrigger();

	//Compare two nodes if they are equal or not.
	bool operator==(Gameobject* other);

private:
	//List all children recursivly
	void ListAllChildren(std::list<Gameobject*>& list);

	Math::Mat4x4 modelMatrix;

	std::string name;
	std::string tag;
	
	std::list<Gameobject*> children;

	Transform transform;
	Gameobject* parent;

	float collisionRadius;

	ui32 modelID;

	bool isTriggerCollider = false;
	bool activeCollision = true;

	bool isRoot = false;
	bool renderable = true;
	bool bufferCreated = false;
	bool active = true;
}; 