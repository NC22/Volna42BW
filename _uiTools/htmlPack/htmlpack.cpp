#include <iostream>
#include <cstdio> 
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <regex>
#ifdef _WIN32
#include <windows.h>
#define SETUP_CONSOLE() SetConsoleOutputCP(CP_UTF8)
#else
#define SETUP_CONSOLE()
#endif


/* 
	Simple tool for remove comments from html, js, css files and prepare them to store in format optimized for Arduino Framework
	
	by nradiowave | part of Volna 42 project
*/

using namespace std;

string pregReplace(string input, string regExp, string replaceWith = "" ) {

    const std::regex re(regExp);

    // std::string out = input;
    // while (std::regex_search(out, re)) {
    //    out = std::regex_replace(out, re, "$&");
    //}

    return std::regex_replace(input, re, replaceWith);
}

string getExt(string input) {
    string result = "";
    int extStart = -1;

    for (int i = input.length()-1; i >= 0; i--) {
        if (input[i] == '.') {
            extStart = i+1;
            break;
        }
    }

    if (extStart == -1 || extStart >= input.length()) return "js";

    for (int i = extStart; i < input.length(); i++) {
        result += input[i];
    }

    return result;
}

string getSaveToPath(string input) {
    string result = "";
    for (int i = 0; i < input.length(); i++) {
        if (input[i] == '.' && i > 1) {
            return result;
        }

        result += input[i];
    }

    return result;
}

string getFileName(string input) {
    string result = "";
    int lastD = -1;
    for (int i = input.length() - 1; i >= 0; i--) {
        if (input[i] == '/' || input[i] == '\\') {
            lastD = i;
            break;
        }
    }

    for (int i = lastD + 1; i < input.length(); i++) {
        result += input[i];
    }

    return result;
}

int main(int argc, char *argv[])
{
    SETUP_CONSOLE();
    
    string removeCommentsRegExp = "\\/\\/[^\n\r]+?(?:\\*\\)|[\n\r])";
    // string removeCommentsRegExp2 = "/\\*.*?\\*/";
    string removeCommentsRegExp2 = "/\\*[\\s\\S]*?\\*/";
    string removeMoveThanOne = "[ ]{2,}";
    string removeNewLine = "[\r\n]+";

    string inputFile = "";
    string saveTo = "";
    string fileName = "";

    string exe = argc >= 1 ? argv[0] : "minify.exe";
    bool createHFile = true;

    if (argc >= 2) {

        inputFile = argv[1];

        if (argc >= 3) {

            saveTo = argv[2];
            saveTo = getSaveToPath(saveTo);

            fileName = getFileName(saveTo);

        } else {
            saveTo = "out";
        }

        //if (argc >= 4) {
        //    createHFile = argv[3] == "1" ? true : false;
        //}

    } else {
        
        std::cout << "Please set input html file and save to args " + exe + " [input file] [save to dir/file]" << std::endl;
        return 1;
    }

    string ext = getExt(inputFile);
    std::cout << "Input file : " + inputFile << std::endl;
    std::cout << "Save to : " + saveTo + "." + ext + " | FILE NAME Alias : " + fileName << std::endl;

    if (ext.length() <= 0) {
       std::cout << "Please specify input file extension" << std::endl; 
    }

    // char buffer[128];

    // std::cout << "before quotes : " << std::endl;
    // std::cout << tt << std::endl;

    // std::cout << "after quotes : " << std::endl;
    // std::cout << "String webdata = " + ss.str() << std::endl;
    
    string line = "";
    ifstream input(inputFile);
    string resultString = "";

    if (input.is_open())
    {

        while ( std::getline (input, line) ) {
            resultString += line + "\n";
        }

        input.close();

    } else {
        std::cout << "Cant find file : " + inputFile << std::endl;
        return 1;
    }

    resultString = pregReplace(resultString, removeCommentsRegExp);
    resultString = pregReplace(resultString, removeCommentsRegExp2);
    resultString = pregReplace(resultString, removeMoveThanOne, " ");
    resultString = pregReplace(resultString, removeNewLine);

    ofstream result;
    
    result.open (saveTo + "." + ext);
    result << resultString;
    result.close();  

    if (createHFile) {

        std::cout << "create .H File : " + saveTo << std::endl;

        std::stringstream ss;
        ss << std::quoted(resultString);

        string resultStr = ss.str();
            
        result.open (saveTo + ".h");
        
        // result << "const webData  webdata_" + fileName + "_" + ext + " PROGMEM = {" << endl;
        // result << "     " + resultStr + "," << endl;
        // result << "     " + std::to_string(resultStr.length()) << endl;
        // result << "};" << endl;
        
        
        // result << "const char webdata_" + fileName + "_" + ext + "[" + std::to_string(resultString.length()+1) + "] PROGMEM = " + resultStr + ";"  + "\r\n";

        result << "const char webdata_" + fileName + "_" + ext + "[] PROGMEM = " + resultStr + ";"  + "\r\n";
        result << "const unsigned int webdataSize_" + fileName + "_" + ext + " PROGMEM = " + std::to_string(resultString.length()) + ";";
        result.close();   
    }
}