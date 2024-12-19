#include <FAC/FAC.h>
#include <filesystem>

#define MODE_NONE 0
#define MODE_READING_FILE_NAME 1
#define MODE_READING_FILE_INDEX 2

constexpr auto FLAG_COMPRESSED = (1 << 1);;
constexpr auto FLAG_ENCRYPTED = (1 << 2);;

namespace fs = std::filesystem;

FAC::ArchiveHeader FAC::loadArchiveHeaderFromFile(std::string file)
{
	ArchiveHeader header;

	// Load the header with flags
	auto headerData = FAC::loadFile(file, 0, 6);
	if (headerData[0] == 0x69 && headerData[1] == 0xFF && headerData[2] == 0xAA && headerData[3] == 0xCC)
	{
		if (headerData[4] != FILE_FORMAT_VERSION)
		{
			throw FAC::Exception(FAC::INVALID_VERSION, "Version of the file format is incompatible with the current version of the tool");
			return {};
		}
		if (headerData[5] & FLAG_COMPRESSED)
		{
			header.compressed = true;
		}
		if (headerData[5] & FLAG_ENCRYPTED)
		{
			header.encrypted = true;
		}
	}
	else  {
		throw FAC::Exception(FAC::WRONG_HEADER, "FAC file format was not recognized");
		return {};
	}

	header.file = file;

	if (!fs::exists(file))
	{
		throw FAC::Exception(FAC::NOT_EXISTING_FILE, "The file does not exist");
		return {};
	}
	if (fs::is_directory(file))
	{
		throw FAC::Exception(FAC::PATH_IS_DIRECTORY, "The path is a directory");
		return {};
	}

	// Reading the data index
	headerData = FAC::loadFile(file, 6, 6+8);
	header.dataIndex = FAC::bytesToInt64(headerData);

	// Reading the file table
	headerData = FAC::loadFile(file, 5 + 8, header.dataIndex);

	char mode = MODE_NONE;
	int lastFile = -1;
	std::vector<unsigned char> temp; // Temp Value for index binary value

	for (int i = 0; i < headerData.size(); i++)
	{
		lastFile = header.files.size()-1;
		if (headerData[i] == 0x21 && headerData[i+1] == 0xFF && headerData[i+2] == 0xAA && headerData[i+3] == 0xCC)
		{
			mode = MODE_READING_FILE_NAME;
			i += 3;
			header.files.push_back(FAC::File());
			continue;
		}
		if (headerData[i] == 0x37 && headerData[i + 1] == 0xFF && headerData[i + 2] == 0xAA && headerData[i + 3] == 0xCC)
		{
			mode = MODE_READING_FILE_INDEX;
			i += 3;
			continue;
		}
		if (lastFile == -1)
		{
			continue;
		}
		if (mode == MODE_READING_FILE_NAME)
		{
			header.files[lastFile].name.push_back(headerData[i]);
		}
		if (mode == MODE_READING_FILE_INDEX)
		{
			temp.push_back(headerData[i]);
			if (temp.size() == 8)
			{
				header.files[lastFile].index = FAC::bytesToInt64(temp);
				temp.clear();
			}
		}
	}

	return header;
}

void FAC::loadFileFromArchive(ArchiveHeader &archive, std::string file)
{
	int fileToGet = -1;
	for (int i = 0; i < archive.files.size(); i++)
	{
		if (archive.files[i].name == file)
		{
			fileToGet = i;
		}
	}
	if (fileToGet == -1)
	{
		throw FAC::Exception(FAC::FILE_NOT_FOUND, "The file was not found");
		return;
	}

	int start = archive.dataIndex + archive.files[fileToGet].index;
	int end = 0;

	if (fileToGet + 1 == archive.files.size())
	{
		end = FAC::getFileSize(archive.file);
	}
	else end = archive.dataIndex + archive.files[fileToGet + 1].index;

	archive.files[fileToGet].data = FAC::loadFile(archive.file, start, end);
}

std::vector<unsigned char> FAC::createArchive(ArchiveHeader &header)
{
	std::vector<unsigned char> archive;

	std::vector<unsigned char> fileTable;

	std::vector<unsigned char> data;

	// Header
	archive.push_back(0x69);
	archive.push_back(0xFF);
	archive.push_back(0xAA);
	archive.push_back(0xCC);
	
	// Version
	archive.push_back(FILE_FORMAT_VERSION);

	// TO DO: Flags
	char flags = 0x0;

	if (header.compressed)
	{
		flags |= FLAG_COMPRESSED;
	}
	if (header.encrypted)
	{
		flags |= FLAG_ENCRYPTED;
	}

	archive.push_back(flags);

	// Creating File Table
	int fileSizes = 0;
	for (auto& file : header.files)
	{
		fileTable.push_back(0x21);
		fileTable.push_back(0xFF);
		fileTable.push_back(0xAA);
		fileTable.push_back(0xCC);

		fileTable.insert(fileTable.end(), file.name.begin(), file.name.end());

		fileTable.push_back(0x37);
		fileTable.push_back(0xFF);
		fileTable.push_back(0xAA);
		fileTable.push_back(0xCC);

		file.index = fileSizes;
		auto indexInBin = FAC::int64ToBytes(fileSizes);

		fileTable.insert(fileTable.end(), indexInBin.begin(), indexInBin.end());

		fileSizes += file.data.size();
	}


	for (auto& file : header.files)
	{
		data.insert(data.end(), file.data.begin(), file.data.end());
	}

	// File Data Index
	header.dataIndex = archive.size() + 8 + fileTable.size();
	auto indexInBin = FAC::int64ToBytes(header.dataIndex);
	archive.insert(archive.end(), indexInBin.begin(), indexInBin.end());

	archive.insert(archive.end(), fileTable.begin(), fileTable.end());
	archive.insert(archive.end(), data.begin(), data.end());

	fileTable.clear();
	data.clear();

	return archive;
}
