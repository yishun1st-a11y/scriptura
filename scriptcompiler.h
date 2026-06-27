#ifndef SCRIPT_COMPILER_H
#define SCRIPT_COMPILER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QVector>
#include <QRegularExpression>
#include <memory>

namespace ScriptLang {

// Token types for the Script language
enum class TokenType {
    Identifier,
    Number,
    String,
    Keyword,
    Operator,
    Punctuation,
    Newline,
    EOF_
};

// Token structure
struct Token {
    TokenType type;
    QString value;
    int line;
    int column;
    
    Token(TokenType t = TokenType::EOF_, const QString &v = "", int l = 0, int c = 0)
        : type(t), value(v), line(l), column(c) {}
};

// Bytecode instructions
enum class Opcode {
    // Stack operations
    LoadConst,      // Load constant onto stack
    LoadVar,        // Load variable onto stack
    StoreVar,       // Store top of stack to variable
    
    // Arithmetic operations
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    
    // String operations
    Concat,
    
    // Control flow
    Print,          // Print top of stack
    Pop,            // Pop from stack
    Halt            // Stop execution
};

// Bytecode instruction
struct Instruction {
    Opcode opcode;
    double numberValue;
    QString stringValue;
    QString varName;
    
    Instruction(Opcode op) : opcode(op), numberValue(0) {}
    Instruction(Opcode op, double val) : opcode(op), numberValue(val) {}
    Instruction(Opcode op, const QString &str) : opcode(op), stringValue(str) {}
    Instruction(Opcode op, const QString &str, bool isVar) : opcode(op), varName(str) {}
};

// Value types for runtime
struct Value {
    enum Type { Number, String, Boolean, Null } type;
    double numVal;
    QString strVal;
    
    Value() : type(Null), numVal(0) {}
    Value(double n) : type(Number), numVal(n) {}
    Value(const QString &s) : type(String), strVal(s) {}
    Value(bool b) : type(Boolean), numVal(b ? 1 : 0) {}
    
    double asNumber() const { return numVal; }
    QString asString() const { 
        if (type == String) return strVal;
        if (type == Number) return QString::number(numVal);
        if (type == Boolean) return numVal ? "true" : "false";
        return "null";
    }
    bool asBoolean() const { return type == Boolean ? (numVal != 0) : false; }
    bool isNull() const { return type == Null; }
};

// Abstract syntax tree node
struct ASTNode {
    enum NodeType {
        Program,
        PrintStatement,
        BinaryExpression,
        StringConcat,
        Identifier,
        NumberLiteral,
        StringLiteral,
        BooleanLiteral,
        VariableDeclaration,
        Assignment,
        Block
    } nodeType;
    
    QString value;
    QVector<std::shared_ptr<ASTNode>> children;
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;
    
    ASTNode(NodeType t) : nodeType(t) {}
};

// Compiler class - compiles to bytecode
class ScriptCompiler {
public:
    ScriptCompiler();
    
    // Compile source code to bytecode
    bool compile(const QString &source, QString &output, QString &error);
    
    // Get the bytecode for inspection
    QVector<Instruction> getBytecode() const { return m_bytecode; }

private:
    // Lexer
    void tokenize(const QString &source);
    void addToken(TokenType type, const QString &value, int line, int column);
    
    // Parser
    std::shared_ptr<ASTNode> parseProgram();
    std::shared_ptr<ASTNode> parseStatement();
    std::shared_ptr<ASTNode> parseExpression();
    std::shared_ptr<ASTNode> parsePrimary();
    std::shared_ptr<ASTNode> parseBinaryExpression(int precedence);
    
    // Code generator
    void generateBytecode(std::shared_ptr<ASTNode> node);
    
    // Virtual machine
    void executeBytecode();
    
    // Helper methods
    bool match(TokenType type);
    Token peekToken(int offset = 0);
    void consumeToken();
    
    // Keywords
    static const QMap<QString, QString> keywords;
    
    QStringList m_sourceLines;
    QVector<Token> m_tokens;
    int m_currentToken;
    std::shared_ptr<ASTNode> m_ast;
    QMap<QString, Value> m_variables;
    QVector<Instruction> m_bytecode;
    QString m_output;
};

} // namespace ScriptLang

#endif // SCRIPT_COMPILER_H