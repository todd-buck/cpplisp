#include "printer.hpp"

string print_string(Value *value) {
    return value->inspect();
}