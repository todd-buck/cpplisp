#include <iostream>

#include "core.hpp"
#include "printer.hpp"

using namespace std;

unordered_map<string, Function> build_namespace() {
    unordered_map<string, Function> ns;

    ns["+"] = add;
    ns["-"] = subtract;
    ns["*"] = multiply;
    ns["/"] = divide;
    ns["list"] = list;
    ns["list?"] = list_q;
    ns["empty?"] = empty_q;
    ns["count"] = cnt;
    ns["="] = eq;
    ns["<"] = lt;
    ns["<="] = lte;
    ns[">"] = gt;
    ns[">="] = gte;

    ns["prn"] = prn;
    ns["pr-str"] = pr_str_function;
    ns["println"] = println;
    ns["str"] = str;
    ns["not"] = not_function;

    return ns;
}

Value *add(size_t argc, Value **args) {
    assert(argc == 2);
    auto a = args[0];
    auto b = args[1];

    assert(a->type() == Value::Type::Integer);
    assert(b->type() == Value::Type::Integer);

    long result = a->as_integer()->to_long() + b->as_integer()->to_long();

    return new IntegerValue{result};
}

Value *subtract(size_t argc, Value **args) {
    assert(argc == 2);
    auto a = args[0];
    auto b = args[1];

    assert(a->type() == Value::Type::Integer);
    assert(b->type() == Value::Type::Integer);

    long result = a->as_integer()->to_long() - b->as_integer()->to_long();

    return new IntegerValue{result};
}

Value *multiply(size_t argc, Value **args) {
    assert(argc == 2);
    auto a = args[0];
    auto b = args[1];

    assert(a->type() == Value::Type::Integer);
    assert(b->type() == Value::Type::Integer);

    long result = a->as_integer()->to_long() * b->as_integer()->to_long();

    return new IntegerValue{result};
}

Value *divide(size_t argc, Value **args) {
    assert(argc == 2);
    auto a = args[0];
    auto b = args[1];

    assert(a->type() == Value::Type::Integer);
    assert(b->type() == Value::Type::Integer);

    long result = a->as_integer()->to_long() / b->as_integer()->to_long();

    return new IntegerValue{result};
}

Value *prn(size_t argc, Value **args) {
    if(argc == 0) {
        cout << "\n";
        return NilValue::the();
    }

    for (size_t i = 0; i < argc; ++i) {
        cout << print_string(args[i], true);
        if (i < argc - 1)
            cout << ' ';
    }
    cout << "\n";
    return NilValue::the();
}

Value *list(size_t argc, Value **args) {
    auto parameter_list = new ListValue { };
    for(size_t i = 0; i < argc; i++) {
        parameter_list->push(args[i]);
    }
    return parameter_list;
}

Value *list_q(size_t argc, Value **args) {
    assert(argc >= 1);
    if(args[0]->is_list()) {
        return TrueValue::the();
    } else {
        return FalseValue::the();
    }
}

Value *empty_q(size_t argc, Value **args) {
    assert(argc >= 1);
    if (args[0]->is_list() && args[0]->as_list()->is_empty())
        return TrueValue::the();
    return FalseValue::the();
}

Value *cnt(size_t argc, Value **args) {
    assert(argc >= 1);
    if (args[0]->is_list())
        return new IntegerValue { static_cast<long>(args[0]->as_list()->size()) };
    return new IntegerValue { 0 };
}

Value *eq(size_t argc, Value **args) {
    assert(argc >= 2);
    auto a = args[0];
    auto b = args[1];

    if(*a == b) 
        return TrueValue::the();
    return FalseValue::the();
}

Value *lt(size_t argc, Value **args) {
    assert(argc >= 2);
    auto a = args[0];
    auto b = args[1];
    assert(a->is_integer());
    assert(b->is_integer());
    if(a->as_integer()->to_long() < b->as_integer()->to_long()) {
        return TrueValue::the();
    } else {
        return FalseValue::the();
    }
}

Value *lte(size_t argc, Value **args) {
    assert(argc >= 2);
    auto a = args[0];
    auto b = args[1];
    assert(a->is_integer());
    assert(b->is_integer());
    if(a->as_integer()->to_long() <= b->as_integer()->to_long()) {
        return TrueValue::the();
    } else {
        return FalseValue::the();
    }
}

Value *gt(size_t argc, Value **args) {
    assert(argc >= 2);
    auto a = args[0];
    auto b = args[1];
    assert(a->is_integer());
    assert(b->is_integer());
    if(a->as_integer()->to_long() > b->as_integer()->to_long()) {
        return TrueValue::the();
    } else {
        return FalseValue::the();
    }
}

Value *gte(size_t argc, Value **args) {
    assert(argc >= 2);
    auto a = args[0];
    auto b = args[1];
    assert(a->is_integer());
    assert(b->is_integer());
    if(a->as_integer()->to_long() >= b->as_integer()->to_long()) {
        return TrueValue::the();
    } else {
        return FalseValue::the();
    }
}

Value *pr_str_function(size_t argc, Value **args) {
    if (argc == 0)
        return new StringValue { "" };
    string str = "";
    for (size_t i = 0; i < argc; ++i) {
        str += print_string(args[i], true);
        if (i < argc - 1)
            str += ' ';
    }
    return new StringValue { str };
}

Value *println(size_t argc, Value **args) {
    if (argc == 0) {
        cout << "\n";
        return NilValue::the();
    }
    string str = "";
    for (size_t i = 0; i < argc; ++i) {
        str += print_string(args[i], false);
        if (i < argc - 1)
            str += ' ';
    }
    cout << str << "\n";
    return NilValue::the();
}

Value *str(size_t argc, Value **args) {
    if (argc == 0)
        return new StringValue { "" };
    string str = "";
    for (size_t i = 0; i < argc; ++i) {
        str += print_string(args[i], false);
    }
    return new StringValue { str };
}

Value *not_function(size_t argc, Value **args) {
    assert(argc >= 1);
    if (args[0]->is_truthy())
        return FalseValue::the();
    return TrueValue::the();
}