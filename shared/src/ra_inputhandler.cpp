#include "input/ra_inputhandler.h"

//Declare Window as a Singleton
DECLARE_SINGLETON(Input)

void Input::RegisterKey(KeyCode key, ui16 mode)
{
	if (mode == 1)
	{
		this->upKeys[key] = key;
		this->upState = true;
		return;
	}
	else
	{
		if (this->downKeys[key] == key || this->holdKeys[key] == key)
		{
			EradicateKey(key, 0);
			this->holdKeys[key] = key;
			return;
		}
		this->downKeys[key] = key;
		return;
	}
}

void Input::EradicateKey(KeyCode key, ui16 mode)
{
	
	if (mode == 1)
	{
		this->downKeys[key] = KeyCode::NO_CODE;
		this->holdKeys[key] = KeyCode::NO_CODE;
	}
	else
	{
		this->downKeys[key] = KeyCode::NO_CODE;
	}
	return;
}

void Input::EradicateUpKeys()
{
	this->upKeys.clear();
	this->upState = false;
}

bool Input::GetKeyDown(KeyCode key)
{
	if (this->downKeys[key] == key)
		return true;
	return false;
}

bool Input::GetKeyUp(KeyCode key)
{
	if (this->upKeys[key] == key)
		return true;
	return false;
}

bool Input::GetKeyHold(KeyCode key)
{
	if (this->holdKeys[key] == key)
		return true;
	return false;
}

bool Input::GetKey(KeyCode key)
{
	if (this->downKeys[key] == key || this->holdKeys[key] == key)
		return true;
	return false;
}

bool Input::GetUpState(void)
{
	if (this->upState)
		return true;
	return false;
}

bool Input::GetAnyKey(void)
{
	if (this->downKeys.size() != 0 || this->holdKeys.size() != 0)
		return true;
	return false;
}
