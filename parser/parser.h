/*
 * parser.h — Recursive Descent Parser interface
 *
 * The Parser is the second phase of the pipeline.  It consumes the
 * token stream produced by the Lexer and builds an Abstract Syntax
 * Tree (AST) that represents the structure of the SQL query.
 *
 * The parser is LL(1)-compatible and uses one function per CFG rule:
 *
 *   CFG Rule           → Parser Method
 *   ────────────────────────────────────
 *   Query              → parseQuery()
 *   SelectStmt         → parseSelect()
 *   ColumnList         → parseColumnList()
 *   WhereClause        → parseWhereClause()
 *   Condition          → parseCondition()
 */

#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <stdexcept>
#include "../common/token.h"
#include "../common/ast.h"

class Parser {
public:
    // Parse the token stream and return the root of the AST.
    // Throws std::runtime_error on syntax errors.
    ASTNode* parse(const std::vector<Token>& tokens);

private:
    std::vector<Token> tokens_;  // token stream
    size_t             pos_;     // current position in token stream

    // ── Utility helpers ─────────────────────────────────────────────
    Token currentToken() const;
    Token peek() const;
    Token advance();
    Token expect(TokenType type, const std::string& errMsg);
    Token expectKeyword(const std::string& keyword, const std::string& errMsg);
    bool  isKeyword(const std::string& keyword) const;

    // ── CFG rule functions ──────────────────────────────────────────
    ASTNode* parseQuery();       // Query → SelectStmt
    ASTNode* parseSelect();      // SelectStmt → SELECT ColumnList FROM ID [WhereClause]
    ASTNode* parseColumnList();  // ColumnList → '*' | ID (',' ID)*
    ASTNode* parseWhereClause(); // WhereClause → WHERE Condition (AND Condition)*
    ASTNode* parseCondition();   // Condition → ID Op Value
};

#endif // PARSER_H
