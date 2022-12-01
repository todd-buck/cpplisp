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
        assert(token.size() >= 1);
        if(token == "true") {
            reader.next();
            return TrueValue::the();
        } else if(token == "false") {
            reader.next();
            return FalseValue::the();
        } else if(token == "nil") {
            reader.next();
            return NilValue::the();
        } else if (token[0] == '"') {
            return read_string(reader);
        } else if (token[0] == ':') {
            reader.next();
            return new KeywordValue { token };
        }
            return read_atom(reader);
    }
}

Value *read_integer(Reader &reader) {
    auto token = reader.next();
    long number = 0;
    bool negative = false;
    for (char c: *token) {
        // need to modify here for decimals
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

ListValue *read_list(Reader &reader) {
    //consume '('
    reader.next();

    auto *list = new ListValue{};

    while (auto token = reader.peek()) {
        if (*token == ")") {
            reader.next();
            return list;
        }
        list->push(read_form(reader));
    }

    cerr << "EOF\n";
    return list;
}

Value *read_atom(Reader &reader) {
    return new SymbolValue{*reader.next()};
}

Value *read_string(Reader &reader) {
    auto token = reader.next().value();
    if (token.size() < 2)
        throw new ExceptionValue { "end of input" };
    assert(token.size() >= 2);
    if (token.size() == 2)
        return new StringValue { "" };
    auto str = token.substr(1, token.size() - 2);
    string processed = "";
    for (size_t i = 0; i < str.size(); ++i) {
        auto c = str[i];
        switch (c) {
        case '"':
            processed += '\\';
            processed += c;
            break;
        case '\\': {
            if (++i >= str.size())
                throw new ExceptionValue { "unbalanced quotes" };
            c = str[i];
            switch (c) {
            case 'n':
                processed += "\n";
                break;
            default:
                processed += c;
            }
            break;
        }
        default:
            processed += c;
        }
    }
    return new StringValue { processed };
}