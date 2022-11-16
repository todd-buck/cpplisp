#include "types.hpp"

using namespace std;

ListValue *Value::as_list() {
    assert(type() == Type::List);
    return static_cast<ListValue*>(this);
}

SymbolValue *Value::as_symbol() {
    assert(type() == Type::Symbol);
    return static_cast<SymbolValue*>(this);
}

IntegerValue *Value::as_int() {
    assert(type() == Type::Integer);
    return static_cast<IntegerValue*>(this);
}

FunctionValue *Value::as_function() {
    assert(type() == Type::Function);
    return static_cast<FunctionValue*>(this);
}

ExceptionValue *Value::as_exception() {
    assert(type() == Type::Exception);
    return static_cast<ExceptionValue*>(this);
}


//builds string out of list (for printing)
string ListValue::inspect() const {
    string out = "(";

    for (auto *value: m_list) {
        out.append(value->inspect());
        out.append(" ");
    }

    if (m_list.size() > 0) {
        out[out.length() - 1] = ')';
    } else {
        out.append(")");
    }


    return out;
}