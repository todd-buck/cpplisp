#include <iostream>

#include "core.hpp"
#include "printer.hpp"

using namespace std;

unordered_map<string, Function> build_namespace() {
    unordered_map<string, Function> ns;

    // arithmetic
    ns["+"] = add;
    ns["-"] = subtract;
    ns["*"] = multiply;
    ns["/"] = divide;

    // verification
    ns["list?"] = list_q;
    ns["number?"] = number_q;
    ns["nil?"] = nil_q;
    ns["symbol?"];                    //implemented in main
    
    // logic
    ns["and?"] = and_q;
    ns["or?"] = or_q;
    ns["="] = eq;
    ns["<"] = lt;
    ns[">"] = gt;
    ns["if"];                        //implemented in main
    
    // cons cell functionality
    ns["cons"] = cons;
    ns["car"] = car;
    ns["cdr"] = cdr;
    ns["cond"];                     //implemented in main

    // variable/function assignment
    ns["set"];                       //implemented in main
    ns["define"];                    //implemented in main

    //Expanded functionality (not in project scope)
    ns["list"] = list;
    ns["print"] = print;

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

Value *list_q(size_t argc, Value **args) {
    assert(argc >= 1);
    if(args[0]->is_list()) {
        return TrueValue::the();
    } else {
        return FalseValue::the();
    }
}

// (number? Expr)
// Returns T if the expr is numeric, () otherwise
Value *number_q(size_t argc, Value **args) {

    // TESTED, WORKS
    assert(argc >= 1);
    if (args[0]->is_integer())
        return TrueValue::the();
    return NilValue::the();
}

// (nil? Expr)
// Return T iff Expr is ()
Value *nil_q(size_t argc, Value **args) {
    assert(argc >= 1);
    if (args[0]->is_nil())
        return TrueValue::the();
    return FalseValue::the();

}

// (AND? exp1 exp2)
// Return nil if either expression is nil
Value *and_q(size_t argc, Value **args) {
    assert(argc >= 2);
    if (args[0]->is_nil() || args[1]->is_nil())
        return NilValue::the();
    return TrueValue::the();    
}

// (OR? exp1 exp2)
// Return nil if both expressions are nil
Value *or_q(size_t argc, Value **args) {
    assert(argc >= 2);
    if (args[0]->is_nil() && args[1]->is_nil())
        return NilValue::the();
    return TrueValue::the();      
}

Value *eq(size_t argc, Value **args) {

    assert(argc >= 2);
    auto a = args[0];
    auto b = args[1];

    if(*a == b || (a->is_nil() && b->is_nil())
        || (a->is_true() && b->is_true())
        || (a->is_false() && b->is_false())) {
        return TrueValue::the();
    }

    return NilValue::the();
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

// (cons expr1 expr2)
// Create a cons cell with expr1 as car and expr2 and cdr: ie: (exp1 . expr2)
Value *cons(size_t argc, Value **args) {
    assert(argc >= 2);
    auto cons_cell = new ListValue { };

    if (args[0]->is_list()) {
        for (size_t i = 0; i < args[0]->as_list()->size(); i++) {
            cons_cell->push(args[0]->as_list()->at(i));
        }
    }
    else {
        cons_cell->push(args[0]);
    }

    if (args[1]->is_list()) {
        for (size_t i = 0; i < args[1]->as_list()->size(); i++) {
            cons_cell->push(args[1]->as_list()->at(i));
        }
    }
    else {
        cons_cell->push(args[1]);
    }

    return cons_cell;
}

// (car expr)
// Expr should be a non empty list. Car returns the car cell of the first cons cell
Value *car(size_t argc, Value **args) {
    assert(argc >= 1);
    
    if (args[0]->is_list() && !args[0]->as_list()->is_empty()) {
        return args[0]->as_list()->at(0);
    }
    else if (args[0]->is_integer()) {
        return args[0]->as_integer();
    }

    return NilValue::the();
}

// (cdr expr)
// Expr should be a non empty list. Cdr returns the cdr cell of the first cons cell
Value *cdr(size_t argc, Value **args) {
    assert(argc >= 1);
    if (args[0]->is_list() && !args[0]->as_list()->is_empty()) {
        auto parameter_list = new ListValue { };
        for(size_t i = 1; i < args[0]->as_list()->size(); i++) {
            parameter_list->push(args[0]->as_list()->at(i));
        }
        return parameter_list;
    }
    return NilValue::the();
}

Value *list(size_t argc, Value **args) {
    auto parameter_list = new ListValue { };
    for(size_t i = 0; i < argc; i++) {
        parameter_list->push(args[i]);
    }
    return parameter_list;
}

Value *print(size_t argc, Value **args) {
    assert(argc >= 1);
    cout << print_string(args[0], true) << endl;
    return NothingValue::the();
}