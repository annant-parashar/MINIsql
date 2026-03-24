/*
 * parser.cpp — Recursive Descent Parser implementation
 *
 * Implements an LL(1) recursive-descent parser for a subset of SQL.
 * Each public / private method corresponds to one production in the
 * context-free grammar (CFG):
 *
 *   Query        → SelectStmt
 *   SelectStmt   → SELECT ColumnList FROM IDENTIFIER WhereClause
 *   ColumnList   → '*' | IDENTIFIER (',' IDENTIFIER)*
 *   WhereClause  → ε  |  WHERE Condition (AND Condition)*
 *   Condition    → IDENTIFIER Operator Value
 *   Operator     → '=' | '!=' | '<' | '>' | '<=' | '>='
 *   Value        → INTEGER_LITERAL | STRING_LITERAL
 */

#include "parser.h"
#include <iostream>

// =====================================================================
// Utility helpers
// =====================================================================

// Return the token at the current position.
Token Parser::currentToken() const {
    if (pos_ < tokens_.size()) return tokens_[pos_];
    return Token(TokenType::END_OF_INPUT, "");
}

// Same as currentToken() — peek without consuming.
Token Parser::peek() const {
    return currentToken();
}

// Consume and return the current token, then advance the position.
Token Parser::advance() {
    Token t = currentToken();
    if (pos_ < tokens_.size()) ++pos_;
    return t;
}

// Consume the current token if its type matches; otherwise throw.
Token Parser::expect(TokenType type, const std::string& errMsg) {
    Token t = currentToken();
    if (t.type != type) {
        throw std::runtime_error("Syntax Error: " + errMsg +
                                 " (got '" + t.value + "')");
    }
    return advance();
}

// Consume the current token if it is a keyword with the given value.
Token Parser::expectKeyword(const std::string& keyword,
                            const std::string& errMsg) {
    Token t = currentToken();
    if (t.type != TokenType::KEYWORD || t.value != keyword) {
        throw std::runtime_error("Syntax Error: " + errMsg +
                                 " (got '" + t.value + "')");
    }
    return advance();
}

// Check whether the current token is a keyword with the given value.
bool Parser::isKeyword(const std::string& keyword) const {
    Token t = currentToken();
    return t.type == TokenType::KEYWORD && t.value == keyword;
}

// =====================================================================
// Public entry point
// =====================================================================

ASTNode* Parser::parse(const std::vector<Token>& tokens) {
    tokens_ = tokens;
    pos_    = 0;
    return parseQuery();
}

// =====================================================================
// CFG rule: Query → SelectStmt
// =====================================================================
ASTNode* Parser::parseQuery() {
    // For now we only support SELECT statements.
    if (isKeyword("SELECT")) {
        return parseSelect();
    }
    throw std::runtime_error("Syntax Error: expected SELECT statement");
}

// =====================================================================
// CFG rule: SelectStmt → SELECT ColumnList FROM IDENTIFIER WhereClause
// =====================================================================
ASTNode* Parser::parseSelect() {
    ASTNode* node = createNode("SELECT_STMT");

    // SELECT
    expectKeyword("SELECT", "expected SELECT");

    // ColumnList
    node->children.push_back(parseColumnList());

    // FROM
    expectKeyword("FROM", "expected FROM after column list");

    // Table name (IDENTIFIER)
    Token tableTok = expect(TokenType::IDENTIFIER, "expected table name after FROM");
    node->children.push_back(createNode("TABLE", tableTok.value));

    // Optional WHERE clause
    if (isKeyword("WHERE")) {
        node->children.push_back(parseWhereClause());
    }

    // Optional semicolon at the end
    if (currentToken().type == TokenType::SEMICOLON) {
        advance();
    }

    return node;
}

// =====================================================================
// CFG rule: ColumnList → '*' | IDENTIFIER (',' IDENTIFIER)*
// =====================================================================
ASTNode* Parser::parseColumnList() {
    ASTNode* node = createNode("COLUMN_LIST");

    // Case 1: SELECT *
    if (currentToken().type == TokenType::STAR) {
        advance();
        node->children.push_back(createNode("COLUMN", "*"));
        return node;
    }

    // Case 2: one or more column names separated by commas
    Token col = expect(TokenType::IDENTIFIER, "expected column name");
    node->children.push_back(createNode("COLUMN", col.value));

    while (currentToken().type == TokenType::COMMA) {
        advance(); // consume ','
        col = expect(TokenType::IDENTIFIER, "expected column name after ','");
        node->children.push_back(createNode("COLUMN", col.value));
    }

    return node;
}

// =====================================================================
// CFG rule: WhereClause → WHERE Condition (AND Condition)*
// =====================================================================
ASTNode* Parser::parseWhereClause() {
    ASTNode* node = createNode("WHERE_CLAUSE");

    expectKeyword("WHERE", "expected WHERE");

    // First condition
    node->children.push_back(parseCondition());

    // Additional conditions joined by AND
    while (isKeyword("AND")) {
        advance(); // consume AND
        node->children.push_back(parseCondition());
    }

    return node;
}

// =====================================================================
// CFG rule: Condition → IDENTIFIER Operator Value
// =====================================================================
ASTNode* Parser::parseCondition() {
    ASTNode* node = createNode("CONDITION");

    // Column name
    Token colTok = expect(TokenType::IDENTIFIER, "expected column name in condition");
    node->children.push_back(createNode("COLUMN", colTok.value));

    // Operator
    Token opTok = expect(TokenType::OPERATOR, "expected operator (=, !=, <, >, <=, >=)");
    node->children.push_back(createNode("OPERATOR", opTok.value));

    // Value (integer or string literal)
    Token valTok = currentToken();
    if (valTok.type == TokenType::INTEGER_LITERAL ||
        valTok.type == TokenType::STRING_LITERAL) {
        advance();
        node->children.push_back(createNode("VALUE", valTok.value));
    } else {
        throw std::runtime_error("Syntax Error: expected a value (integer or string)");
    }

    return node;
}
