
#pragma once
// EXTERNAL INCLUDES
#include <comdef.h>
#include <cstdio>
#include <iostream>
#include <chrono>


// Converts degrees to radians.
#define DegToRad(angleDegrees) (angleDegrees * M_PI / 180.0f)

// Converts radians to degrees.
#define RadToDeg(angleRadians) (angleRadians * 180.0f / M_PI)

#define TIMING

#ifdef TIMING
#define INIT_TIMER auto start = std::chrono::high_resolution_clock::now();
#define START_TIMER  start = std::chrono::high_resolution_clock::now();
#define STOP_TIMER(name)  std::cout << "RUNTIME of " << name << ": " << \
    std::chrono::duration_cast<std::chrono::milliseconds>( \
            std::chrono::high_resolution_clock::now()-start \
    ).count() << " ms " << std::endl; 
#else
#define INIT_TIMER
#define START_TIMER
#define STOP_TIMER(name)
#endif

//Defines for pointer. Delete, Delete an array and release.
#define SAFE_DELETE(x) if(x) { delete x; x = nullptr; }
#define SAFE_DELETE_ARRAY(x) if(x) { delete[] x; x = nullptr; }
#define SAFE_RELEASE(x) if(x) { x->Release(); x = nullptr; }

//Define a forbid for the copy constructor.
#define FORBID_COPY(Type)					\
private:									\
	Type(const Type&);						

//Define a forbid for the constructor.
#define FORBID_CONSTRUCTION(Type)			\
private:									\
	Type() { };								\

//Functions to get the instance or instance pointer. 
//Defines function to delete the instance.
#define DEFINE_SINGLETON(Type)				\
FORBID_COPY(Type)							\
FORBID_CONSTRUCTION(Type)					\
public:										\
	static Type& GetInstance(void)			\
	{										\
		if (!instance)						\
		{									\
			instance = new Type();			\
		}									\
		return *instance;					\
	}										\
	static Type* GetInstancePtr(void)		\
	{										\
		if (!instance)						\
		{									\
			instance = new Type();			\
		}									\
		return instance;					\
	}										\
	static void Release(void)				\
	{										\
		SAFE_DELETE(instance)				\
	}										\
											\
protected:									\
	static Type* instance;

//Declare a Singleton
#define DECLARE_SINGLETON(Type)				\
Type* Type::instance = nullptr;

//Define an output message in the console if the debug mode is active.
#if defined(_DEBUG)
#define LOG(x, ...) { char string[128]; snprintf(string, 128, x, __VA_ARGS__); printf("[INFO]: [%s]\n", string); }
#else
#define LOG(x, ...)
#endif

//Define error and warning messages.
#define WARN(x, ...) { char string[128]; snprintf(string, 128, x, __VA_ARGS__); printf("[WARNING]: [%s] (%s #%i)\n", string, __FILE__, __LINE__); }
#define ERR(x, ...) { char string[128]; snprintf(string, 128, x, __VA_ARGS__); printf("[ERROR]: [%s] (%s #%i)\n", string, __FILE__, __LINE__); }

//Define a function that is able to give the error message of an HRESULT to the ERR define. That way the HRESULT Error is getting outputted.
#define V_RETURN(x) hr = x; if (hr != S_OK) { _com_error err(hr); LPCTSTR errMsg = err.ErrorMessage(); ERR("%s", errMsg); throw; }

#define GetVariableName(var) #var

#define SET_STRING(x, y) { for (int i = 0; i < y.length(); i++) { x[i] = y[i]; }}