#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <cassert>

using namespace std;

class Value {
    public:
        virtual string inspect() { assert(0); }
};

//stores list in vectors
class ListValue : public Value {
    public:
        ListValue() {}

        void push(Value *value) {
            m_list.push_back(value);
        }

        virtual string inspect();

    private:
        vector<Value *> m_list {};
};

//stores symbols as strings
class SymbolValue : public Value {
    public:
        SymbolValue(string_view str) 
            : m_str { str } {}

        string str() { return m_str; }

        //for printing symbol
        virtual string inspect() {
            return str();
        }
        
    private:
        string m_str;
};

