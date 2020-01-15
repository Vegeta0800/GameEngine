
#pragma once
//EXTERNAL INCLUDES
#include <map>
//INTERNAL INCLUDES
#include "ra_utils.h"
#include "ra_keys.h"

#pragma pack(push, 1)
struct Data
{
	std::map<KeyCode, KeyCode> downKeys;
	std::map<KeyCode, KeyCode> holdKeys;
	std::map<KeyCode, KeyCode> upKeys;
	bool upState;
};
#pragma pack(pop)

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

	bool GetOppKeyDown(KeyCode key);
	bool GetOppKeyUp(KeyCode key);
	bool GetOppKeyHold(KeyCode key);
	bool GetOppKey(KeyCode key);
	bool GetOppUpState(void);
	bool GetOppAnyKey(void);

	Data& GetMyData();
	Data& GetOpponentData();

private:
	Data myData;
	Data opponentData;
};