// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for
// future builds. This also affects IntelliSense performance, including code
// completion and many code browsing features. However, files listed here are
// ALL re-compiled if any one of them is updated between builds. Do not add
// files here that you will be updating frequently as this negates the
// performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"
#include <string>
#ifdef TTSLLibBuild
#define TTSLLib __declspec(dllexport)
#else
#define TTSLLib __declspec(dllimport)
#endif

constexpr int MOD_VERSION = 0;
const std::string VERSION_STRING = "v0.1";
#endif // PCH_H
