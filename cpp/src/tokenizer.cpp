#include "tokenizer.hpp"
#include <sstream>

std::vector<std::string> Tokenizer::tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string token;
    bool in_quotes = false;
    std::string current_token;

    while (stream >> std::ws) { // Skip leading whitespace
        char c = stream.peek();

        if (c == '"') {
            // Handle quoted token
            stream.get(); // Consume the quote
            std::getline(stream, token, '"');
            tokens.push_back(token);
        } else {
            // Handle unquoted token
            stream >> token;
            tokens.push_back(token);
        }
    }

    return tokens;
}