#include <iostream>

#include "../../linenoise.hpp"
#include "reader.hpp"
#include "printer.hpp"

using namespace std;

Value* READ(string input) {
    return read_str(input);
}

Value* EVAL(Value* input) {
    return input;
}

string PRINT(Value* input) {
    return print_string(input);
}

string rep(string input) {
    auto ast = READ(input);
    auto result = EVAL(ast);
    return PRINT(result);

}

int main() {
    const auto history_path = "history.txt";

    string input;

    // Set max length of the history
    linenoise::SetHistoryMaxLen(10);

    // Load history
    linenoise::LoadHistory(history_path);

    while(true) {

        auto quit = linenoise::Readline("user> ", input);

        if(quit) break;

        cout << rep(input) << endl;

        linenoise::AddHistory(input.c_str());
    }

    linenoise::SaveHistory(history_path);

    return 0;
}