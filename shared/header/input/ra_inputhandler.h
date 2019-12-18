
#pragma once
//EXTERNAL INCLUDES
#include <map>
//INTERNAL INCLUDES
#include "ra_utils.h"
#include "ra_keys.h"

class Input 
{
	DEFINE_SINGLETON(Input)
	
public:
	void RegisterKey(KeyCode key, ui16 mode);
	void EradicateKey(KeyCode key, ui16 mode);
	void EradicateUpKeys();

	bool GetKeyDown(KeyCode key);
	bool GetKeyUp(KeyCode key);
	bool GetKeyHold(KeyCode key);
	bool GetKey(KeyCode key);
	bool GetUpState(void);
	bool GetAnyKey(void);

private:
	std::map<KeyCode, KeyCode> downKeys;
	std::map<KeyCode, KeyCode> holdKeys;
	std::map<KeyCode, KeyCode> upKeys;

	bool upState;
};