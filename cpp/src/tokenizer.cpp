#include <sstream>
#include <iostream>

#include "tokenizer.hpp"

std::vector<std::string> Tokenizer::tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::string current_token;
    bool in_quotes = false;
    char quote_char = '\0'; // To track which quote is used

    for (size_t i = 0; i < input.length(); ++i) {
        char c = input[i];

        if (in_quotes) {
            if (c == quote_char) {
                // End of quoted token
                in_quotes = false;
                tokens.push_back(current_token);
                current_token.clear();
            }
            else {
                // Inside quotes, add character to current token
                current_token += c;
            }
        }
        else {
            if (std::isspace(c)) {
                if (!current_token.empty()) {
                    tokens.push_back(current_token);
                    current_token.clear();
                }
                // Ignore multiple spaces
            }
            else if (c == '"' || c == '\'') {
                if (!current_token.empty()) {
                    // If there's a token before the quote, push it
                    tokens.push_back(current_token);
                    current_token.clear();
                }
                // Start of quoted token
                in_quotes = true;
                quote_char = c;
            }
            else if (c == '|') {
                if (!current_token.empty()) {
                    tokens.push_back(current_token);
                    current_token.clear();
                }
                tokens.emplace_back("|");
            }
            else {
                // Regular character, add to current token
                current_token += c;
            }
        }
    }

    // Add the last token if exists
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }

    return tokens;
}