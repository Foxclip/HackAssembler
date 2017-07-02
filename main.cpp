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
    CINSTR,
    CINSTR_BEGIN,
    CINSTR_COMP,
    CINSTR_JMP,
    LABEL
};

void debugPrint(std::string str) {
    //std::cout << str;
}

void debugPrintLine(std::string str) {
    debugPrint(str + '\n');
}

std::string registerNames = "AMD";
std::string jmpLetters = "JGTEQLNMP";
std::string compSymbols = "1+-!&|";

void writeInstruction(std::string outputFilename, std::string str) {
    debugPrintLine("Writing instruction to file: " + str);
    std::ofstream stream(outputFilename, std::ios_base::app);
    stream << str.c_str() << std::endl;
}

std::string decToBin(std::string str) {
    int inputNumber = std::stoi(str);
    std::string outputNumber(16, '0');
    while(inputNumber != 0) {
        int biggestPO2 = (int)(log(inputNumber) / log(2));
        outputNumber[15 - biggestPO2] = '1';
        int numberToSubstract = (int)pow(2, biggestPO2);
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

struct SymbolTableEntry {
    std::string name;
    int address;
};

int findInSymbolTable(std::vector<SymbolTableEntry> symbolTable, std::string labelName) {
    for(int i = 0; i < symbolTable.size(); i++) {
        if(symbolTable[i].name == labelName) {
            return i;
        }
    }
    return -1;
}

std::vector<SymbolTableEntry> buildSymbolTable(std::string inputFilename) {
    std::cout << "Building symbol table " + inputFilename << std::endl;
    std::cout << std::endl;
    std::fstream inputStream(inputFilename);
    if(inputStream.bad()) {
        std::cout << "Error" << std::endl;
    }
    std::vector<SymbolTableEntry> symbolTable;
    symbolTable.push_back({ "SP", 0 });
    symbolTable.push_back({ "LCL", 1 });
    symbolTable.push_back({ "ARG", 2 });
    symbolTable.push_back({ "THIS", 3 });
    symbolTable.push_back({ "THAT", 4 });
    symbolTable.push_back({ "R0", 0 });
    symbolTable.push_back({ "R1", 1 });
    symbolTable.push_back({ "R2", 2 });
    symbolTable.push_back({ "R3", 3 });
    symbolTable.push_back({ "R4", 4 });
    symbolTable.push_back({ "R5", 5 });
    symbolTable.push_back({ "R6", 6 });
    symbolTable.push_back({ "R7", 7 });
    symbolTable.push_back({ "R8", 8 });
    symbolTable.push_back({ "R9", 9 });
    symbolTable.push_back({ "R10", 10 });
    symbolTable.push_back({ "R11", 11 });
    symbolTable.push_back({ "R12", 12 });
    symbolTable.push_back({ "R13", 13 });
    symbolTable.push_back({ "R14", 14 });
    symbolTable.push_back({ "R15", 15 });
    symbolTable.push_back({ "SCREEN", 16384 });
    symbolTable.push_back({ "KBD", 24576 });
    std::string labelBuffer;
    State currentState = SPACE;
    char c;
    int lineNumber = -1;
    while(inputStream >> std::noskipws >> c) {
        switch(currentState) {
            case SPACE:   debugPrintLine("State: space");   break;
            case SLASH:   debugPrintLine("State: slash");   break;
            case COMMENT: debugPrintLine("State: comment"); break;
            case AINSTR:  debugPrintLine("State: ainstr");  break;
            case CINSTR:  debugPrintLine("State: cinstr");  break;
            case LABEL:   debugPrintLine("State: label");   break;
        }
        if(c == '\n') {
            debugPrintLine("Read symbol \\n");
        } else {
            debugPrintLine(std::string("Read symbol ") + c);
        }
        if(c == '/') {
            debugPrintLine("Slash symbol found");
            if(currentState == SLASH) {
                debugPrintLine("Comment found");
                currentState = COMMENT;
            } else if(currentState == SPACE) {
                currentState = SLASH;
            }
        }
        if(std::isspace(c)) {
            debugPrintLine("Whitespace symbol found");
            if(currentState == AINSTR) {
                currentState = SPACE;
            } else if(currentState == CINSTR) {
                currentState = SPACE;
            }
        }
        if(c == '\n') {
            if(currentState == COMMENT) {
                currentState = SPACE;
            }
        }
        if(c == '@') {
            debugPrintLine("At symbol found");
            if(currentState == SPACE) {
                currentState = AINSTR;
                lineNumber++;
                debugPrintLine("Line number is " + std::to_string(lineNumber));
            }
        }
        if(c == '(') {
            debugPrintLine("Opening parenthesis found");
            if(currentState == SPACE) {
                currentState = LABEL;
                labelBuffer.clear();
            }
        }
        if(std::isalnum(c) || c == '_' || c == '.' || c == '$') {
            debugPrintLine("Label symbol found");
            if(currentState == LABEL) {
                labelBuffer += c;
                debugPrintLine("labelBuffer: " + labelBuffer);
            }
        }
        if(c == ')') {
            debugPrintLine("Closing parenthesis found");
            if(currentState == LABEL) {
                currentState = SPACE;
                if(findInSymbolTable(symbolTable, labelBuffer) == -1) {
                    debugPrintLine("Adding symbol to symbol table: " + labelBuffer);
                    symbolTable.push_back({ labelBuffer, lineNumber + 1 });
                    debugPrintLine("Symbol table size: " + std::to_string(symbolTable.size()));
                }
            }
        }
        if(registerNames.find(c) != std::string::npos || std::isdigit(c)) {
            debugPrintLine("Register name or digit found");
            if(currentState == SPACE) {
                currentState = CINSTR;
                lineNumber++;
                debugPrintLine("Line number is " + std::to_string(lineNumber));
            }
        }
        debugPrintLine("");
    }

    debugPrintLine("Symbol table size: " + std::to_string(symbolTable.size()));

    //for(SymbolTableEntry entry : symbolTable) {
    //    //debugPrint(std::to_string(entry.lineNumber) + ": " + entry.name);
    //    std::cout << entry.address << ": " << entry.name << std::endl;
    //}

    return symbolTable;

}

bool notANumber(std::string str) {
    for(char c : str) {
        if(!std::isdigit(c)) {
            return true;
        }
    }
    return false;
}

void assemble(std::string inputFilename, std::vector<SymbolTableEntry>& symbolTable) {
    std::cout << "Assembling " + inputFilename << std::endl;
    std::fstream inputStream(inputFilename);
    if(inputStream.bad()) {
        std::cout << "Error" << std::endl;
    }
    std::string outputFilename = inputFilename.substr(0, inputFilename.rfind(".")) + ".hack";
    std::cout << "Output file: " + outputFilename << std::endl;
    std::ofstream clearFileContents(outputFilename, std::ios::trunc);
    clearFileContents.close();
    int line_number = 0;
    int variableAddress = 16;
    State currentState = SPACE;
    char c;
    std::string ainstrBuffer;
    std::string cinstrBeginBuffer;
    std::string cinstrDestBuffer;
    std::string cinstrCompBuffer;
    std::string cinstrJmpBuffer;
    debugPrintLine("\n");
    while(inputStream >> std::noskipws >> c) {
        debugPrint("State: ");
        switch(currentState) {
            case SPACE:         debugPrintLine("space");        break;
            case SLASH:         debugPrintLine("slash");        break;
            case COMMENT:       debugPrintLine("comment");      break;
            case AINSTR:        debugPrintLine("ainstr");       break;
            case CINSTR_BEGIN:  debugPrintLine("cinstr_begin"); break;
            case CINSTR_COMP:   debugPrintLine("cinstr_comp");  break;
            case CINSTR_JMP:    debugPrintLine("cinstr_jump");  break;
            case LABEL:         debugPrintLine("State: label"); break;
        }
        if(c == '\n') {
            debugPrintLine("Read symbol \\n");
        } else {
            debugPrintLine(std::string("Read symbol ") + c);
        }
        if(c == '/') {
            debugPrintLine("Slash symbol found");
            if(currentState == SLASH) {
                debugPrintLine("Comment found");
                currentState = COMMENT;
            } else if(currentState == SPACE) {
                currentState = SLASH;
            }
        }
        if(std::isspace(c)) {
            debugPrintLine("Whitespace symbol found");
            if(currentState == AINSTR) {
                int symbolIndex = findInSymbolTable(symbolTable, ainstrBuffer);
                if(symbolIndex != -1) {
                    ainstrBuffer = std::to_string(symbolTable[symbolIndex].address);
                    debugPrintLine("Symbol found: " + std::to_string(symbolTable[symbolIndex].address) + " " + symbolTable[symbolIndex].name);
                } else if(notANumber(ainstrBuffer)) {
                    debugPrintLine("Variable found: " + std::to_string(variableAddress) + " " + ainstrBuffer);
                    symbolTable.push_back({ ainstrBuffer, variableAddress });
                    ainstrBuffer = std::to_string(variableAddress);
                    variableAddress++;
                }
                debugPrintLine("Writing A instruction: " + ainstrBuffer);
                writeInstruction(outputFilename, decToBin(ainstrBuffer));
                currentState = SPACE;
            } else if(currentState == CINSTR_JMP || currentState == CINSTR_COMP) {
                debugPrintLine("Writing C instruction");
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
            debugPrintLine("At symbol found");
            if(currentState == SPACE) {
                currentState = AINSTR;
                ainstrBuffer.clear();
            }
        }
        if(c == '(') {
            debugPrintLine("Opening parenthesis found");
            if(currentState == SPACE) {
                currentState = LABEL;
            }
        }
        if(c == ')') {
            debugPrintLine("Closing parenthesis found");
            if(currentState == LABEL) {
                currentState = SPACE;
            }
        }
        if(std::isalnum(c) || c == '_' || c == '.' || c == '$') {
            debugPrintLine("Address symbol found");
            if(currentState == AINSTR) {
                ainstrBuffer += c;
                debugPrintLine("ainstrBuffer: " + ainstrBuffer);
            }
        }
        if(registerNames.find(c) != std::string::npos || c == '0') {
            debugPrintLine("Register name or zero found");
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
            debugPrintLine("Comp symbol found");
            if(currentState == CINSTR_COMP) {
                cinstrCompBuffer += c;
            } else if(currentState == CINSTR_BEGIN) {
                cinstrCompBuffer = cinstrBeginBuffer;
                cinstrCompBuffer += c;
                currentState = CINSTR_COMP;
            }
        }
        if(c == '=') {
            debugPrintLine("Equals symbol found");
            if(currentState == CINSTR_BEGIN) {
                currentState = CINSTR_COMP;
                cinstrDestBuffer = cinstrBeginBuffer;
            }
        }
        if(c == ';') {
            debugPrintLine("Semicolon symbol found");
            if(currentState == CINSTR_COMP) {
                currentState = CINSTR_JMP;
            }
            if(currentState == CINSTR_BEGIN) {
                currentState = CINSTR_JMP;
                cinstrCompBuffer = cinstrBeginBuffer;
            }
        }
        if(jmpLetters.find(c) != std::string::npos) {
            debugPrintLine("Jmp letter found");
            if(currentState == CINSTR_JMP) {
                cinstrJmpBuffer += c;
            }
        }
        debugPrintLine("");
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[]) {

    std::vector<SymbolTableEntry> symbolTable = buildSymbolTable(argv[1]);
    assemble(argv[1], symbolTable);

    return 0;

}