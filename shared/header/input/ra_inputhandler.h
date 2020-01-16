
#pragma once
//EXTERNAL INCLUDES
#include <map>
//INTERNAL INCLUDES
#include "ra_utils.h"
#include "ra_keys.h"

#pragma pack(push, 1)
struct DataPacket
{
	bool left, right, shoot;
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

	DataPacket& GetMyData();
	DataPacket& GetOpponentData();
private:
	std::map<KeyCode, KeyCode> downKeys;
	std::map<KeyCode, KeyCode > holdKeys;
	std::map<KeyCode, KeyCode > upKeys;

	bool upState;

	DataPacket myData;
	DataPacket opponentData;
};