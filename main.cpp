/*
 * main.cpp — Mini SQL Query Engine entry point
 *
 * This program ties together all five compiler-design phases:
 *
 *   1. Lexical Analysis   — break query string into tokens
 *   2. Syntax Analysis    — parse tokens into an AST
 *   3. Semantic Analysis  — validate against schema
 *   4. Query Optimization — produce an execution plan
 *   5. Query Execution    — fetch and display results
 *
 * Compilation (g++ / MinGW):
 *   g++ main.cpp lexicalanalyzer/lexical.cpp 
 *       parser/parser.cpp  
 *       semantic/semantic.cpp 
 *       optimizer/optimizer.cpp 
 *       executor/executor.cpp
 *
 * Usage:
 *   .\a.exe
 *
 * Sample queries to try:
 *   SELECT name FROM students WHERE age = 20;
 *   SELECT name, grade FROM students WHERE grade = 'A';
 *   SELECT * FROM students;
 *   SELECT name FROM students WHERE age > 20 AND grade = 'A';
 */

#include <bits/stdc++.h>
//#include <string>
//#include <vector>
using namespace std;

#include "common/token.h"
#include "common/ast.h"
#include "lexicalanalyzer/lexical.h"
#include "parser/parser.h"

// ── Path to the data directory (relative to the working directory) ──
static const string DATA_DIR = "data";

// ── Pretty header helpers ───────────────────────────────────────────
static void printHeader(const string& title) {
    string bar(60, '=');
    cout << "\n" << bar << "\n";
    cout << "  " << title << "\n";
    cout << bar << "\n";
}

static void printSubHeader(const string& title) {
    string bar(60, '-');
    cout << "\n" << bar << "\n";
    cout << "  " << title << "\n";
    cout << bar << "\n";
}

// ── Process a single query through all five phases ──────────────────
static void processQuery(const string& query) {

    //printHeader("MINI SQL QUERY ENGINE");
    std::cout << "  Query: " << query << "\n";

    // ================================================================
    // PHASE 1 — Lexical Analysis
    // ================================================================
    printSubHeader("Phase 1: Lexical Analysis (Tokenization)");

    Lexer lexer;
    std::vector<Token> tokens = lexer.tokenize(query);

    std::cout << "\n  Tokens produced:\n";
    std::cout << "  " << std::string(44, '-') << "\n";
    std::cout << "  | " << std::left;
    std::cout.width(20); std::cout << "Type";
    std::cout << "| ";
    std::cout.width(20); std::cout << "Value";
    std::cout << "|\n";
    std::cout << "  " << std::string(44, '-') << "\n";
    for (const auto& t : tokens) {
        std::cout << "  | ";
        std::cout.width(20); std::cout << std::left << tokenTypeName(t.type);
        std::cout << "| ";
        std::cout.width(20); std::cout << std::left << (t.value.empty() ? "(empty)" : t.value);
        std::cout << "|\n";
    }
    std::cout << "  " << std::string(44, '-') << "\n";

    // ================================================================
    // PHASE 2 — Syntax Analysis (Parsing)
    // ================================================================
    printSubHeader("Phase 2: Syntax Analysis (Parsing → AST)");

    Parser parser;
    ASTNode* ast = nullptr;
    try {
        ast = parser.parse(tokens);
    } catch (const std::runtime_error& e) {
        std::cerr << "\n  [ERROR] " << e.what() << "\n";
        return;
    }

    std::cout << "\n  Parse Tree (AST):\n\n";
    printTree(ast, "  ");

    // Cleanup
    delete ast;

    //std::cout << "\n" << std::string(60, '=') << "\n";
}

// ── Main ─────────────────────────────────────────────────────────────
int main() {
    cout << R"(

  Type a SQL query and press Enter. 
  Type 'exit' to quit.

)" << endl;

    string line;
    while (true) {
        cout << "sql> ";
        if (!getline(cin, line)) break;

        // Trim the input
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == string::npos) continue;
        line = line.substr(start);
        size_t end = line.find_last_not_of(" \t\r\n");
        line = line.substr(0, end + 1);

        if (line.empty()) continue;
        if (line == "exit" || line == "quit") {
            std::cout << "Thank You for using MiniSQL\n";
            break;
        }

        processQuery(line);
        std::cout << std::endl;
    }

    return 0;
}
