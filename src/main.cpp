#include <stdio.h>
#include <FAC/FAC.h>
#include <filesystem>

namespace fs = std::filesystem;

#define OPERATION_NONE 0
#define OPERATION_UNPACK 1
#define OPERATION_PACK 2

struct Operation
{
	char type = OPERATION_NONE;
	std::string output;
	std::string input;
	bool recursive = false;
	bool compressed = false;
	bool encrypted = false;
};

std::vector<std::string> parseArguments(int argc, char** argv);
Operation getOperation(std::vector<std::string> &arguments);
void packArchive(Operation operation);
void unpackArchive(Operation operation);

int main(int argc, char** argv)
{
	auto arguments = parseArguments(argc, argv);

	auto operation = getOperation(arguments);

	switch (operation.type)
	{
	case OPERATION_NONE:
		printf("No operation is done\n");
		break;
	case OPERATION_PACK:
		printf("Packing %s to %s\n", operation.input.c_str(), operation.output.c_str());
		packArchive(operation);
		break;
	case OPERATION_UNPACK:
		printf("Unpacking %s to %s\n", operation.input.c_str(), operation.output.c_str());
		unpackArchive(operation);
		break;
	}

	return 0;
}

void unpackArchive(Operation operation)
{
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
	bool isDirectory = fs::is_directory(operation.input);
	
	FAC::ArchiveHeader header;

	if (isDirectory)
	{
		for (const auto& entry : fs::recursive_directory_iterator(operation.input)) {
			if (fs::is_regular_file(entry)) {
				FAC::File file;
				file.name = entry.path().string();
				header.files.push_back(file);
			}
		}
	}
	else {
		FAC::File file;
		file.name = operation.input;
		header.files.push_back(file);
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
		if (arguments[i] == "-r")
		{
			operation.recursive = true;
		}
		else if (arguments[i] == "-o")
		{
			i++;
			operation.output = arguments[i];
		}
		else if (arguments[i] == "-pack")
		{
			i++;
			operation.input = arguments[i];
			operation.type = OPERATION_PACK;
		}
		else if (arguments[i] == "-unpack")
		{
			i++;
			operation.input = arguments[i];
			operation.type = OPERATION_UNPACK;
		}
		else if (arguments[i] == "-c")
		{
			operation.compressed = true;
		}
		else if (arguments[i] == "-e")
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

