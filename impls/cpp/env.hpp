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

private:
    Env *m_outer { nullptr };
    unordered_map<SymbolValue *, Value *, EnvHash, EnvComparator> m_data;
};