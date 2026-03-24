/*
 * lexical.h — Lexer interface
 *
 * The Lexer (also called the Scanner or Tokenizer) is the first phase
 * of the query-processing pipeline. It breaks a raw SQL string into a
 * sequence of Tokens that the Parser can consume.
 */

#ifndef LEXICAL_H
#define LEXICAL_H

#include <vector>
#include <string>
using namespace std;

#include "../common/token.h"

class Lexer {
public:
    // Tokenize the input SQL query string and return a vector of tokens.
    // The last token in the vector is always END_OF_INPUT.
    vector<Token> tokenize(const string& query);

private:
    // Check whether a word is a SQL keyword (case-insensitive).
    bool isKeyword(const string& word) const;

    // Convert a string to uppercase (used for keyword matching).
    string toUpper(const string& s) const;
};

#endif // LEXICAL_H
