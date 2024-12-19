#include <stdio.h>
#include <FAC/FAC.h>
#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;

#define OPERATION_NONE 0
#define OPERATION_UNPACK 1
#define OPERATION_PACK 2

struct Operation
{
	char type = OPERATION_NONE;
	std::string output;
	std::string input;
	bool compressed = false;
	bool encrypted = false;
};

std::vector<std::string> parseArguments(int argc, char** argv);
Operation getOperation(std::vector<std::string> &arguments);
void printOperationInfo(Operation operation);
void packArchive(Operation operation);
void unpackArchive(Operation operation);
void printHelp();

int main(int argc, char** argv)
{
	printf("FAC Tools for FAC Version %i\n", FILE_FORMAT_VERSION);
	printf("Made by FukralEK. Open Source\n\n\n");
	auto arguments = parseArguments(argc, argv);

	for (auto& argument : arguments)
	{
		if (argument == "--help")
		{
			printHelp();
			return 0;
		}
	}

	auto operation = getOperation(arguments);
	

	try
	{
		printOperationInfo(operation);
		switch (operation.type)
		{
		case OPERATION_NONE:
			throw std::runtime_error("No operation is specified");
			break;
		case OPERATION_PACK:
			packArchive(operation);
			break;
		case OPERATION_UNPACK:
			unpackArchive(operation);
			break;
		}
	}
	catch (std::runtime_error e)
	{
		printf("Error: %s\n",e.what());
		return -1;
	}
	catch (FAC::Exception e)
	{
		printf("Library Error: %s\n", e.what());
		return -1;
	}


	return 0;
}

void printOperationInfo(Operation operation)
{
	switch (operation.type)
	{
	case OPERATION_NONE:
		printf("Operation Type: NONE\n");
		break;
	case OPERATION_PACK:
		printf("Operation Type: PACK\n");
		break;
	case OPERATION_UNPACK:
		printf("Operation Type: UNPACK\n");
		break;
	}
	printf("Input: %s\n", operation.input.c_str());
	printf("Output: %s\n\n\n", operation.output.c_str());
}

void unpackArchive(Operation operation)
{
	if (fs::is_directory(operation.input))
	{
		throw std::runtime_error("The path is a directory");
	}
	if (fs::exists(operation.input))
	{
		throw std::runtime_error("The file does not exist");
	}
	auto header = FAC::loadArchiveHeaderFromFile(operation.input);

	for (auto &file : header.files)
	{
		FAC::loadFileFromArchive(header, file.name);
	}

	for (auto& file : header.files)
	{
		std::string output = file.name;


		operation.output.push_back('/');

		output.insert(output.begin(), operation.output.begin(), operation.output.end());


		fs::path dirPath = fs::path(output).parent_path();

		if (!fs::exists(dirPath)) {
			fs::create_directories(dirPath);
		}

		FAC::saveFile(file.data, output);
	}

	return;
}

void packArchive(Operation operation)
{
	FAC::ArchiveHeader header;

	if (fs::is_directory(operation.input))
	{
		for (const auto& entry : fs::recursive_directory_iterator(operation.input)) {
			if (fs::is_regular_file(entry)) {
				FAC::File file;
				file.name = entry.path().string();
				header.files.push_back(file);
			}
		}
	}
	else throw std::runtime_error("The input path is not a directory");

	if (fs::is_directory(operation.output))
	{
		throw std::runtime_error("The output path is a directory");
	}

	if (!fs::exists(fs::path(operation.output).parent_path()))
	{
		throw std::runtime_error("The parent directory of the output path does not exist");
	}

	for (auto& file : header.files)
	{
		file.data = FAC::loadFile(file.name);
		file.name = fs::relative(file.name, operation.input).string();
		for (auto& character : file.name)
		{
			if (character == '\\')
			{
				character = '/';
			}
		}
	}

	auto data = FAC::createArchive(header);

	FAC::saveFile(data, operation.output);

	return;
}

Operation getOperation(std::vector<std::string> &arguments)
{
	Operation operation;

	for (int i = 0; i < arguments.size(); i++)
	{
		if (arguments[i] == "--output")
		{
			i++;
			operation.output = arguments[i];
		}
		else if (arguments[i] == "--pack")
		{
			i++;
			operation.input = arguments[i];
			operation.type = OPERATION_PACK;
		}
		else if (arguments[i] == "--unpack")
		{
			i++;
			operation.input = arguments[i];
			operation.type = OPERATION_UNPACK;
		}
		else if (arguments[i] == "--compress")
		{
			operation.compressed = true;
		}
		else if (arguments[i] == "--encrypt")
		{
			operation.encrypted = true;
		}
	}

	return operation;
}

std::vector<std::string> parseArguments(int argc, char** argv)
{
	std::vector<std::string> arguments;
	arguments.resize(argc);

	int index = 0;
	for (auto& argument : arguments)
	{
		argument = argv[index];
		index++;
	}
	return arguments;
}

void printHelp()
{
	printf("Parameters\n");
	printf("\t--pack - pick directory to pack\n");
	printf("\t--unpack - pick .FAC file to unpack\n");
	printf("\t--output - pick output\n");
}