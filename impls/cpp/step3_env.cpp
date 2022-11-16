#include <iostream>
#include <unordered_map>

#include "../../linenoise.hpp"
#include "reader.hpp"
#include "printer.hpp"
#include "env.hpp"

using namespace std;

Value *READ(string input) {
    return read_str(input);
}

Value *eval_ast(Value *ast, Env &env);

Value *EVAL(Value *ast, Env &env) {
    if (ast->type() != Value::Type::List) {
        return eval_ast(ast, env);
    } else if (ast->as_list()->is_empty()) {
        return ast;
    } else {
        auto list = eval_ast(ast, env)->as_list();
        auto fn = list->at(0)->as_function()->to_function();
        Value *args[list->size() - 1];
        for (size_t i = 1; i < list->size(); ++i) {
            args[i - 1] = list->at(i);
        }
        return fn(list->size() - 1, args);

    }
}

Value *eval_ast(Value *ast, Env &env) {
    switch (ast->type()) {
        case Value::Type::Symbol: {
            auto search = env.find(ast->as_symbol());
            if (search == env.end()) {
                throw new ExceptionValue {ast->as_symbol()->str() + " not found."};
                //return new SymbolValue{"nil"};
            }
            return search->second;
        }
        case Value::Type::List: {
            auto result = new ListValue{};

            for (auto val: *ast->as_list()) {
                result->push(EVAL(val, env));
            }

            return result;
        }
        default:
            return ast;
    }

}

string PRINT(Value *input) {
    return print_string(input);
}

string rep(string input, Env &env) {
    try {
        auto ast = READ(input);
        auto result = EVAL(ast, env);
        return PRINT(result);
    } catch (ExceptionValue* exception) {
        cerr << exception->message() << endl;
        return "";
    }
}

Value *add(size_t argc, Value **args) {
    assert(argc == 2);
    auto a = args[0];
    auto b = args[1];

    assert(a->type() == Value::Type::Integer);
    assert(b->type() == Value::Type::Integer);

    long result = a->as_int()->to_long() + b->as_int()->to_long();

    return new IntegerValue{result};
}

Value *sub(size_t argc, Value **args) {
    assert(argc == 2);
    auto a = args[0];
    auto b = args[1];

    assert(a->type() == Value::Type::Integer);
    assert(b->type() == Value::Type::Integer);

    long result = a->as_int()->to_long() - b->as_int()->to_long();

    return new IntegerValue{result};
}

Value *mul(size_t argc, Value **args) {
    assert(argc == 2);
    auto a = args[0];
    auto b = args[1];

    assert(a->type() == Value::Type::Integer);
    assert(b->type() == Value::Type::Integer);

    long result = a->as_int()->to_long() * b->as_int()->to_long();

    return new IntegerValue{result};
}

Value *div(size_t argc, Value **args) {
    assert(argc == 2);
    auto a = args[0];
    auto b = args[1];

    assert(a->type() == Value::Type::Integer);
    assert(b->type() == Value::Type::Integer);

    long result = a->as_int()->to_long() / b->as_int()->to_long();

    return new IntegerValue{result};
}

int main() {
    const auto history_path = "history.txt";

    // Set max length of the history
    linenoise::SetHistoryMaxLen(10);

    // Load history
    linenoise::LoadHistory(history_path);
    Env env{};
    env[new SymbolValue("+")] = new FunctionValue{add};
    env[new SymbolValue("-")] = new FunctionValue{sub};
    env[new SymbolValue("*")] = new FunctionValue{mul};
    env[new SymbolValue("/")] = new FunctionValue{div};


    string input;
    while (true) {

        auto quit = linenoise::Readline("user> ", input);

        if (quit) break;

        cout << rep(input, env) << endl;

        linenoise::AddHistory(input.c_str());
    }

    linenoise::SaveHistory(history_path);

    return 0;
}