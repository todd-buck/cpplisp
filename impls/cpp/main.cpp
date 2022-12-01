#include <iostream>
#include <unordered_map>
#include <regex>

#include "../../linenoise.hpp"
#include "reader.hpp"
#include "printer.hpp"
#include "env.hpp"
#include "core.hpp"

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
        if(first->is_symbol()) {
            auto special = first->as_symbol();
            if (special->matches("define")) {
                auto key = list->at(1)->as_symbol();                
                auto env_ptr = &env;
                auto binds = list->at(2)->as_list();
                auto body = list->at(3);
                auto closure = [env_ptr, binds, body](size_t argc, Value ** args) {
                    auto exprs = new ListValue {};
                    for(size_t i = 0; i < argc; i++) {
                        exprs->push(args[i]);
                    }
                    auto function_env = new Env { env_ptr, binds, exprs };
                    return EVAL(body, *function_env);
                };
                auto val = new FunctionValue { closure };
                env.set(key, val);
                return NothingValue::the();
            }
            else if(special->matches("if")) {
                auto condition = list->at(1);
                auto true_expr = list->at(2);
                auto false_expr = list->size() >= 4 ? list->at(3) : NilValue::the();
                if(EVAL(condition, env)->is_truthy()) {
                    return EVAL(true_expr, env);
                } else {
                    return EVAL(false_expr, env);
                }
            }
            else if(special->matches("set")) {
                //set potential_variable value
                assert(list->size() == 3);

                //casts potential variable as a string
                string potential_variable = list->at(1)->inspect();

                //if variable is only numbers
                if(regex_match(potential_variable.begin(), potential_variable.end(), regex("^[0-9]*$"))) {
                    throw new ExceptionValue { "variable (" + potential_variable + ") cannot contain only numbers." };
                }

                //if variable contains illegal characters
                if(!regex_match(potential_variable.begin(), potential_variable.end(), regex("^[A-Za-z0-9_-]*$"))) {
                    throw new ExceptionValue { "variable (" + potential_variable + ") must only contain letters, numbers, and variables." };
                }

                auto key = list->at(1)->as_symbol();
                auto val = EVAL(list->at(2), env);
                env.set(key, val);
                // If there's no return variable then it'll still set the variable
                // but say "set not found" after. Returning nullptr creates a segfault.
                return NothingValue::the();

            }
            else if(special->matches("cond")) { // keep
                // (cond t1 r1 t2 r2 t3 r3)
                // if t1 is true returns r1...if t2 is true return r2...
                // Most efficient if lazy evalauation is used.
                
                for (size_t i = 1; i < list->size(); i += 2) { // need to cast to int instead of size_t original type
                    auto tcond = list->at(i);
                    auto treturn = list->at(i+1);
                    if(EVAL(tcond, env)->is_truthy()) {
                        return EVAL(treturn, env);
                    } 
                }
                return NilValue::the();
            }
            else if(special->matches("symbol?")) { // keep
                if(list->size() != 2) {
                    throw new ExceptionValue { "Usage: (symbol? expr)" };
                }

                // If expr is any type other than symbol, return false
                if(list->at(1)->type() != Value::Type::Symbol) {
                    return FalseValue::the();
                }
                
                // If symbol is not in current or top-level environment, return false
                if(env.find(list->at(1)->as_symbol()) == nullptr) {
                    return FalseValue::the();
                }

                return TrueValue::the();
            }
        }
        // otherwise: call eval_ast on the list and apply the first element to the rest as before.
        // old code
        auto eval_list = eval_ast(ast, env)->as_list();
        auto fn = eval_list->at(0)->as_function()->to_function();
        Value *args[eval_list->size() - 1];
        for (size_t i = 1; i < eval_list->size(); ++i) {
            args[i - 1] = eval_list->at(i);
        }
        return fn(eval_list->size() - 1, args);

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

int main() {
    const auto history_path = "history.txt";

    // Set max length of the history
    linenoise::SetHistoryMaxLen(10);

    // Load history
    linenoise::LoadHistory(history_path);

    auto env = new Env { nullptr }; // top level Env

    auto ns = build_namespace();
    for(auto pair: ns) {
        env->set(new SymbolValue(pair.first), new FunctionValue(pair.second)); 
    }

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