
#include <FAC/FAC.h>
#include <fstream>

// TO DO: Port to C APIs
// TO DO: Error handling

void FAC::saveFile(std::vector<unsigned char> &data, std::string filename)
{
    std::ofstream file(filename, std::ios::binary);

    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

std::vector<unsigned char> FAC::loadFile(std::string filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> buffer(fileSize);

    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

    return buffer;
}

std::vector<unsigned char> FAC::loadFile(std::string filename, int start, int end)
{
    std::ifstream file(filename, std::ios::binary);

    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();

    file.seekg(start, std::ios::beg);

    std::streamsize numBytesToRead = end - start;

    std::vector<unsigned char> buffer(numBytesToRead);

    file.read(reinterpret_cast<char*>(buffer.data()), numBytesToRead);

    return buffer;
}

FAC_API uint64_t FAC::getFileSize(std::string filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate); 
    if (file) {
        return file.tellg();
    }
    return -1; 
}
