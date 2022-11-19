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

StringValue *Value::as_string() {
    assert(type() == Type::String);
    return static_cast<StringValue *>(this);
}

KeywordValue *Value::as_keyword() {
    assert(type() == Type::Keyword);
    return static_cast<KeywordValue *>(this);
}

const ListValue *Value::as_list() const {
    assert(type() == Type::List);
    return static_cast<const ListValue *>(this);
}

const SymbolValue *Value::as_symbol() const {
    assert(type() == Type::Symbol);
    return static_cast<const SymbolValue *>(this);
}

const IntegerValue *Value::as_integer() const {
    assert(type() == Type::Integer);
    return static_cast<const IntegerValue *>(this);
}

const FunctionValue *Value::as_function() const {
    assert(type() == Type::Function);
    return static_cast<const FunctionValue *>(this);
}

const ExceptionValue *Value::as_exception() const {
    assert(type() == Type::Exception);
    return static_cast<const ExceptionValue *>(this);
}

const TrueValue *Value::as_true() const {
    assert(type() == Type::True);
    return static_cast<const TrueValue *>(this);
}

const FalseValue *Value::as_false() const {
    assert(type() == Type::False);
    return static_cast<const FalseValue *>(this);
}

const NilValue *Value::as_nil() const {
    assert(type() == Type::Nil);
    return static_cast<const NilValue *>(this);
}

const StringValue *Value::as_string() const {
    assert(type() == Type::String);
    return static_cast<const StringValue *>(this);
}

const KeywordValue *Value::as_keyword() const {
    assert(type() == Type::Keyword);
    return static_cast<const KeywordValue *>(this);
}

//builds string out of list (for printing)
string ListValue::inspect(bool print_readably) const {
    string out = "(";

    for (auto *value: m_list) {
        out.append(value->inspect(print_readably));
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

string StringValue::inspect(bool print_readably) const {
    if (print_readably) {
        string str = "\"";
        for (char c : m_str) {
            switch (c) {
            case '"':
                str += '\\';
                str += c;
                break;
            case '\\':
                str += '\\';
                str += '\\';
                break;
            case '\n':
                str += '\\';
                str += 'n';
                break;
            default:
                str += c;
            }
        }
        str += "\"";
        return str;
    }
    return m_str;
}