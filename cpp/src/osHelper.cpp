#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

#include <osHelper.hpp>

void openFileDialog(std::string &result) {
    std::string out;
    char buffer[128];

    FILE* pipe = popen("zenity --file-selection", "r");
    if (!pipe) {
        throw std::runtime_error("popen() failed !");
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        out += buffer;
    }

    if (int returnCode = pclose(pipe); returnCode != 0) {
        throw std::runtime_error("Command failed with return code: " + std::to_string(returnCode));
    }

    result = out;
}
void open_text_dialog(std::string &result) {
    std::string out;
    char buffer[128];

    FILE* pipe = popen("zenity --entry --title=\"\" --text=\"\"", "r");
    if (!pipe) {
        throw std::runtime_error("popen() failed !");
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        out += buffer;
    }

    if (int returnCode = pclose(pipe); returnCode != 0) {
        throw std::runtime_error("Command failed with return code: " + std::to_string(returnCode));
    }

    result = out;
}