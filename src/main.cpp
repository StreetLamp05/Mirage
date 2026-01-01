#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Incorrect usage. Usage is..." << std::endl;
        std::cerr << "MirageC <input.mir>" << argv[0] << " <file>" << std::endl;
        return EXIT_FAILURE;
    }
    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Unable to open file: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    std:: string word;

    while (inputFile >> word)
    {
        std::cout << word << std::endl;
    }

    inputFile.close();
    return EXIT_SUCCESS;
}