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
            assert(binds->size() == exprs->size());
            for(size_t i = 0; i < binds->size(); i++) {
                auto key = binds->at(i)->as_symbol();
                auto val = exprs->at(i);
                set(key, val);
            }

        }

    // defining three methods for the env object

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
    Env *m_outer { nullptr };
    unordered_map<const SymbolValue *, Value *, EnvHash, EnvComparator> m_data;
};