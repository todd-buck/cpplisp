#include "reader.hpp"

using namespace std;

vector <string_view> tokenize(string &input) {
    Tokenizer tokenizer{input};
    vector <string_view> vector;

    while (auto token = tokenizer.next()) {
        vector.push_back(*token);
    }

    return vector;
}

Value *read_str(string &input) {
    auto tokens = tokenize(input);

    Reader reader{tokens};

    return read_form(reader);
}

Value *read_form(Reader &reader) {
    auto maybe_token = reader.peek();

    if (!maybe_token) return nullptr;

    auto token = maybe_token.value();

    switch (token[0]) {
        case '(':
            return read_list(reader);
        case '-': {
            if (token.length() == 1 || !isdigit(token[1])) { return read_atom(reader); }

            return read_integer(reader);
        }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return read_integer(reader);
        default:
            return read_atom(reader);
    }
}

Value *read_integer(Reader &reader) {
    auto token = reader.next();
    long number = 0;
    bool negative = false;
    for (char c: *token) {
        if (c == '-') {
            negative = true;
        } else {
            number *= 10;
            int digit = c - 48;
            number += digit;
        }
    }

    if (negative) {
        number *= -1;
    }

    return new IntegerValue{number};
}

Value *read_list(Reader &reader) {
    //consume '('
    reader.next();

    auto *list = new ListValue{};

    while (auto token = reader.peek()) {
        if (*token == ")") {
            reader.next();
            break;
        }
        list->push(read_form(reader));
    }

    return list;
}

Value *read_atom(Reader &reader) {
    return new SymbolValue{*reader.next()};
}