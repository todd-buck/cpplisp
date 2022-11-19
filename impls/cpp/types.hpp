#pragma once

#include <string>
#include <string_view>
#include <functional>
#include <vector>
#include <cassert>

using namespace std;

class ListValue;
class SymbolValue;
class IntegerValue;
class FunctionValue;
class ExceptionValue;
class TrueValue;
class FalseValue;
class NilValue;
class StringValue;
class KeywordValue;

class Value {
public:
    enum class Type {
        List,
        Symbol,
        Integer,
        Function,
        Exception,
        True,
        False,
        Nil,
        String,
        Keyword,
    };

    virtual Type type() const = 0;
    virtual string inspect(bool print_readably = false) const = 0;

    virtual bool is_symbol() const {return false;}
    virtual bool is_nil() const { return false; }
    virtual bool is_false() const { return false; }
    virtual bool is_true() const { return false; }
    virtual bool is_truthy() const { return true; }
    virtual bool is_list() const { return false; }
    virtual bool is_integer() const { return false; }
    virtual bool is_exception() const { return false; }
    virtual bool is_string() const { return false; }
    virtual bool is_keyword() const { return false; }

    virtual bool operator==(const Value *other) const { return this == other; }
    bool operator!=(const Value *other) const { return !(*this == other); }

    ListValue *as_list();
    SymbolValue *as_symbol();
    IntegerValue *as_integer();
    FunctionValue *as_function();
    ExceptionValue *as_exception();
    TrueValue *as_true();
    FalseValue *as_false();
    NilValue *as_nil();
    StringValue *as_string();
    KeywordValue *as_keyword();

    const ListValue *as_list() const;
    const SymbolValue *as_symbol() const;
    const IntegerValue *as_integer() const;
    const FunctionValue *as_function() const;
    const ExceptionValue *as_exception() const;
    const TrueValue *as_true() const;
    const FalseValue *as_false() const;
    const NilValue *as_nil() const;
    const StringValue *as_string() const;
    const KeywordValue *as_keyword() const;
    
};

//stores list in vectors
class ListValue : public Value {
public:
    ListValue() {}

    void push(Value *value) {
        m_list.push_back(value);
    }

    virtual Type type() const override {return Type::List;}
    virtual string inspect(bool print_readably = false) const override;
    virtual bool is_list() const override { return true; }

    virtual bool operator==(const Value *) const override;

    auto begin() {return m_list.begin();}
    auto end() {return m_list.end();}

    bool is_empty() {
        return m_list.size() == 0;
    }

    size_t size() const {
        return m_list.size();
    }

    Value *at(size_t index) const {
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
    virtual string inspect(bool) const override {
        return str();
    }

    bool operator==(const Value *other) const override {
        return other->is_symbol() && const_cast<Value *>(other)->as_symbol()->m_str == m_str;    
    }

    virtual bool is_symbol() const override {return true;}

private:
    string m_str;
};

class IntegerValue : public Value {
public:
    IntegerValue(long l) : m_long{l} {}
    
    virtual Type type() const override {return Type::Integer;}
    
    virtual string inspect(bool) const override {
        return to_string(m_long);
    }

    virtual bool is_integer() const override { return true; }
    
    bool operator==(const Value *other) const override { 
        return other-> is_integer() && const_cast<Value *>(other)->as_integer()->m_long == m_long;
    }

    long to_long() { return m_long; }

private:
    long m_long{0};
};

using Function = function<Value *(size_t, Value **)>;

class FunctionValue : public Value {
public:
    FunctionValue(Function function) : m_function{function} {}

    Function to_function() { return m_function; }

    bool operator==(const Value *other) const override {
        return other == this;
    }

    virtual Type type() const override { return Type::Function; }

    virtual string inspect(bool) const override {
        return "#<function>";
    }

private:
    Function m_function {nullptr};
};

class ExceptionValue : public Value {
public:
    ExceptionValue(string message) : m_message{message} {}

    virtual Type type() const override {return Type::Exception;}

    virtual string inspect(bool) const override {
        return "<exception" + m_message + ">";
    }

    virtual bool is_exception() const override { return true; }

    bool operator==(const Value *other) const override {
        return other->is_exception() && const_cast<Value *>(other)->as_exception()->m_message == m_message;
    }

    const string &message() {return m_message;}

private:
    string m_message;
};

class TrueValue : public Value {
public:
    static TrueValue *the() {
        if (!s_instance)
            s_instance = new TrueValue;
        return s_instance;
    }

    virtual Type type() const override {return Type::True;}

    virtual string inspect(bool) const override {
        return "true";
    }

    virtual bool is_true() const override { return true; }

private:
    TrueValue() { }

    static inline TrueValue *s_instance { nullptr };
};

class FalseValue : public Value {
public:
    static FalseValue *the() {
        if (!s_instance)
            s_instance = new FalseValue;
        return s_instance;
    }

    virtual Type type() const override {return Type::False;}

    virtual string inspect(bool) const override {
        return "false";
    }

    virtual bool is_false() const override { return true; }
    virtual bool is_truthy() const override { return false; }

private:
    FalseValue() { }

    static inline FalseValue *s_instance { nullptr };
};

class NilValue : public Value {
public:
    static NilValue *the() {
        if (!s_instance)
            s_instance = new NilValue;
        return s_instance;
    }

    virtual Type type() const override {return Type::Nil;}

    virtual string inspect(bool) const override {
        return "nil";
    }

    virtual bool is_nil() const override { return true; }

    virtual bool is_truthy() const override { return false; }
private:
    NilValue() { }

    static inline NilValue *s_instance { nullptr };
};

class StringValue : public Value {
public:
    StringValue(string_view str)
        : m_str { str } { }

    string str() const { return m_str; }

    virtual Type type() const override { return Type::String; }
    virtual bool is_string() const override { return true; }

    bool operator==(const Value *other) const override {
        return other->is_string() && other->as_string()->m_str == m_str;
    }

    virtual string inspect(bool print_readably = false) const override;

private:
    string m_str;
};

class KeywordValue : public Value {
public:
    KeywordValue(string_view str)
        : m_str { str } { }

    string str() const { return m_str; }

    virtual Type type() const override { return Type::Keyword; }
    virtual bool is_keyword() const override { return true; }

    bool operator==(const Value *other) const override {
        return other->is_keyword() && other->as_keyword()->m_str == m_str;
    }

    virtual string inspect(bool) const override {
        return m_str;
    }

private:
    string m_str;
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
