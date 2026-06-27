#include "scriptcompiler.h"
#include <QDebug>
#include <QTextStream>

namespace ScriptLang {

const QMap<QString, QString> ScriptCompiler::keywords = {
    {"print", "print"},
    {"let", "let"},
    {"var", "var"},
    {"true", "true"},
    {"false", "false"}
};

ScriptCompiler::ScriptCompiler() : m_currentToken(0) {}

bool ScriptCompiler::compile(const QString &source, QString &output, QString &error) {
    m_output.clear();
    m_variables.clear();
    m_tokens.clear();
    m_currentToken = 0;
    m_bytecode.clear();
    
    // Split source into lines
    m_sourceLines = source.split('\n');
    
    // Tokenize
    tokenize(source);
    
    // Parse
    m_ast = parseProgram();
    
    if (m_tokens.size() > m_currentToken) {
        error = QString("Unexpected token at line %1: %2")
                .arg(m_tokens[m_currentToken].line)
                .arg(m_tokens[m_currentToken].value);
        return false;
    }
    
    // Generate bytecode
    generateBytecode(m_ast);
    
    // Add halt instruction
    m_bytecode.append(Instruction(Opcode::Halt));
    
    // Execute bytecode
    executeBytecode();
    
    output = m_output;
    return true;
}

void ScriptCompiler::tokenize(const QString &source) {
    int line = 1;
    int column = 1;
    int i = 0;
    
    while (i < source.length()) {
        QChar ch = source[i];
        
        // Skip whitespace
        if (ch == ' ' || ch == '\t' || ch == '\r') {
            i++;
            column++;
            continue;
        }
        
        // Newline
        if (ch == '\n') {
            addToken(TokenType::Newline, "\n", line, column);
            i++;
            line++;
            column = 1;
            continue;
        }
        
        // String
        if (ch == '"') {
            int start = i;
            i++;
            column++;
            while (i < source.length() && source[i] != '"') {
                if (source[i] == '\\' && i + 1 < source.length()) {
                    i += 2;
                    column += 2;
                } else {
                    i++;
                    column++;
                }
            }
            if (i < source.length()) {
                i++;
                column++;
            }
            addToken(TokenType::String, source.mid(start, i - start), line, column - (i - start));
            continue;
        }
        
        // Number
        if (ch.isDigit() || (ch == '.' && i + 1 < source.length() && source[i + 1].isDigit())) {
            int start = i;
            while (i < source.length() && (source[i].isDigit() || source[i] == '.')) {
                i++;
                column++;
            }
            addToken(TokenType::Number, source.mid(start, i - start), line, column - (i - start));
            continue;
        }
        
        // Identifier or keyword
        if (ch.isLetter() || ch == '_') {
            int start = i;
            while (i < source.length() && (source[i].isLetterOrNumber() || source[i] == '_')) {
                i++;
                column++;
            }
            QString text = source.mid(start, i - start);
            if (keywords.contains(text)) {
                addToken(TokenType::Keyword, text, line, column - text.length());
            } else {
                addToken(TokenType::Identifier, text, line, column - text.length());
            }
            continue;
        }
        
        // Operators
        if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%') {
            addToken(TokenType::Operator, QString(ch), line, column);
            i++;
            column++;
            continue;
        }
        
        // Comparison operators
        if (ch == '=' || ch == '!' || ch == '<' || ch == '>') {
            QString op;
            if (i + 1 < source.length() && source[i + 1] == '=') {
                op = QString(ch) + "=";
                addToken(TokenType::Operator, op, line, column);
                i += 2;
                column += 2;
            } else {
                op = QString(ch);
                addToken(TokenType::Operator, op, line, column);
                i++;
                column++;
            }
            continue;
        }
        
        // Punctuation
        if (ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == ',' || ch == ';') {
            addToken(TokenType::Punctuation, QString(ch), line, column);
            i++;
            column++;
            continue;
        }
        
        // Skip comments
        if (ch == '#') {
            while (i < source.length() && source[i] != '\n') {
                i++;
                column++;
            }
            continue;
        }
        
        // Unknown character
        i++;
        column++;
    }
    
    addToken(TokenType::EOF_, "", line, column);
}

void ScriptCompiler::addToken(TokenType type, const QString &value, int line, int column) {
    m_tokens.append(Token(type, value, line, column));
}

Token ScriptCompiler::peekToken(int offset) {
    int idx = m_currentToken + offset;
    if (idx >= m_tokens.size()) {
        return Token(TokenType::EOF_, "", 0, 0);
    }
    return m_tokens[idx];
}

bool ScriptCompiler::match(TokenType type) {
    return peekToken().type == type;
}

void ScriptCompiler::consumeToken() {
    m_currentToken++;
}

std::shared_ptr<ASTNode> ScriptCompiler::parseProgram() {
    auto program = std::make_shared<ASTNode>(ASTNode::Program);
    
    while (!match(TokenType::EOF_)) {
        if (match(TokenType::Newline)) {
            consumeToken();
            continue;
        }
        auto stmt = parseStatement();
        if (stmt) {
            program->children.append(stmt);
        }
    }
    
    return program;
}

std::shared_ptr<ASTNode> ScriptCompiler::parseStatement() {
    if (match(TokenType::Keyword) && peekToken().value == "print") {
        consumeToken(); // consume 'print'
        
        if (!match(TokenType::Punctuation) || peekToken().value != "(") {
            return nullptr;
        }
        consumeToken(); // consume '('
        
        auto expr = parseExpression();
        
        if (!match(TokenType::Punctuation) || peekToken().value != ")") {
            return nullptr;
        }
        consumeToken(); // consume ')'
        
        auto printNode = std::make_shared<ASTNode>(ASTNode::PrintStatement);
        printNode->children.append(expr);
        return printNode;
    }
    
    if (match(TokenType::Keyword) && (peekToken().value == "let" || peekToken().value == "var")) {
        consumeToken(); // consume 'let' or 'var'
        
        if (!match(TokenType::Identifier)) {
            return nullptr;
        }
        QString varName = peekToken().value;
        consumeToken(); // consume identifier
        
        if (!match(TokenType::Operator) || peekToken().value != "=") {
            return nullptr;
        }
        consumeToken(); // consume '='
        
        auto expr = parseExpression();
        
        auto varDecl = std::make_shared<ASTNode>(ASTNode::VariableDeclaration);
        varDecl->value = varName;
        varDecl->children.append(expr);
        return varDecl;
    }
    
    // Expression statement
    return parseExpression();
}

std::shared_ptr<ASTNode> ScriptCompiler::parseExpression() {
    return parseBinaryExpression(0);
}

std::shared_ptr<ASTNode> ScriptCompiler::parsePrimary() {
    if (match(TokenType::Number)) {
        auto node = std::make_shared<ASTNode>(ASTNode::NumberLiteral);
        node->value = peekToken().value;
        consumeToken();
        return node;
    }
    
    if (match(TokenType::String)) {
        auto node = std::make_shared<ASTNode>(ASTNode::StringLiteral);
        node->value = peekToken().value;
        consumeToken();
        return node;
    }
    
    if (match(TokenType::Keyword) && peekToken().value == "true") {
        auto node = std::make_shared<ASTNode>(ASTNode::BooleanLiteral);
        node->value = "true";
        consumeToken();
        return node;
    }
    
    if (match(TokenType::Keyword) && peekToken().value == "false") {
        auto node = std::make_shared<ASTNode>(ASTNode::BooleanLiteral);
        node->value = "false";
        consumeToken();
        return node;
    }
    
    if (match(TokenType::Identifier)) {
        auto node = std::make_shared<ASTNode>(ASTNode::Identifier);
        node->value = peekToken().value;
        consumeToken();
        return node;
    }
    
    if (match(TokenType::Punctuation) && peekToken().value == "(") {
        consumeToken(); // consume '('
        auto expr = parseExpression();
        if (match(TokenType::Punctuation) && peekToken().value == ")") {
            consumeToken(); // consume ')'
        }
        return expr;
    }
    
    return nullptr;
}

std::shared_ptr<ASTNode> ScriptCompiler::parseBinaryExpression(int precedence) {
    auto left = parsePrimary();
    
    while (match(TokenType::Operator)) {
        QString op = peekToken().value;
        int opPrecedence = 0;
        
        if (op == "+" || op == "-") opPrecedence = 1;
        else if (op == "*" || op == "/" || op == "%") opPrecedence = 2;
        else break;
        
        if (opPrecedence < precedence) break;
        
        consumeToken(); // consume operator
        
        auto right = parseBinaryExpression(opPrecedence + 1);
        
        auto binExpr = std::make_shared<ASTNode>(ASTNode::BinaryExpression);
        binExpr->value = op;
        binExpr->left = left;
        binExpr->right = right;
        left = binExpr;
    }
    
    return left;
}

void ScriptCompiler::generateBytecode(std::shared_ptr<ASTNode> node) {
    if (!node) return;
    
    switch (node->nodeType) {
    case ASTNode::Program:
        for (auto &child : node->children) {
            generateBytecode(child);
        }
        break;
        
    case ASTNode::PrintStatement:
        if (!node->children.isEmpty()) {
            generateBytecode(node->children[0]);
            m_bytecode.append(Instruction(Opcode::Print));
        }
        break;
        
    case ASTNode::NumberLiteral:
        m_bytecode.append(Instruction(Opcode::LoadConst, node->value.toDouble()));
        break;
        
    case ASTNode::StringLiteral:
        m_bytecode.append(Instruction(Opcode::LoadConst, node->value.mid(1, node->value.length() - 2)));
        break;
        
    case ASTNode::BooleanLiteral:
        m_bytecode.append(Instruction(Opcode::LoadConst, node->value == "true"));
        break;
        
    case ASTNode::Identifier:
        m_bytecode.append(Instruction(Opcode::LoadVar, node->value, true));
        break;
        
    case ASTNode::VariableDeclaration:
        if (!node->children.isEmpty()) {
            generateBytecode(node->children[0]);
            m_bytecode.append(Instruction(Opcode::StoreVar, node->value, true));
        }
        break;
        
    case ASTNode::BinaryExpression:
        generateBytecode(node->left);
        generateBytecode(node->right);
        
        if (node->value == "+") {
            m_bytecode.append(Instruction(Opcode::Add));
        } else if (node->value == "-") {
            m_bytecode.append(Instruction(Opcode::Sub));
        } else if (node->value == "*") {
            m_bytecode.append(Instruction(Opcode::Mul));
        } else if (node->value == "/") {
            m_bytecode.append(Instruction(Opcode::Div));
        } else if (node->value == "%") {
            m_bytecode.append(Instruction(Opcode::Mod));
        }
        break;
        
    default:
        break;
    }
}

void ScriptCompiler::executeBytecode() {
    QVector<Value> stack;
    
    for (const auto &instr : m_bytecode) {
        switch (instr.opcode) {
        case Opcode::LoadConst:
            if (instr.stringValue.isEmpty()) {
                stack.append(Value(instr.numberValue));
            } else {
                stack.append(Value(instr.stringValue));
            }
            break;
            
        case Opcode::LoadVar:
            if (m_variables.contains(instr.varName)) {
                stack.append(m_variables[instr.varName]);
            } else {
                stack.append(Value());
            }
            break;
            
        case Opcode::StoreVar:
            if (!stack.isEmpty()) {
                m_variables[instr.varName] = stack.last();
            }
            break;
            
        case Opcode::Add:
            if (stack.size() >= 2) {
                Value b = stack.takeLast();
                Value a = stack.takeLast();
                if (a.type == Value::String || b.type == Value::String) {
                    stack.append(Value(a.asString() + b.asString()));
                } else {
                    stack.append(Value(a.asNumber() + b.asNumber()));
                }
            }
            break;
            
        case Opcode::Sub:
            if (stack.size() >= 2) {
                Value b = stack.takeLast();
                Value a = stack.takeLast();
                stack.append(Value(a.asNumber() - b.asNumber()));
            }
            break;
            
        case Opcode::Mul:
            if (stack.size() >= 2) {
                Value b = stack.takeLast();
                Value a = stack.takeLast();
                stack.append(Value(a.asNumber() * b.asNumber()));
            }
            break;
            
        case Opcode::Div:
            if (stack.size() >= 2) {
                Value b = stack.takeLast();
                Value a = stack.takeLast();
                stack.append(Value(a.asNumber() / b.asNumber()));
            }
            break;
            
        case Opcode::Mod:
            if (stack.size() >= 2) {
                Value b = stack.takeLast();
                Value a = stack.takeLast();
                stack.append(Value(static_cast<double>(static_cast<int>(a.asNumber()) % static_cast<int>(b.asNumber()))));
            }
            break;
            
        case Opcode::Print:
            if (!stack.isEmpty()) {
                Value val = stack.takeLast();
                m_output += val.asString() + "\n";
            }
            break;
            
        case Opcode::Pop:
            if (!stack.isEmpty()) {
                stack.takeLast();
            }
            break;
            
        case Opcode::Halt:
            return;
        }
    }
}

} // namespace ScriptLang