#pragma once

#include <unordered_map>
#include "types.hpp"

using namespace std;

unordered_map<string, Function> build_namespace ();

Value *add(size_t argc, Value **args);
Value *subtract(size_t argc, Value **args);
Value *multiply(size_t argc, Value **args);
Value *divide(size_t argc, Value **args);
Value *prn(size_t argc, Value **args);
Value *list(size_t argc, Value **args);
Value *list_q(size_t argc, Value **args);
Value *empty_q(size_t argc, Value **args);
Value *cnt(size_t argc, Value **args);
Value *eq(size_t argc, Value **args);
Value *lt(size_t argc, Value **args);
Value *lte(size_t argc, Value **args);
Value *gt(size_t argc, Value **args);
Value *gte(size_t argc, Value **args);

// new additions
Value *number_q(size_t argc, Value **args);
Value *symbol_q(size_t argc, Value **args);
Value *nil_q(size_t argc, Value **args);
Value *cons(size_t argc, Value **args);
Value *car(size_t argc, Value **args);
Value *cdr(size_t argc, Value **args);
Value *and_q(size_t argc, Value **args);
Value *or_q(size_t argc, Value **args);
