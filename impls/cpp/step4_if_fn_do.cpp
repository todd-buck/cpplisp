#include <iostream>
#include <unordered_map>

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
            if (special->matches("def!")) {
                // symbol "def!": call the set method of the current environment (second parameter of EVAL called env) using the unevaluated first parameter (second list element) as the symbol key and the evaluated second parameter as the value.
                auto key = list->at(1)->as_symbol();
                auto val = EVAL(list->at(2), env);
                env.set(key, val);
                return val;
            }
            else if (special->matches("let*")) {
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
            } else if(special->matches("do")) {
                Value *result = nullptr;
                assert(list->size() > 1);
                for(size_t i = 0; i < list->size(); i++) {
                    result = eval_ast(list->at(i), env);
                    return result;
                }
            } else if(special->matches("if")) {
                auto condition = list->at(1);
                auto true_expr = list->at(2);
                auto false_expr = list->size() >= 4 ? list->at(3) : NilValue::the();
                if(EVAL(condition, env)->is_truthy()) {
                    return EVAL(true_expr, env);
                } else {
                    return EVAL(false_expr, env);
                }
            } else if(special->matches("fn*")) {
                auto env_ptr = &env;
                auto binds = list->at(1)->as_list();
                auto body = list->at(2);
                auto closure = [env_ptr, binds, body](size_t argc, Value ** args) {
                    auto exprs = new ListValue {};
                    for(size_t i = 0; i < argc; i++) {
                        exprs->push(args[i]);
                    }
                    auto function_env = new Env { env_ptr, binds, exprs };
                    return EVAL(body, *function_env);
                };
                return new FunctionValue { closure };
            }
            else if(special->matches("set")) {

                // TESTED, WORKS
                // I think we can just reuse def here
                assert(list->size() == 3);
                auto key = list->at(1)->as_symbol();
                auto val = EVAL(list->at(2), env);
                env.set(key, val);
                // If there's no return variable then it'll still set the variable
                // but say "set not found" after. Returning nullptr creates a segfault.
                return val;

            }
            else if(special->matches("cond")) {
                // (cond t1 r1 t2 r2 t3 r3)
                // if t1 is true returns r1...if t2 is true return r2...
                // Most efficient if lazy evalauation is used.
                // Behavior undefined if no tn is true. (probably return nil, buit exit(1) is also fine)

                // FIXME: if this is fucking up do size-1
                for (int i = 1; i < int(list->size()); i += 2) { // need to cast to int instead of size_t original type
                    auto tcond = list->at(i);
                    auto treturn = list->at(i+1);
                    if(EVAL(tcond, env)->is_truthy()) {
                        return EVAL(treturn, env);
                    } 
                }
                return NilValue::the();
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