/*
 * ast.h — Abstract Syntax Tree (AST) node definition
 *
 * The AST is built by the Parser and consumed by later phases.
 * Each node has a descriptive type string, an optional value,
 * and zero or more children, forming a tree.
 *
 * Two printing helpers are provided:
 *   printTree()          — top-down visual tree (root at top, branches down)
 *   printTreeIndented()  — classic indented format with +-- connectors
 */

#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <functional>
#include <algorithm>

// ── AST node ────────────────────────────────────────────────────────
struct ASTNode {
    std::string              type;      // e.g. "SELECT_STMT", "COLUMN_LIST"
    std::string              value;     // e.g. column name, literal value
    std::string              annotation;// filled in by Semantic Analyzer
    std::vector<ASTNode*>    children;

    ASTNode(const std::string& t, const std::string& v = "")
        : type(t), value(v) {}

    ~ASTNode() {
        for (auto* child : children) delete child;
    }
};

// ── Factory helper ──────────────────────────────────────────────────
inline ASTNode* createNode(const std::string& type,
                           const std::string& value = "") {
    return new ASTNode(type, value);
}

// ── Indented tree (classic format) ──────────────────────────────────
inline void printTreeIndented(const ASTNode* node,
                              const std::string& prefix = "",
                              bool isLast = true) {
    if (!node) return;
    std::cout << prefix << (isLast ? "+-- " : "|-- ");
    std::cout << node->type;
    if (!node->value.empty())      std::cout << " : " << node->value;
    if (!node->annotation.empty()) std::cout << "  [" << node->annotation << "]";
    std::cout << "\n";
    std::string cp = prefix + (isLast ? "    " : "|   ");
    for (size_t i = 0; i < node->children.size(); ++i)
        printTreeIndented(node->children[i], cp, i == node->children.size() - 1);
}

// =====================================================================
// Top-down visual tree printer
// =====================================================================
/*
 * Example output for  SELECT name FROM students WHERE age = 20;
 *
 *                              SELECT_STMT
 *                   _______________|_______________
 *                  |               |               |
 *            COLUMN_LIST    TABLE : students   WHERE_CLAUSE
 *                  |                               |
 *          COLUMN : name                       CONDITION
 *                                    ______________|______________
 *                                   |              |              |
 *                             COLUMN : age   OPERATOR : =    VALUE : 20
 */
inline void printTree(const ASTNode* root,
                      const std::string& lineIndent = "  ") {
    if (!root) return;

    // ── Get display label for a node ────────────────────────────────
    auto getLabel = [](const ASTNode* n) -> std::string {
        std::string label = n->type;
        if (!n->value.empty())      label += " : " + n->value;
        if (!n->annotation.empty()) label += " [" + n->annotation + "]";
        return label;
    };

    // ── Calculate minimum column-width needed for each subtree ──────
    //    Leaf nodes need only their label width.
    //    Internal nodes need max(labelWidth, sum-of-children + gaps).
    std::function<int(const ASTNode*)> subtreeWidth;
    subtreeWidth = [&](const ASTNode* node) -> int {
        int lw = (int)getLabel(node).size();
        if (node->children.empty()) return lw;
        int cw = 0;
        for (size_t i = 0; i < node->children.size(); i++) {
            if (i > 0) cw += 3;               // gap between sibling subtrees
            cw += subtreeWidth(node->children[i]);
        }
        return std::max(lw, cw);
    };

    // ── Assign a center-column position to every node ───────────────
    std::map<const ASTNode*, int> centerOf;
    std::vector<std::vector<const ASTNode*>> levels;

    std::function<void(const ASTNode*, int, int)> assignPos;
    assignPos = [&](const ASTNode* node, int left, int depth) {
        int w = subtreeWidth(node);
        centerOf[node] = left + w / 2;

        if ((int)levels.size() <= depth) levels.resize(depth + 1);
        levels[depth].push_back(node);

        if (!node->children.empty()) {
            // Total width occupied by all children (with gaps)
            int totalCW = 0;
            for (size_t i = 0; i < node->children.size(); i++) {
                if (i > 0) totalCW += 3;
                totalCW += subtreeWidth(node->children[i]);
            }
            // Center children within the allocated width
            int childLeft = left + std::max(0, (w - totalCW) / 2);
            for (size_t i = 0; i < node->children.size(); i++) {
                int cw = subtreeWidth(node->children[i]);
                assignPos(node->children[i], childLeft, depth + 1);
                childLeft += cw + 3;
            }
        }
    };
    assignPos(root, 0, 0);

    // ── Determine canvas width ──────────────────────────────────────
    int canvasW = 0;
    for (auto it = centerOf.begin(); it != centerOf.end(); ++it) {
        int lw = (int)getLabel(it->first).size();
        canvasW = std::max(canvasW, it->second + lw / 2 + 2);
    }
    canvasW += 5;

    // ── Utility lambdas ─────────────────────────────────────────────
    auto trimRight = [](std::string& s) {
        size_t e = s.find_last_not_of(' ');
        s = (e != std::string::npos) ? s.substr(0, e + 1) : "";
    };
    auto putChar = [&](std::string& line, int col, char ch) {
        if (col >= 0 && col < (int)line.size()) line[col] = ch;
    };
    auto putStr = [&](std::string& line, int col, const std::string& text) {
        for (int i = 0; i < (int)text.size(); i++)
            putChar(line, col + i, text[i]);
    };

    // ── Render each level ───────────────────────────────────────────
    for (size_t lvl = 0; lvl < levels.size(); lvl++) {

        // ── Line 1: node labels ─────────────────────────────────────
        std::string labelLine(canvasW, ' ');
        for (auto* node : levels[lvl]) {
            std::string label = getLabel(node);
            int start = centerOf[node] - (int)label.size() / 2;
            putStr(labelLine, start, label);
        }
        trimRight(labelLine);
        std::cout << lineIndent << labelLine << "\n";

        // ── Lines 2-3: branch connectors to children ────────────────
        bool needConn = false;
        for (auto* node : levels[lvl])
            if (!node->children.empty()) { needConn = true; break; }
        if (!needConn || lvl + 1 >= levels.size()) continue;

        // brLine: horizontal underscores + parent pipe
        std::string brLine(canvasW, ' ');
        // vdLine: vertical pipes down to each child
        std::string vdLine(canvasW, ' ');

        for (auto* node : levels[lvl]) {
            if (node->children.empty()) continue;
            int pc = centerOf[node];

            if (node->children.size() == 1) {
                // Single child: just a vertical pipe
                putChar(brLine, pc, '|');
            } else {
                // Multiple children: draw ________|________
                int lc = centerOf[node->children.front()];
                int rc = centerOf[node->children.back()];
                for (int col = lc; col <= rc; col++)
                    putChar(brLine, col, '_');
                putChar(brLine, pc, '|');   // parent stem
            }
            // Vertical ticks above each child
            for (auto* child : node->children)
                putChar(vdLine, centerOf[child], '|');
        }
        trimRight(brLine);
        trimRight(vdLine);
        std::cout << lineIndent << brLine << "\n";
        std::cout << lineIndent << vdLine << "\n";
    }
}

#endif // AST_H
