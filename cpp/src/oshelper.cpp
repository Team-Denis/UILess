#include <oshelper.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

std::string open_file_dialog() {
    std::string result;
    char buffer[128];

    FILE* pipe = popen("zenity --file-selection", "r");
    if (!pipe) {
        throw std::runtime_error("popen() failed !");
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    int returnCode = pclose(pipe);
    if (returnCode != 0) {
        throw std::runtime_error("Command failed with return code: " + std::to_string(returnCode));
    }

    return result;
}