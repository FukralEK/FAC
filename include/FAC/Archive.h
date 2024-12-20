#pragma once
#include "compat.h"

namespace FAC
{
	class FAC_API File
	{
	public:
		std::string name;
		int index;
		std::vector<unsigned char> data;
		File() : name(""), index(0) {}
	};
	class FAC_API ArchiveHeader
	{
	public:
		std::vector<File> files;
		bool compressed;	// TO DO
		bool encrypted;		// TO DO
		std::string password;
		int dataIndex;
		ArchiveHeader() : compressed(false), encrypted(false), password(""), dataIndex(0), filename("") {}

		ArchiveHeader(std::string filename) : compressed(false), encrypted(false), password(""), dataIndex(0), filename(filename) {
			loadArchive();
		}

		void loadFile(int index);

		std::vector<unsigned char> binary();
	private:
		void loadArchive();
		std::string filename;
	};
}