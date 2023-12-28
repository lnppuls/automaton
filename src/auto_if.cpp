#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class automaton
{
public:
    enum class State {
        Normal,
        InsideIfCondition,
        JudgeIf,
        JudgePattern,
    };
    automaton(std::string pattern,std::string judge_pattern) : pattern_(pattern),judge_pattern_(judge_pattern)
    {
        judge_first = judge_pattern.at(0);
        pattern_first = pattern.at(0);
    }
    ~automaton() = default;
    bool init( std::ifstream &infilestream,std::vector<int> *lines){
        if (!infilestream.is_open())
        {
            printf("err\n");
            return false;
        }
        char mchar;
        while (infilestream.get(mchar)) {
            switch (currentState)
            {
            case State::Normal:
                if(mchar == judge_first){
                    judge_num = 1;
                    currentState = State::JudgeIf;
                }
                if(mchar == '\n')
                    line_num++;
                break;            
            case State::JudgeIf:
                if(mchar == '\n')
                    line_num++;
                if(judge_num < judge_pattern_.size()){
                    if(mchar == judge_pattern_.at(judge_num)){
                        judge_num++;
                    }else{
                        judge_num = 0;
                        currentState = State::Normal;
                    }
                }else{
                    if(mchar == ' '){
                        judge_num = 0;
                        (*lines).push_back(line_num);
                        currentState = State::InsideIfCondition;
                    }else if(mchar == '('){
                        bracket_num++;
                        judge_num = 0;
                        (*lines).push_back(line_num);
                        currentState = State::InsideIfCondition;
                    }
                    else{
                        judge_num = 0;
                        currentState = State::Normal;
                    }
                }
                break;
            case State::JudgePattern:
                if(mchar == '\n')
                    line_num++;
                break;
            case State::InsideIfCondition:
                if(mchar == '\n')
                    line_num++;
                if(mchar == ')'){
                    bracket_num--;
                    if(bracket_num == 0){
                        bracket_num = 0;
                        currentState = State::Normal;
                    }
                }else if(mchar == '('){
                    bracket_num++;
                }
                break;
            }
        }
        return true;
    }
    
private:
    std::string judge_pattern_;
    std::string pattern_;
    char judge_first{0};
    char pattern_first{0};
    int judge_num{0};
    int pattern_num{0};
    int bracket_num{0};
    int line_num{1};
    State currentState{State::Normal};
};


int main(){
    automaton mauto{"offset","if"};
    std::ifstream infilestream;
    infilestream.open("./auto1.cpp");
    std::vector<int> lines;
    mauto.init(infilestream,&lines);
    for(auto line : lines){
        std::cout << line << std::endl;
    }
}