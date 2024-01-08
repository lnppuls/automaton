#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <unordered_set>

enum class TYPE {
    VARIABLE,
    KEYWORDS,
    SYMBOL,
    VALUE,
    NUM,
};

static std::unordered_set<std::string> symbols = {
        "+", "-", "*", "/", "%", "++", "--", "=", "==", "!=", ">", "<", ">=", "<=",
        ",", ";", ":", ".", "\"", "'", "(", ")", "[", "]", "{", "}",
        "&&", "||", "!", "&", "|", "^", "~", "<<", ">>",
        "?", ":", ".", "->",
        "::",
        "//", "/*", "*/", "#", "&", "*"
};

static std::unordered_set<std::string> keywords = {
    "auto", "break", "case", "char", "const", "continue", "default", "do", "double",
    "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register",
    "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef",
    "union", "unsigned", "void", "volatile", "while"
};


class word_unit{
public:
    word_unit(std::string name,TYPE group,uint32_t id) : name_(name) , group_(group) , id_(id) { }
    std::string name_;
    TYPE group_;
    uint32_t id_;
};

class lex_analysis
{
public:
    enum class State {
        NORMAL,
        WORD,
        INANNOTATION,
        SYMBOL,
        NUMBER,
        INVARIABLE,
    };
    lex_analysis() { buff_size_ = 0; }
    lex_analysis(std::ifstream &infilestream,uint32_t buff_size) : buff_size_(buff_size) { file_string_ = std::move(infilestream); }
    ~lex_analysis() = default;
    auto init() -> bool;
    auto run() -> std::vector<word_unit>;
private:
    uint32_t global_offset_;
    uint32_t local_offset_;
    std::ifstream file_string_;
    uint32_t buff_size_;
    std::unordered_map<std::string,uint8_t> key_values;
};


auto lex_analysis::init() -> bool
{
    std::ifstream init_file("./src/profile.txt");
    if (init_file.is_open()) {
        std::string line;
        std::string name;
        uint32_t id = 0;
        std::getline(init_file, line);
        line.erase(std::remove_if(line.begin(), line.end(), [](char c) { return std::isspace(c); }), line.end());
        std::istringstream s_line(line);
        while (std::getline(s_line,name,','))
        {
            key_values[name] = id++;
        }
        init_file.close(); 
    } else {
        std::cout << "can not open profile" << std::endl;
    }
    return false;
}

auto lex_analysis::run() -> std::vector<word_unit>
{
    char mchar = '\0';
    std::vector<word_unit> out;
    State status{State::NORMAL};
    std::string now_string;
    while (file_string_.get(mchar)) {
    switch (status) {
        case State::NORMAL: {
            if (mchar != '\n' && mchar != ' ' && mchar != (char)9){
                status = State::WORD;
                file_string_.putback(mchar);
                break;
            }
            break;
        }
        case State::WORD: {
            if(mchar == ' ' || mchar == '\n' || mchar == (char)9){
                status = State::NORMAL;
                break;
            }
            char new_char;
            file_string_.get(new_char);
            if(mchar == '/' && new_char == '/'){
                status = State::INANNOTATION;
                break;
            } else {
                file_string_.putback(new_char);
            }
            if(isalpha(mchar)){   
                file_string_.putback(mchar);
                status = State::INVARIABLE;
                break;
            } else if(isdigit(mchar)){
                file_string_.putback(mchar);
                status = State::NUMBER;
                break;
            }
            else{
                file_string_.putback(mchar);
                status = State::SYMBOL;
                break;
            }
            break;
        }
        case State::INANNOTATION:{
            if(mchar == '\n'){
                status = State::NORMAL;
            }
            break;
        }
        case State::INVARIABLE: {
            if(isalnum(mchar) || mchar == '_' || mchar == '.'){
                now_string.push_back(mchar);
                break;
            }
            file_string_.putback(mchar);
            status = State::NORMAL;
            if(keywords.find(now_string) == keywords.end()){
                out.emplace_back(now_string,TYPE::VARIABLE,-1);
                // std::cout << now_string << std::endl;
            } else {
                out.emplace_back(now_string,TYPE::KEYWORDS,-1);
            }
            now_string.clear();
            status = State::NORMAL;
            break;
        }
        case State::SYMBOL: {
            std::string msymbol;
            msymbol.push_back(mchar);
            out.emplace_back(msymbol,TYPE::SYMBOL,-1);
            status = State::NORMAL;
            break;
        }
        case State::NUMBER: {
            if(!isdigit(mchar)){
                file_string_.putback(mchar);
                out.emplace_back(now_string,TYPE::NUM,-1);
                // std::cout << now_string << std::endl;
                now_string.clear();
                status = State::NORMAL;
                break;
            }
            now_string.push_back(mchar);
        }
        default:
            break;
    }
    }
    return out;
}

int main(){
    std::ifstream infilestream;
    infilestream.open("./test_cases/main.c");
    lex_analysis ana{infilestream,100};
    ana.init();
    auto res = ana.run();
    int num = 0;
    for(auto &uit : res){
        std::cout << uit.name_ << ' ';
        if(++num % 10 == 0){
            std::cout << std::endl;
        } 
    }
}
