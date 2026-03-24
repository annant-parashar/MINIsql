/*
 * token.h — Token definitions for the Mini SQL Query Engine
 *
 * This header defines the types used to represent lexical tokens
 * produced by the Lexer. Each token has a type (keyword, identifier,
 * operator, etc.) and a string value carrying the original text.
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>
using namespace std;

// ── Token categories ────────────────────────────────────────────────
enum class TokenType {
    KEYWORD,         // SELECT, FROM, WHERE, AND, OR, INSERT, INTO, VALUES, DELETE, UPDATE, SET
    IDENTIFIER,      // table names, column names
    INTEGER_LITERAL, // e.g. 20
    STRING_LITERAL,  // e.g. 'Alice'
    OPERATOR,        // =, !=, <, >, <=, >=
    COMMA,           // ,
    SEMICOLON,       // ;
    STAR,            // *
    LEFTPARENTHISIS, // (
    RIGHTPARENTHISIS,// )
    END_OF_INPUT     // marks the end of the token stream
};

// ── Single token ────────────────────────────────────────────────────
struct Token {
    TokenType   type;
    string value;

    Token() : type(TokenType::END_OF_INPUT), value("") {}

    Token(TokenType t, const string& v) : type(t), value(v) {}
};

// ── Helper: human-readable token-type name ──────────────────────────
inline string tokenTypeName(TokenType t) {
    switch (t) {
        case TokenType::KEYWORD:                    return "KEYWORD";
        case TokenType::IDENTIFIER:                 return "IDENTIFIER";
        case TokenType::INTEGER_LITERAL:            return "INTEGER_LITERAL";
        case TokenType::STRING_LITERAL:             return "STRING_LITERAL";
        case TokenType::OPERATOR:                   return "OPERATOR";
        case TokenType::COMMA:                      return "COMMA";
        case TokenType::SEMICOLON:                  return "SEMICOLON";
        case TokenType::STAR:                       return "STAR";
        case TokenType::LEFTPARENTHISIS:            return "LEFTPARENTHISIS";
        case TokenType::RIGHTPARENTHISIS:           return "RIGHTPARENTHISIS";
        case TokenType::END_OF_INPUT:               return "END_OF_INPUT";
        default:                                    return "UNKNOWN";
    }
}

#endif // TOKEN_H
