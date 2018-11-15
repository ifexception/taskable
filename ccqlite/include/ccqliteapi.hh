#pragma once

#ifdef _WIN32

#ifdef CCQLITE_EXPORTS
#define CCQLITE_API __declspec(dllexport)
#else
#define CCQLITE_API __declspec(dllimport)
#endif // CCQLITE_EXPORTS

#endif // _WIN32
