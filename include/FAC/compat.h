#pragma once

#include <vector>
#include <string>
#include <cstdint>

#ifdef _WIN32
#ifdef IS_FAC_BUILT
#define FAC_API __declspec(dllexport)
#else 
#define FAC_API __declspec(dllimport)
#endif
#endif

#ifndef FAC_API
#define FAC_API
#endif

#include "FACException.h"