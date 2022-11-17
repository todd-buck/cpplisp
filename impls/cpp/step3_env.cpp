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
        auto list = ast->as_list();
        auto first= list->at(0); 
        if (first->is_symbol() && first->as_symbol()->matches("def!")) {
            // symbol "def!": call the set method of the current environment (second parameter of EVAL called env) using the unevaluated first parameter (second list element) as the symbol key and the evaluated second parameter as the value.
            auto key = list->at(1)->as_symbol();
            auto val = EVAL(list->at(2), env);
            env.set(key, val);
            return val;
        }
        else if (first->is_symbol() && first->as_symbol()->matches("let*")) {
            // symbol "let*": create a new environment using the current environment as the outer value and then use the first parameter as a list of new bindings in the "let*" environment. Take the second element of the binding list, call EVAL using the new "let*" environment as the evaluation environment, then call set on the "let*" environment using the first binding list element as the key and the evaluated second element as the value. This is repeated for each odd/even pair in the binding list. Note in particular, the bindings earlier in the list can be referred to by later bindings. Finally, the second parameter (third element) of the original let* form is evaluated using the new "let*" environment and the result is returned as the result of the let* (the new let environment is discarded upon completion).
            auto new_env = new Env { &env };
            auto bindings = list->at(1)->as_list();
            for (size_t i = 0; i < bindings->size(); i+= 2) {
                auto key = bindings->at(i)->as_symbol();
                assert(i+1 < bindings->size()); // CORRECTION FROM VIDEO AROUND 26:37
                auto val = EVAL(bindings->at(i+1), *new_env);
                new_env->set(key, val);
            }
            return EVAL(list->at(2), *new_env);
        }
        else {
            // otherwise: call eval_ast on the list and apply the first element to the rest as before.
            // old code
            auto list = eval_ast(ast, env)->as_list();
            auto fn = list->at(0)->as_function()->to_function();
            Value *args[list->size() - 1];
            for (size_t i = 1; i < list->size(); ++i) {
                args[i - 1] = list->at(i);
            }
            return fn(list->size() - 1, args);
        }

    }
}

// modify eval_ast to call the get() method on the "env" parameter
Value *eval_ast(Value *ast, Env &env) {
    switch (ast->type()) {
        case Value::Type::Symbol: {
            return env.get(ast->as_symbol());
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

    auto env = new Env { nullptr }; // top level Env
    env->set(new SymbolValue("+"), new FunctionValue{add}); 
    env->set(new SymbolValue("-"), new FunctionValue{sub});
    env->set(new SymbolValue("*"), new FunctionValue{mul});
    env->set(new SymbolValue("/"), new FunctionValue{div});


    string input;
    while (true) {

        auto quit = linenoise::Readline("user> ", input);

        if (quit) break;

        cout << rep(input, *env) << endl;

        linenoise::AddHistory(input.c_str());
    }

    linenoise::SaveHistory(history_path);

    return 0;
}