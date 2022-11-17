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

IntegerValue *Value::as_integer() {
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

TrueValue *Value::as_true() {
    assert(type() == Type::True);
    return static_cast<TrueValue*>(this);
}

FalseValue *Value::as_false() {
    assert(type() == Type::False);
    return static_cast<FalseValue*>(this);
}

NilValue *Value::as_nil() {
    assert(type() == Type::Nil);
    return static_cast<NilValue*>(this);
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

bool ListValue::operator==(const Value *other) const {
    if(type() != other->type()) {
        return false;
    }
    auto other_list = const_cast<Value *>(other)->as_list();

    if(size() != other_list->size()) {
        return false;
    }

    for(size_t i = 0; i < size(); ++i) {
        if(!(*at(i) == other_list->at(i))) return false;
    }

    return true;
}