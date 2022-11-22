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
    ns["list?"] = list_q;
    ns["="] = eq;
    ns["<"] = lt;
    ns[">"] = gt;

    // new additions
    ns["number?"] = number_q;
    ns["symbol?"] = symbol_q;
    ns["nil?"] = nil_q;
    ns["cons"] = cons;
    ns["car"] = car;
    ns["cdr"] = cdr;

    // to be removed?
    ns["<="] = lte;
    ns[">="] = gte;
    ns["count"] = cnt; // size of a list
    ns["prn"] = prn; // empty parenthesis?
    ns["list"] = list; // list constructor?
    ns["empty?"] = empty_q; // is this "nil?"

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
    assert(argc >= 1);
    cout << print_string(args[0]) << endl;
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

// NEW THINGS YAY!!!!!

// (number? Expr)
// Returns T if the expr is numeric, () otherwise
Value *number_q(size_t argc, Value **args) {
    assert(argc >= 1);
    if (args[0]->is_integer()) // ??
        return TrueValue::the();
    return FalseValue::the();
}

// (symbol? Expr)
// Returns T if the expr is a name, () otherwise
Value *symbol_q(size_t argc, Value **args) {
    assert(argc >= 1);
    if (args[0]->is_symbol())
        return TrueValue::the();
    return FalseValue::the();
}

// (nil? Expr)
// Return T iff Expr is ()
Value *nil_q(size_t argc, Value **args) {
    assert(argc >= 1);
    if (args[0]->is_nil())
        return TrueValue::the();
    return FalseValue::the();
}

// (cons expr1 expr2)
// Create a cons cell with expr1 as car and expr2 and cdr: ie: (exp1 . expr2)
Value *cons(size_t argc, Value **args) {
    assert(argc >= 2);

    auto car = args[0]->as_list();
    auto cdr = args[1]->as_list();

    return car->push(cdr)->as_list();
    // WHY IS THIS SCREAMING??????
}

// (car expr)
// Expr should be a non empty list. Car returns the car cell of the first cons cell
Value *car(size_t argc, Value **args) {
    assert(argc >= 1);
    if (args[0]->is_list() && !args[0]->as_list()->is_empty()) {
        return args[0]->as_list()->at(0);
    }
    return NilValue::the();
}

// (cdr expr)
// Expr should be a non empty list. Cdr returns the cdr cell of the first cons cell
Value *cdr(size_t argc, Value **args) {
    assert(argc >= 1);
    if (args[0]->is_list() && !args[0]->as_list()->is_empty()) {
        return args[0]->as_list()->at(1);
        // this is wrong
    }
    return NilValue::the();
}