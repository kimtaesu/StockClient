#pragma once
#ifdef MATHLIBRARY_EXPORTS
#define MATHLIBRARY_API __declspec(dllexport) 
#else
#define MATHLIBRARY_API __declspec(dllimport) 
#endif

	class Functions
	{
	public:

		// Returns a + b
		static MATHLIBRARY_API char* GetID();

		// Returns a * b
		static MATHLIBRARY_API char* GetPass();

		// Returns a + (a * b)
		static MATHLIBRARY_API char* GetPublicPass();
	};