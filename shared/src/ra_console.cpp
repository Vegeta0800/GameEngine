#include "console/ra_console.h"

DECLARE_SINGLETON(Console)

void Console::SetCVar(const CVar& cvar)
{
	this->cvars[cvar.name] = cvar.cvarValue;
}

CVar Console::GetCVar(const std::string& name)
{
	CVar cvar = { };
	cvar.name = name.c_str();
	cvar.cvarValue = this->cvars[name];

	return cvar;
}

void Console::CleanUp()
{
	this->cvars.clear();
}
