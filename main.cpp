#include <iostream>
#include <fstream>
#include <cctype>
#include <vector>
#include <cmath>
#include <string>

enum State {
    SPACE,
    SLASH,
    COMMENT,
    AINSTR,
    CINSTR_BEGIN,
    CINSTR_COMP,
    CINSTR_JMP
};

std::string registerNames = "AMD";
std::string jmpLetters = "JGTEQLNMP";
std::string compSymbols = "1+-!&|";

void writeInstruction(std::string outputFilename, std::string str) {
    printf("Writing instruction to file: %s\n", str.c_str());
    std::ofstream stream(outputFilename, std::ios_base::app);
    stream << str.c_str() << std::endl;
}

std::string decToBin(std::string str) {
    int inputNumber = std::stoi(str);
    std::string outputNumber(16, '0');
    while(inputNumber != 0) {
        int biggestPO2 = log(inputNumber) / log(2);
        outputNumber[15 - biggestPO2] = '1';
        int numberToSubstract = pow(2, biggestPO2);
        inputNumber -= numberToSubstract;
    }
    return outputNumber;
}

std::string getCInstruction(std::string dest, std::string comp, std::string jmp) {
    std::string output(16, '0');
    output.replace(0, 3, "111");
    if(dest == "M") {
        output.replace(10, 3, "001"); //
    }
    if(dest == "D") {
        output.replace(10, 3, "010"); //
    }
    if(dest == "MD") {
        output.replace(10, 3, "011"); //
    }
    if(dest == "A") {
        output.replace(10, 3, "100"); //
    }
    if(dest == "AM") {
        output.replace(10, 3, "101"); //
    }
    if(dest == "AD") {
        output.replace(10, 3, "110"); //
    }
    if(dest == "AMD") {
        output.replace(10, 3, "111"); //
    }
    if(comp == "0") {
        output.replace(4, 6, "101010"); //
    }
    if(comp == "1") {
        output.replace(4, 6, "111111"); //
    }
    if(comp == "-1") {
        output.replace(4, 6, "111010"); //
    }
    if(comp == "D") {
        output.replace(4, 6, "001100"); //
    }
    if(comp == "A" || comp == "M") {
        output.replace(4, 6, "110000"); //
        if(comp == "M") {
            output[3] = '1';
        }
    }
    if(comp == "!D") {
        output.replace(4, 6, "001101"); //
    }
    if(comp == "!A" || comp == "!M") {
        output.replace(4, 6, "110001"); //
        if(comp == "!M") {
            output[3] = '1';
        }
    }
    if(comp == "-D") {
        output.replace(4, 6, "001111"); //
    }
    if(comp == "-A" || comp == "-M") {
        output.replace(4, 6, "110011"); //
        if(comp == "-M") {
            output[3] = '1';
        }
    }
    if(comp == "D+1") {
        output.replace(4, 6, "011111"); //
    }
    if(comp == "A+1" || comp == "M+1") {
        output.replace(4, 6, "110111"); //
        if(comp == "M+1") {
            output[3] = '1';
        }
    }
    if(comp == "D-1") {
        output.replace(4, 6, "001110"); //
    }
    if(comp == "A-1" || comp == "M-1") {
        output.replace(4, 6, "110010"); //
        if(comp == "M-1") {
            output[3] = '1';
        }
    }
    if(comp == "D+A" || comp == "D+M") {
        output.replace(4, 6, "000010"); //
        if(comp == "D+M") {
            output[3] = '1';
        }
    }
    if(comp == "D-A" || comp == "D-M") {
        output.replace(4, 6, "010011"); //
        if(comp == "D-M") {
            output[3] = '1';
        }
    }
    if(comp == "A-D" || comp == "M-D") {
        output.replace(4, 6, "000111"); //
        if(comp == "M-D") {
            output[3] = '1';
        }
    }
    if(comp == "D&A" || comp == "D&M") {
        output.replace(4, 6, "000000"); //
        if(comp == "D&M") {
            output[3] = '1';
        }
    }
    if(comp == "D|A" || comp == "D|M") {
        output.replace(4, 6, "010101"); //
        if(comp == "D|M") {
            output[3] = '1';
        }
    }
    if(jmp == "JGT") {
        output.replace(13, 3, "001"); //
    }
    if(jmp == "JEQ") {
        output.replace(13, 3, "010"); //
    }
    if(jmp == "JGE") {
        output.replace(13, 3, "011"); //
    }
    if(jmp == "JLT") {
        output.replace(13, 3, "100"); //
    }
    if(jmp == "JNE") {
        output.replace(13, 3, "101"); //
    }
    if(jmp == "JLE") {
        output.replace(13, 3, "110"); //
    }
    if(jmp == "JMP") {
        output.replace(13, 3, "111"); //
    }
    return output;
}

void assemble(std::string inputFilename) {
    std::cout << "Assembling " + inputFilename << std::endl;
    std::fstream inputStream(inputFilename);
    if(inputStream.bad()) {
        std::cout << "Error" << std::endl;
    }
    std::string outputFilename = inputFilename.substr(0, inputFilename.rfind(".")) + ".hack";
    std::cout << "Output file: " + outputFilename << std::endl;
    std::ofstream clearFileContents(outputFilename, std::ios::out, std::ios::trunc);
    clearFileContents.close();
    int line_number = 0;
    State currentState = SPACE;
    char c;
    std::string ainstrBuffer;
    std::string cinstrBeginBuffer;
    std::string cinstrDestBuffer;
    std::string cinstrCompBuffer;
    std::string cinstrJmpBuffer;
    printf("\n");
    while(inputStream >> std::noskipws >> c) {
        printf("State: ");
        switch(currentState) {
            case SPACE:         printf("space");        break;
            case SLASH:         printf("slash");        break;
            case COMMENT:       printf("comment");      break;
            case AINSTR:        printf("ainstr");       break;
            case CINSTR_BEGIN:  printf("cinstr_begin"); break;
            case CINSTR_COMP:   printf("cinstr_comp");  break;
            case CINSTR_JMP:    printf("cinstr_jump");  break;
        }
        printf("\n");
        if(c == '\n') {
            printf("Read symbol \\n\n");
        } else {
            printf("Read symbol %c\n", c);
        }
        if(c == '/') {
            printf("Slash symbol found\n");
            if(currentState == SLASH) {
                printf("Comment found\n");
                currentState = COMMENT;
            } else if(currentState == SPACE) {
                currentState = SLASH;
            }
        }
        if(std::isspace(c)) {
            printf("Whitespace symbol found\n");
            if(currentState == AINSTR) {
                printf("Writing A instruction: %s\n", ainstrBuffer.c_str());
                writeInstruction(outputFilename, decToBin(ainstrBuffer));
                currentState = SPACE;
            } else if(currentState == CINSTR_JMP || currentState == CINSTR_COMP) {
                printf("Writing C instruction\n");
                writeInstruction(outputFilename, getCInstruction(cinstrDestBuffer, cinstrCompBuffer, cinstrJmpBuffer));
                currentState = SPACE;
            }
        }
        if(c == '\n') {
            if(currentState == COMMENT) {
                currentState = SPACE;
            }
        }
        if(c == '@') {
            printf("At symbol found\n");
            if(currentState == SPACE) {
                currentState = AINSTR;
                ainstrBuffer.clear();
            }
        }
        if(std::isdigit(c)) {
            printf("Digit found\n");
            if(currentState == AINSTR) {
                ainstrBuffer += c;
                printf("AinstrBuffer: %s\n", ainstrBuffer.c_str());
            }
        }
        if(registerNames.find(c) != std::string::npos || std::isdigit(c)) {
            printf("Register name or digit found\n");
            if(currentState == SPACE) {
                currentState = CINSTR_BEGIN;
                cinstrBeginBuffer.clear();
                cinstrDestBuffer.clear();
                cinstrCompBuffer.clear();
                cinstrJmpBuffer.clear();
                cinstrBeginBuffer += c;
            } else if(currentState == CINSTR_BEGIN) {
                cinstrBeginBuffer += c;
            } else if(currentState == CINSTR_COMP) {
                cinstrCompBuffer += c;
            }
        }
        if(compSymbols.find(c) != std::string::npos) {
            printf("Comp symbol found\n");
            if(currentState == CINSTR_COMP) {
                cinstrCompBuffer += c;
            } else if(currentState == CINSTR_BEGIN) {
                cinstrCompBuffer = cinstrBeginBuffer;
                cinstrCompBuffer += c;
                currentState = CINSTR_COMP;
            }
        }
        if(c == '=') {
            printf("Equals symbol found\n");
            if(currentState == CINSTR_BEGIN) {
                currentState = CINSTR_COMP;
                cinstrDestBuffer = cinstrBeginBuffer;
            }
        }
        if(c == ';') {
            printf("Semicolon symbol found\n");
            if(currentState == CINSTR_COMP) {
                currentState = CINSTR_JMP;
            }
            if(currentState == CINSTR_BEGIN) {
                currentState = CINSTR_JMP;
                cinstrCompBuffer = cinstrBeginBuffer;
            }
        }
        if(jmpLetters.find(c) != std::string::npos) {
            printf("Jmp letter found\n");
            if(currentState == CINSTR_JMP) {
                cinstrJmpBuffer += c;
            }
        }
        printf("\n");
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[]) {

    assemble(argv[1]);

    return 0;

}