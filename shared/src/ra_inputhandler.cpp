
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "input/ra_inputhandler.h"

//Declare Window as a Singleton
DECLARE_SINGLETON(Input)

//Register a hit key
void Input::RegisterKey(KeyCode key, ui16 mode)
{
	//If mode is one, that means a key got released
	if (mode == 1)
	{
		this->upKeys[key] = key;
		this->upState = true;

		return;
	}
	//If other mode
	else
	{
		//Eradicate key out of down key mapping if already inside
		//And set hold key to key
		if (this->downKeys[key] == key || this->holdKeys[key] == key)
		{
			EradicateKey(key, 0);
			this->holdKeys[key] = key;
			return;
		}

		//Set down key if not yet in mapping.
		this->downKeys[key] = key;
		return;
	}
}

//Eradicate a key 
void Input::EradicateKey(KeyCode key, ui16 mode)
{
	//If mode is one
	if (mode == 1)
	{
		//Set down key and hold key to NO_CODE
		this->downKeys[key] = KeyCode::NO_CODE;
		this->holdKeys[key] = KeyCode::NO_CODE;
	}
	else
	{
		//Set down key to NO_CODE
		this->downKeys[key] = KeyCode::NO_CODE;
	}
	return;
}
//Eradicate up keys
void Input::EradicateUpKeys()
{
	//Clear all up keys and set up state to false
	this->upKeys.clear();
	this->upState = false;
}


//Get if requested key is down right now
bool Input::GetKeyDown(KeyCode key)
{
	if (this->downKeys[key] == key)
		return true;
	return false;
}
//Get if requested key is hold right now
bool Input::GetKeyHold(KeyCode key)
{
	if (this->holdKeys[key] == key)
		return true;
	return false;
}
//Get if key is either down or hold
bool Input::GetKey(KeyCode key)
{
	if (this->downKeys[key] == key || this->holdKeys[key] == key)
		return true;
	return false;
}
//Get if requested key is up right now
bool Input::GetKeyUp(KeyCode key)
{
	if (this->upKeys[key] == key)
		return true;
	return false;
}

//Get Up state
bool Input::GetUpState(void)
{
	if (this->upState)
		return true;
	return false;
}

//Get clients data packet reference
DataPacket& Input::GetMyData()
{
	return this->myData;
}
//Get opponents data packet reference
DataPacket& Input::GetOpponentData()
{
	return this->opponentData;
}