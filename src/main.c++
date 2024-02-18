#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

#include "tokenization.hpp"

using namespace std;

std::string tokens_to_asm(const std::vector<Token> tokens) {
  if (tokens.empty() && tokens.size() < 1) {
    exit(EXIT_FAILURE);
  }
  std::stringstream output;
  output << "global _start\n_start:\n";
  for (int i = 0; i < tokens.size(); i++) {
    const Token& token = tokens.at(i);
    if (token.type == TokenType::_exit) {
      if (i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::int_lit) {
        if (i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::semi) {
          output << "   mov rax, 60\n";
          output << "   mov rdi, " << tokens.at(i + 1).value.value() << "\n";
          output << "   syscall";
        }
      }
    }
  }
  return output.str();
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stdout, "Incorrect usage, the correct usage is:\n");
    fprintf(stdout, "<filename>.kx\n");
    return EXIT_FAILURE;
  }
  std::string buffer;

  {
    std::stringstream contents_stream;
    std::fstream file(argv[1], std::ios::in);
    contents_stream << file.rdbuf();
    buffer = contents_stream.str();
  }

  Tokenization tokenization = Tokenization(buffer);
  std::vector<Token> tokens = tokenization.tokenize();

  {
    std::fstream file("out.asm", std::ios::out);
    file << tokens_to_asm(tokens);
  }

  system("nasm -felf64 out.asm");
  system("ld out.o -o out");

  return EXIT_SUCCESS;
}