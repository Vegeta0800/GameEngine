
#pragma once
//EXTERNAL INCLUDES
#include <map>
//INTERNAL INCLUDES
#include "ra_utils.h"
#include "ra_keys.h"

//Define Data packet without padding
#pragma pack(push, 1)
struct DataPacket
{
	bool left, right, shoot;
};
#pragma pack(pop)

//Input class to map keys and hold data on client and opponent
class Input 
{
	DEFINE_SINGLETON(Input)
	
public:
	void RegisterKey(KeyCode key, ui16 mode);

	//Eradicate a key 
	void EradicateKey(KeyCode key, ui16 mode);
	//Eradicate up keys
	void EradicateUpKeys();


	//Get if requested key is down right now
	bool GetKeyDown(KeyCode key);
	//Get if requested key is hold right now
	bool GetKeyHold(KeyCode key);
	//Get if key is either down or hold
	bool GetKey(KeyCode key);
	//Get if requested key is up right now
	bool GetKeyUp(KeyCode key);

	//Get Up state
	bool GetUpState(void);

	//Get clients data packet reference
	DataPacket& GetMyData();
	//Get opponents data packet reference
	DataPacket& GetOpponentData();

private:
	//Key maps
	std::map<KeyCode, KeyCode> downKeys;
	std::map<KeyCode, KeyCode > holdKeys;
	std::map<KeyCode, KeyCode > upKeys;

	DataPacket myData;
	DataPacket opponentData;

	bool upState;
};