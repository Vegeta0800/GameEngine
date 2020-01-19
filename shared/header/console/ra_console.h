
#pragma once
//EXTERNAL INCLUDES
#include <map>
#include <string>
//INTERNAL INCLUDES
#include "ra_types.h"
#include "ra_utils.h"
#include "ra_cvar.h"

//Console class to use commands
class Console 
{
	//Define Console as a singleton. See utils.h for definition.
	DEFINE_SINGLETON(Console)

public:
	//Cleanup the console
	void Cleanup();
	
	
	//Set a new cvar
	void SetCVar(const CVar& cvar);


	//Get a specific cvar
	CVar GetCVar(const std::string& name);

private:
	std::map<std::string, const char*> cvars;
};