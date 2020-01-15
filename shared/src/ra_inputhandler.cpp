#include "..\header\input\ra_inputhandler.h"
#include "..\header\input\ra_inputhandler.h"
#include "input/ra_inputhandler.h"

//Declare Window as a Singleton
DECLARE_SINGLETON(Input)

void Input::RegisterKey(KeyCode key, ui16 mode)
{
	if (mode == 1)
	{
		this->myData.upKeys[key] = key;
		this->myData.upState = true;

		return;
	}
	else
	{
		if (this->myData.downKeys[key] == key || this->myData.holdKeys[key] == key)
		{
			EradicateKey(key, 0);
			this->myData.holdKeys[key] = key;
			return;
		}
		this->myData.downKeys[key] = key;
		return;
	}
}

void Input::EradicateKey(KeyCode key, ui16 mode)
{
	
	if (mode == 1)
	{
		this->myData.downKeys[key] = KeyCode::NO_CODE;
		this->myData.holdKeys[key] = KeyCode::NO_CODE;
	}
	else
	{
		this->myData.downKeys[key] = KeyCode::NO_CODE;
	}
	return;
}

void Input::EradicateUpKeys()
{
	this->myData.upKeys.clear();
	this->myData.upState = false;
}

bool Input::GetKeyDown(KeyCode key)
{
	if (this->myData.downKeys[key] == key)
		return true;
	return false;
}

bool Input::GetKeyUp(KeyCode key)
{
	if (this->myData.upKeys[key] == key)
		return true;
	return false;
}

bool Input::GetKeyHold(KeyCode key)
{
	if (this->myData.holdKeys[key] == key)
		return true;
	return false;
}

bool Input::GetKey(KeyCode key)
{
	if (this->myData.downKeys[key] == key || this->myData.holdKeys[key] == key)
		return true;
	return false;
}

bool Input::GetUpState(void)
{
	if (this->myData.upState)
		return true;
	return false;
}

bool Input::GetAnyKey(void)
{
	if (this->myData.downKeys.size() != 0 || this->myData.holdKeys.size() != 0)
		return true;
	return false;
}

bool Input::GetOppKeyDown(KeyCode key)
{
	if (this->opponentData.downKeys[key] == key)
		return true;
	return false;
}

bool Input::GetOppKeyUp(KeyCode key)
{
	if (this->opponentData.upKeys[key] == key)
		return true;
	return false;
}

bool Input::GetOppKeyHold(KeyCode key)
{
	if (this->opponentData.holdKeys[key] == key)
		return true;
	return false;
}

bool Input::GetOppKey(KeyCode key)
{
	if (this->opponentData.downKeys[key] == key || this->opponentData.holdKeys[key] == key)
		return true;
	return false;
}

bool Input::GetOppUpState(void)
{
	if (this->opponentData.upState)
		return true;
	return false;
}

bool Input::GetOppAnyKey(void)
{
	if (this->opponentData.downKeys.size() != 0 || this->opponentData.holdKeys.size() != 0)
		return true;
	return false;
}

Data& Input::GetMyData()
{
	return this->myData;
}

Data& Input::GetOpponentData()
{
	return this->opponentData;
}
