#ifndef DEFINE_H
#define DEFINE_H

#ifdef DLL_EXPORTS
#define DLL_EXPORTS __declspec(dllexport)
#else
#define DLL_EXPORTS __declspec(dllimport)
#endif

#endif