#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <optional>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;



typedef enum{
    _return,
    int_lit,
    semi
}TokenType;



typedef struct{
    TokenType type;
    std::optional<std::string> value;
} Token;


std::vector<Token> Tokenize(const std::string& str){
    std::vector<Token> tokens;
    std::string buffer;
    for(int i = 0 ; i < str.length(); i++){
        char c = str.at(i);
        if (std::isalpha(c)){
            buffer.push_back(c);
            i++;
            while (std::isalnum(str.at(i))){
                buffer.push_back(str.at(i));
                i++;
            }
            i--;
            if (buffer == "return"){
                tokens.push_back({
                    .type = _return // no values since its not a integer literal
                });
                buffer.clear();
                continue;
            } else {
                fprintf(stdout, "Undefined token");
                exit(EXIT_FAILURE);
            }
        }
        else if(std::isdigit(str.at(i))){
            buffer.push_back(str.at(i));
            i++;
            while(std::isdigit(str.at(i))){
                buffer.push_back(str.at(i));
                i++;
            }
            i--;
            tokens.push_back({
                .type = int_lit,
                .value = buffer
            });
            buffer.clear();
        }
        else if (c == ';'){
            tokens.push_back({
                .type = semi
            });
        }

        else if (std::isspace(str.at(i))){
            continue;
        } else {
             fprintf(stdout, "Undefined token");
            exit(EXIT_FAILURE);
        }
    }
    return tokens;
};


std::string tokens_to_asm(const std::vector<Token> tokens){
    if (tokens.empty() && tokens.size() < 1){
        exit(EXIT_FAILURE);
    }
    std::stringstream output;
    output << "global _start\n_start:\n";
    for(int i = 0 ; i < tokens.size(); i++){
        const Token& token = tokens.at(i);
        if(token.type == TokenType::_return){
            if (i + 1 < tokens.size() && tokens.at(i+1).type == TokenType::int_lit){
                if (i+2 < tokens.size() && tokens.at(i+2).type == TokenType::semi){
                    output << "   mov rax, 60\n";
                    output << "   mov rdi, " << tokens.at(i+1).value.value() << "\n";
                    output << "   syscall";
                }
            }
        }
    }
    return output.str();
}


int main(int argc, char* argv[]){
    if (argc != 2){
        fprintf(stdout, "Incorrect usage, the correct usage is:\n");
        fprintf(stdout, "<filename>.kx\n");
        return EXIT_FAILURE;
    }

    char* buffer;

    FILE* file = fopen(argv[1], "rb");
    if(!file){
        fprintf(stderr, "Failed to open file\n");
        return EXIT_FAILURE;
    }

    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fseek(file, 0, SEEK_SET);
    buffer = (char*) malloc((length + 1) * sizeof(char)); 
    if (!buffer){
        fprintf(stderr, "Failed to allocate memory\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    size_t read = fread(buffer, sizeof(char), length, file);
    buffer[read] = '\0';
    fclose(file);

    
    std::vector<Token> tokens = Tokenize(buffer);
    std::cout << tokens_to_asm(tokens) << std::endl;

    {
        std::fstream file("out.asm", std::ios::out);
        file << tokens_to_asm(tokens);
    }
   

    system("nasm -felf64 out.asm");
    system("ld out.o -o out");

    free(buffer); 

    return EXIT_SUCCESS;
}