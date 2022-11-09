#include "types.hpp"

using namespace std;

//builds string out of list (for printing)
string ListValue::inspect() {
    string out = "(";

    for(auto *value : m_list) {
        out.append(value->inspect());
        out.append(" ");
    }

    if(m_list.size() > 0) {
        out[out.length() - 1] = ')';
    } else {
        out.append(")");
    }


    return out;
}