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
    //ns["symbol?"] = symbol_q;
    ns["nil?"] = nil_q;
    ns["cons"] = cons;
    ns["car"] = car;
    ns["cdr"] = cdr;
    ns["prn"] = prn;
    ns["and?"] = and_q;
    ns["or?"] = or_q;

    // to be removed?
    ns["<="] = lte;
    ns[">="] = gte;
    ns["count"] = cnt; // size of a list
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

// modify to be cons
Value *list(size_t argc, Value **args) {
    //assert size == 2
    //auto cons_cell = new ListValue { }
    //push 1st arg
    //push 2nd arg
    //return cons_cell
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

    // TESTED, WORKS

    // implent as NumberValue
    assert(argc >= 1);
    if (args[0]->is_integer()) // ??
        return TrueValue::the();
    return NilValue::the();
}

// // (symbol? Expr)
// // Returns T if the expr is a name, () otherwise
// Value *symbol_q(size_t argc, Value **args) {
//     assert(argc >= 1);

//     auto key = args[0]->as_symbol();
    
//     // string name = args[0];
    
//     // for (size_t i = 0; i < sym->length(); i++) {
//     //     if ( !isalpha(sym->at(i)) || sym->at(i) != '_') {
//     //         return NilValue::the();
//     //     }
//     // }

//     //return TrueValue::the();

//     return key;
// }

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
    auto cons_cell = new ListValue { };

    if (args[0]->is_list()) {
        for (size_t i = 0; i < args[0]->as_list()->size(); i++) {
            cons_cell->push(args[0]->as_list()->at(i));
        }
    }
    else if (args[0]->is_integer()) {
        cons_cell->push(args[0]);
    }

    cons_cell->push(args[1]);
    return cons_cell;
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
        auto parameter_list = new ListValue { };
        for(size_t i = 1; i < args[0]->as_list()->size(); i++) {
            parameter_list->push(args[0]->as_list()->at(i));
        }
        return parameter_list;
    }
    return NilValue::the();
}

// (AND? exp1 exp2)
// Return nil if either expression is nil
// TESTED, CORRECT
Value *and_q(size_t argc, Value **args) {
    assert(argc >= 2);
    if (args[0]->is_nil() || args[1]->is_nil())
        return NilValue::the();
    return TrueValue::the();    
}

// (OR? exp1 exp2)
// Return nil if both expressions are nil
// TESTED, CORRECT
Value *or_q(size_t argc, Value **args) {
    assert(argc >= 2);
    if (args[0]->is_nil() && args[1]->is_nil())
        return NilValue::the();
    return TrueValue::the();      
}