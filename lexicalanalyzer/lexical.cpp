/*
 * lexical.cpp — Lexer implementation
 *
 * Scans the input character-by-character and groups characters into
 * tokens.  The lexer recognises:
 *   • SQL keywords  (SELECT, FROM, WHERE, AND, OR, INSERT, INTO,
 *                     VALUES, DELETE, UPDATE, SET)
 *   • Identifiers   (table and column names)
 *   • Integers      (e.g. 20)
 *   • String literals(e.g. 'Alice')
 *   • Operators     (=, !=, <, >, <=, >=)
 *   • Punctuation   (, ; * ( ))
 */

#include "lexical.h"
#include <cctype>
#include <set>
using namespace std;

// ── SQL keywords (stored in uppercase) ──────────────────────────────
static const set<string> KEYWORDS = {
    "SELECT", "FROM", "WHERE", "AND", "OR",
    "INSERT", "INTO", "VALUES",
    "DELETE", "UPDATE", "SET"
};

// ── Helper: is this word a keyword? ─────────────────────────────────
bool Lexer::isKeyword(const string& word) const {
    return KEYWORDS.count(toUpper(word)) > 0;
}

// ── Helper: convert string to upper case ────────────────────────────
string Lexer::toUpper(const string& s) const {
    string result = s;
    for (auto& c : result) c = static_cast<char>(toupper(c));
    return result;
}

// ── Main tokenizer ──────────────────────────────────────────────────
vector<Token> Lexer::tokenize(const string& query) {
    vector<Token> tokens;
    size_t i = 0;
    size_t len = query.length();

    while (i < len) {
        // ── Skip whitespace ─────────────────────────────────────────
        if (isspace(query[i])) {
            ++i;
            continue;
        }

        // ── Single-character punctuation ────────────────────────────
        if (query[i] == ',') { tokens.emplace_back(TokenType::COMMA,            ","); ++i; continue; }
        if (query[i] == ';') { tokens.emplace_back(TokenType::SEMICOLON,        ";"); ++i; continue; }
        if (query[i] == '*') { tokens.emplace_back(TokenType::STAR,             "*"); ++i; continue; }
        if (query[i] == '(') { tokens.emplace_back(TokenType::LEFTPARENTHISIS,  "("); ++i; continue; }
        if (query[i] == ')') { tokens.emplace_back(TokenType::RIGHTPARENTHISIS, ")"); ++i; continue; }

        // ── Two-character operators (!= <= >=) ──────────────────────
        if (query[i] == '!' && i + 1 < len && query[i + 1] == '=') {
            tokens.emplace_back(TokenType::OPERATOR, "!=");
            i += 2; continue;
        }
        if (query[i] == '<' && i + 1 < len && query[i + 1] == '=') {
            tokens.emplace_back(TokenType::OPERATOR, "<=");
            i += 2; continue;
        }
        if (query[i] == '>' && i + 1 < len && query[i + 1] == '=') {
            tokens.emplace_back(TokenType::OPERATOR, ">=");
            i += 2; continue;
        }

        // ── Single-character operators (= < >) ─────────────────────
        if (query[i] == '=') { tokens.emplace_back(TokenType::OPERATOR, "="); ++i; continue; }
        if (query[i] == '<') { tokens.emplace_back(TokenType::OPERATOR, "<"); ++i; continue; }
        if (query[i] == '>') { tokens.emplace_back(TokenType::OPERATOR, ">"); ++i; continue; }

        // ── String literal (single-quoted) ──────────────────────────
        if (query[i] == '\'') {
            ++i; // skip opening quote
            std::string strLit;
            while (i < len && query[i] != '\'') {
                strLit += query[i];
                ++i;
            }
            if (i < len) ++i; // skip closing quote
            tokens.emplace_back(TokenType::STRING_LITERAL, strLit);
            continue;
        }

        // ── Integer literal ─────────────────────────────────────────
        if (std::isdigit(query[i])) {
            std::string num;
            while (i < len && std::isdigit(query[i])) {
                num += query[i];
                ++i;
            }
            tokens.emplace_back(TokenType::INTEGER_LITERAL, num);
            continue;
        }

        // ── Identifier or keyword ───────────────────────────────────
        if (std::isalpha(query[i]) || query[i] == '_') {
            std::string word;
            while (i < len && (std::isalnum(query[i]) || query[i] == '_')) {
                word += query[i];
                ++i;
            }
            if (isKeyword(word)) {
                tokens.emplace_back(TokenType::KEYWORD, toUpper(word));
            } else {
                tokens.emplace_back(TokenType::IDENTIFIER, word);
            }
            continue;
        }

        // ── Unknown character — skip ────────────────────────────────
        ++i;
    }

    // Always end with an END_OF_INPUT sentinel
    tokens.emplace_back(TokenType::END_OF_INPUT, "");
    return tokens;
}
