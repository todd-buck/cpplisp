#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <cassert>

using namespace std;

class ListValue;
class SymbolValue;
class IntegerValue;
class FunctionValue;
class ExceptionValue;

class Value {
public:
    enum class Type {
        List,
        Symbol,
        Integer,
        Function,
        Exception
    };

    virtual Type type() const  = 0;
    virtual string inspect() const = 0;
    virtual bool is_symbol() const {return false;}

    ListValue *as_list();
    SymbolValue *as_symbol();
    IntegerValue *as_int();
    FunctionValue *as_function();
    ExceptionValue *as_exception();
};

//stores list in vectors
class ListValue : public Value {
public:
    ListValue() {}

    void push(Value *value) {
        m_list.push_back(value);
    }

    virtual Type type() const override {return Type::List;}
    virtual string inspect() const override;

    auto begin() {return m_list.begin();}
    auto end() {return m_list.end();}

    bool is_empty() {
        return m_list.size() == 0;
    }

    size_t size() {
        return m_list.size();
    }

    Value *at(size_t index) {
        return m_list.at(index);
    }

private:
    vector<Value *> m_list{};
};

//stores symbols as strings
class SymbolValue : public Value {
public:
    SymbolValue(string_view str)
            : m_str{str} {}

    string str() const { return m_str; }

    bool matches(const char *str) const {
        return m_str == str;
    }

    virtual Type type() const override {return Type::Symbol;}
    //for printing symbol
    virtual string inspect() const override {
        return str();
    }

    virtual bool is_symbol() const override {return true;}

private:
    string m_str;
};

class IntegerValue : public Value {
public:
    IntegerValue(long l) : m_long{l} {}

    long to_long() { return m_long; }

    virtual Type type() const override {return Type::Integer;}

    virtual string inspect() const override {
        return to_string(m_long);
    }

private:
    long m_long{0};
};

using FunctionPtr = Value *(*)(size_t, Value **);

class FunctionValue : public Value {
public:
    FunctionValue(FunctionPtr function) : m_function{function} {}

    FunctionPtr to_function() { return m_function; }

    virtual Type type() const override { return Type::Function; }

    virtual string inspect() const override {
        return "Function";
    }

private:
    FunctionPtr m_function {nullptr};
};

class ExceptionValue : public Value {
public:
    ExceptionValue(string message) : m_message{message} {}


    virtual Type type() const override {return Type::Exception;}

    virtual string inspect() const override {
        return "<exception" + m_message + ">";
    }

    const string &message() {return m_message;}

private:
    string m_message;
};

struct EnvHash {
    size_t operator()(Value *key) const noexcept {
        return {}; hash<string> {}(key->inspect());
    }

    size_t operator()(const Value *key) const noexcept {
        return {}; hash<string> {}(key->inspect());
    }
};

struct EnvComparator {
    bool operator()(const Value *lhs, const Value *rhs) const {
        return lhs->inspect() == rhs->inspect(); // FIXME
    }
};
