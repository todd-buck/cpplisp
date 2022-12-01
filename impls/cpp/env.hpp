#pragma once

// file created to hold the environment definition

#include <unordered_map>

#include "types.hpp"

// define env object that is instantiated with a single
// "outer" parameter and starts with an empty associative
// data structure property "data"
class Env {
public:
    Env(Env *outer)
        : m_outer(outer) { }

    Env(Env *outer, ListValue *binds, ListValue *exprs)
        : m_outer(outer) { 
            set_binds(binds, exprs);

        }

    // defining three methods for the env object

    // used to implement set
    // SET: takes a symbol key and a mal value and adds to the data structure
    void set(SymbolValue *key, Value *val) {
        m_data[key] = val;
    }

    // FIND: takes a symbol key and if the current environment contains
    // that key then return the environment. If no key if found
    // and outer is not nil then call find() (recursive) on the outer environment.
    Env *find(const SymbolValue *key) const {
        auto search = m_data.find(key);
        if (search != m_data.end())
            return const_cast<Env *>(this);
        else if (m_outer)
            return m_outer->find(key);
        return nullptr;
    }

    // GET: takes a symbol key and uses the find() method to locate the environment
    // with the key, then returns the mathing value. If no key is found
    // up the outer chain, then throws a "not found" error.
    Value *get(const SymbolValue *key) const {
        auto env = find(key);
        if (!env)
            throw new ExceptionValue { key->str() + " not found" };
        return env->m_data[key];
    }

private:
    void set_binds(ListValue *binds, ListValue *exprs) {
        for (size_t i = 0; i < binds->size(); ++i) {
            auto key = binds->at(i)->as_symbol();
            if (key->matches("&")) {
                if (i + 1 >= binds->size())
                    throw new ExceptionValue { "missing symbol after &" };
                key = binds->at(i + 1)->as_symbol();
                set_binds_rest(key, exprs, i);
                return;
            }
            if (i >= exprs->size())
                throw new ExceptionValue { "not enough arguments" };
            auto val = exprs->at(i);
            set(key, val);
        }
    }

    void set_binds_rest(SymbolValue *key, ListValue *exprs, size_t starting_index) {
        auto vals = new ListValue;
        for (size_t i = starting_index; i < exprs->size(); ++i) {
            vals->push(exprs->at(i));
        }
        set(key, vals);
    }

    Env *m_outer { nullptr };
    unordered_map<const SymbolValue *, Value *, EnvHash, EnvComparator> m_data;
};