#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <optional>
#include <vector>

#include "types.hpp"

using namespace std;

struct Tokenizer {
    public:
        Tokenizer(string &input)
            : m_input { input } { }

        optional<string_view> next() {
            auto view = string_view(m_input);
            while(m_index < m_input.length()) {
                char c = m_input.at(m_index);

                switch(c) {
                    //skips whitespace
                    case ' ':
                    case '\t':
                    case '\n':
                    case ',':
                        ++m_index;
                        break;

                    //TODO: SHOULD BE REMOVED BASED ON YESSICK'S LISP DOCUMENTATION 
                    case '~': {
                        ++m_index;
                        if(m_index < m_input.length() && m_input.at(m_index) == '@') {
                            ++m_index;
                            return view.substr(m_index - 2, 2);
                        }
                        return view.substr(m_index - 1, 1);
                    }

                    //single-characters
                    case ']':
                    case '[':
                    case '}':
                    case '{':
                    case ')':
                    case '(':
                    case '\'':
                    case '`':
                    case '^':
                    case '@': 
                        return view.substr(m_index++, 1);

                    //strings
                    case '"': {
                        size_t start = m_index;
                        ++m_index;

                        while(m_index < m_input.length()) {
                            char c = m_input.at(m_index);

                            switch(c) {
                                //end of successful string
                                case '"' : {
                                    ++m_index;
                                    return view.substr(start, m_index - start);
                                }
                                
                                //ignores double-quote if it has a preceding backslash (i.e. string string_with_quotes = "\"this is a string with a quote\"";)
                                case '\\': {
                                    ++m_index;
                                    break;
                                }
                            }
                            ++m_index;
                        }
                        cout << "EOF\n";
                        return view.substr(start, m_index - start);
                    }

                    //comments
                    case ';': {
                        size_t start = m_index;
                        while(m_index < m_input.length()) {
                            c = m_input.at(m_index);
                            if(c == '\n') break;
                            ++m_index;
                        }
                        
                        return view.substr(start, m_index - start);
                    }

                    case '-':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9': {
                        //reading entire number sequence
                        size_t start = m_index;
                        bool done = false;
                        ++m_index;
                        while (!done && m_index < m_input.length()) {
                            c = m_input.at(m_index);
                            switch (c) {
                            case '0':
                            case '1':
                            case '2':
                            case '3':
                            case '4':
                            case '5':
                            case '6':
                            case '7':
                            case '8':
                            case '9':
                                ++m_index;
                                break;
                            default:
                                done = true;
                                break;
                            }
                        }
                        if (m_index - start == 1 && view.at(start) == '-') {
                            --m_index;
                            return tokenize_symbol();
                        }
                        return view.substr(start, m_index - start);
                    }

                    default: {
                        return tokenize_symbol();
                    }
                }
            }
            return {};
        }

    private:
        string &m_input;
        size_t m_index { 0 };

        string_view tokenize_symbol() {
            auto view = std::string_view(m_input);
            size_t start = m_index;
            bool done = false;
            char c;
            while (!done && m_index < m_input.length()) {
                c = m_input.at(m_index);
                switch (c) {
                case ' ':
                case '\t':
                case '\n':
                case '[':
                case ']':
                case '{':
                case '}':
                case '(':
                case ')':
                case '\'':
                case '"':
                case '`':
                case ',':
                case ';':
                    done = true;
                    break;
                default:
                    ++m_index;
                }
            }
            return view.substr(start, m_index - start);
        }

};

class Reader {
    public:
        Reader(vector<string_view> &tokens)
            : m_tokens { tokens } { }

        optional<string_view> peek() {
            if(m_index < m_tokens.size()) {
                return m_tokens.at(m_index);
            }

            return {};
        }

        optional<string_view> next() {
            if(m_index < m_tokens.size()) {
                return m_tokens.at(m_index++);
            }

            return {};
        }

    private:
        vector<string_view> &m_tokens;
        size_t m_index { 0 };

};


vector<string_view> tokenize(string &input);

Value *read_form(Reader &reader);
Value *read_str(string &input);
Value *read_integer(Reader &reader);
Value *read_string(Reader &reader);
ListValue *read_list(Reader &reader);
Value *read_atom(Reader &reader);

