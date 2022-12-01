#include "printer.hpp"

string print_string(Value *value, bool print_readably) {
    return value->inspect(print_readably);
}