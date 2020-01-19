
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "console/ra_console.h"

//Singletons
DECLARE_SINGLETON(Console)

//Cleanup the console
void Console::Cleanup()
{
	this->cvars.clear();
}


//Set a new cvar
void Console::SetCVar(const CVar& cvar)
{
	this->cvars[cvar.name] = cvar.cvarValue;
}



//Get a specific cvar
CVar Console::GetCVar(const std::string& name)
{
	CVar cvar = { };
	cvar.name = name.c_str();
	cvar.cvarValue = this->cvars[name];

	return cvar;
}