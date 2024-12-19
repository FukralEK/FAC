#pragma once
#include "compat.h"

namespace FAC
{
	FAC_API void saveFile(std::vector<unsigned char> &data, std::string filename);
	FAC_API std::vector<unsigned char> loadFile(std::string filename);
	FAC_API std::vector<unsigned char> loadFile(std::string filename, int start, int end);
	FAC_API uint64_t getFileSize(std::string filename);
}