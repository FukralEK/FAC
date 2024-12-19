#pragma once
#include "compat.h"

namespace FAC
{
	struct FAC_API File
	{
		std::string name;
		int index;
		std::vector<unsigned char> data;
		File() : name(""), index(0) {}
		File(std::vector<unsigned char>& data, std::string filename) : name(filename), index(0), data(data) {}
	};
	struct FAC_API ArchiveHeader
	{
		std::vector<File> files;
		bool compressed;	// TO DO
		bool encrypted;		// TO DO
		std::string password;
		int dataIndex;
		std::string file;
		ArchiveHeader() : compressed(false), encrypted(false), password(""), dataIndex(0), file("") {}
	};
	
	FAC_API ArchiveHeader loadArchiveHeaderFromFile(std::string file);
	FAC_API void loadFileFromArchive(ArchiveHeader &archive, std::string file);
	FAC_API std::vector<unsigned char> createArchive(ArchiveHeader &header);
}