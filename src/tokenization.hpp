#pragma once
#include <optional>
#include <string>
#include <vector>

enum class TokenType { _exit, int_lit, semi };

typedef struct {
  TokenType type;
  std::optional<std::string> value;
} Token;

class Tokenization {
 public:
  inline Tokenization(std::string src) { this->source = src; };

  std::vector<Token> tokenize() {
    std::vector<Token> tokens;
    std::string buffer;
    while (peak().has_value()) {
      if (std::isalpha(peak().value())) {
        buffer.push_back(consume());
        while (peak().has_value() && isalnum(peak().value())) {
          buffer.push_back(consume());
        }
        if (buffer == "exit") {
          tokens.push_back({.type = TokenType::_exit});
          buffer.clear();
          continue;
        } else {
          fprintf(stdout, "Undefined token1");
          exit(EXIT_FAILURE);
        }
      } else if (std::isdigit(peak().value())) {
        buffer.push_back(consume());
        while (peak().has_value() && std::isdigit(peak().value())) {
          buffer.push_back(consume());
        }
        tokens.push_back({.type = TokenType::int_lit, .value = buffer});
        buffer.clear();
        continue;
      } else if (peak().has_value() && peak().value() == ';') {
        consume();
        tokens.push_back({.type = TokenType::semi});
        continue;
      } else if (std::isspace(peak().value())) {
        consume();
        continue;
      } else {
        fprintf(stdout, "Undefined token");
        exit(EXIT_FAILURE);
      }
    }
    return tokens;
  }

 private:
  std::optional<char> peak(size_t ahead = 0) {
    if (m_indx + ahead >= source.length())
      return {};
    else {
      return source.at(m_indx + ahead);
    }
  }

  char consume() { return source.at(m_indx++); }

  std::string source;
  size_t m_indx = 0;  // index of the current character we are reading
};
