#include "reader.hpp"

using namespace std;

vector<string_view> tokenize(string &input) {
    Tokenizer tokenizer { input };
    vector<string_view> vector;

    while(auto token = tokenizer.next()) {
        vector.push_back(*token);
    }

    return vector;
}

Value *read_str(string &input) {
    auto tokens  = tokenize(input);

    Reader reader { tokens };

    return read_form(reader);
}

Value* read_form(Reader &reader) {
    auto token = reader.peek();

    if(!token) return nullptr;

    switch(token.value()[0]) {
        case '(': 
            return read_list(reader);
        default:
            return read_atom(reader);
    }
}

Value *read_list(Reader &reader) {
    //consume '('
    reader.next();

    auto *list = new ListValue {};

    while(auto token = reader.peek()) {
        if(*token == ")") {
            reader.next();
            break;
        }
        list->push(read_form(reader));
    }

    return list;
}

Value *read_atom(Reader &reader) {
    return new SymbolValue { *reader.next() };
}