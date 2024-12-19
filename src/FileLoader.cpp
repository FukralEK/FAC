
#include <FAC/FAC.h>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

// TO DO: Error handling

void FAC::saveFile(std::vector<unsigned char> &data, std::string filename)
{
    std::ofstream file(filename, std::ios::binary);

    if (!file)
    {
        throw FAC::Exception(FAC::NO_PERMISSION, "Could not open the file");
    }

    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

std::vector<unsigned char> FAC::loadFile(std::string filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file)
    {
        throw FAC::Exception(FAC::NO_PERMISSION, "Could not open the file");
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> buffer(fileSize);

    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

    return buffer;
}

std::vector<unsigned char> FAC::loadFile(std::string filename, int start, int end)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file)
    {
        throw FAC::Exception(FAC::NO_PERMISSION, "Could not open the file");
    }

    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();

    file.seekg(start, std::ios::beg);

    std::streamsize numBytesToRead = end - start;

    if (numBytesToRead < fileSize)
    {
        throw FAC::Exception(FAC::WRONG_INDEX, "Out of bound");
    }

    std::vector<unsigned char> buffer(numBytesToRead);

    file.read(reinterpret_cast<char*>(buffer.data()), numBytesToRead);

    return buffer;
}

FAC_API uint64_t FAC::getFileSize(std::string filename)
{
    if (!fs::exists(filename))
    {
        throw FAC::Exception(FAC::FILE_NOT_FOUND, "The file not found");
        return -1;
    }
    if (fs::is_directory(filename))
    {
        throw FAC::Exception(FAC::PATH_IS_DIRECTORY, "The path is a directory");
        return -1;
    }
    std::ifstream file(filename, std::ios::binary | std::ios::ate); 
    if (file) {
        return file.tellg();
    }

    throw FAC::Exception(FAC::NO_PERMISSION, "No permission to the path");
    return -1;
}
