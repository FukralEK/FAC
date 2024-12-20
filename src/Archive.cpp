#include <FAC/FAC.h>
#include <filesystem>

#define MODE_NONE 0
#define MODE_READING_FILE_NAME 1
#define MODE_READING_FILE_INDEX 2

constexpr auto FLAG_COMPRESSED = (1 << 1);;
constexpr auto FLAG_ENCRYPTED = (1 << 2);;

namespace fs = std::filesystem;


void FAC::ArchiveHeader::loadArchive()
{
	if (!fs::exists(filename))
	{
		throw FAC::Exception(FAC::NOT_EXISTING_FILE, "The file does not exist");
	}
	if (fs::is_directory(filename))
	{
		throw FAC::Exception(FAC::PATH_IS_DIRECTORY, "The path is a directory");
	}

	files.clear();

	// Load the header with flags
	auto headerData = FAC::loadFile(filename, 0, 6);
	if (headerData[0] == 0x69 && headerData[1] == 0xFF && headerData[2] == 0xAA && headerData[3] == 0xCC)
	{
		if (headerData[4] != FILE_FORMAT_VERSION)
		{
			throw FAC::Exception(FAC::INVALID_VERSION, "Version of the file format is incompatible with the current version of the tool");
		}
		if (headerData[5] & FLAG_COMPRESSED)
		{
			compressed = true;
		}
		if (headerData[5] & FLAG_ENCRYPTED)
		{
			encrypted = true;
		}
	}
	else  {
		throw FAC::Exception(FAC::WRONG_HEADER, "FAC file format was not recognized");
	}

	// Reading the data index
	headerData = FAC::loadFile(filename, 6, 6+8);
	dataIndex = FAC::bytesToInt64(headerData);

	// Reading the file table
	headerData = FAC::loadFile(filename, 5 + 8, dataIndex);

	char mode = MODE_NONE;
	int lastFile = -1;
	std::vector<unsigned char> temp; // Temp Value for index binary value

	for (int i = 0; i < headerData.size(); i++)
	{
		lastFile = files.size()-1;
		if (headerData[i] == 0x21 && headerData[i+1] == 0xFF && headerData[i+2] == 0xAA && headerData[i+3] == 0xCC)
		{
			mode = MODE_READING_FILE_NAME;
			i += 3;
			files.push_back(FAC::File());
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
			files[lastFile].name.push_back(headerData[i]);
		}
		if (mode == MODE_READING_FILE_INDEX)
		{
			temp.push_back(headerData[i]);
			if (temp.size() == 8)
			{
				files[lastFile].index = FAC::bytesToInt64(temp);
				temp.clear();
			}
		}
	}
}

void FAC::ArchiveHeader::loadFile(int index)
{

	int start = dataIndex + files[index].index;
	int end = 0;

	if (index + 1 == files.size())
	{
		end = FAC::getFileSize(filename);
	}
	else end = dataIndex + files[index + 1].index;

	files[index].data = FAC::loadFile(filename, start, end);
}

std::vector<unsigned char> FAC::ArchiveHeader::binary()
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

	if (compressed)
	{
		flags |= FLAG_COMPRESSED;
	}
	if (encrypted)
	{
		flags |= FLAG_ENCRYPTED;
	}

	archive.push_back(flags);

	// Creating File Table
	int fileSizes = 0;
	for (auto& file : files)
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


	for (auto& file : files)
	{
		data.insert(data.end(), file.data.begin(), file.data.end());
	}

	// File Data Index
	dataIndex = archive.size() + 8 + fileTable.size();
	auto indexInBin = FAC::int64ToBytes(dataIndex);
	archive.insert(archive.end(), indexInBin.begin(), indexInBin.end());

	archive.insert(archive.end(), fileTable.begin(), fileTable.end());
	archive.insert(archive.end(), data.begin(), data.end());

	return archive;
}

