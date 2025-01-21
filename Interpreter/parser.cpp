#include "parser.hpp"
#include "interpreter.hpp"

Node::Node(Token token) : token(token) {}

IntNode::IntNode(Token token) : Node(token) {}

std::shared_ptr<Value> IntNode::eval(FunctionScope &fncScp) const {
    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(std::stoi(token.data)));
}

DoubleNode::DoubleNode(Token token) : Node(token) {}

std::shared_ptr<Value> DoubleNode::eval(FunctionScope &fncScp) const {
    return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(std::stod(token.data)));
}

ArgumentNode::ArgumentNode(Token token) : Node(token) {}

std::shared_ptr<Value> ArgumentNode::eval(FunctionScope &fncScp) const {
    return fncScp.nth(std::stoi(token.data));
}

ListLiteralNode::ListLiteralNode(Token token, const std::vector<std::shared_ptr<Node>> &contents) : Node(token), contents(contents) {}

std::shared_ptr<Value> ListLiteralNode::eval(FunctionScope &fncScp) const {
    std::vector<std::shared_ptr<Value>> list;

    for (std::shared_ptr<Node> item : contents) {
        list.push_back(item->eval(fncScp));
    }

    return std::dynamic_pointer_cast<Value>(std::make_shared<ListLiteralValue>(list));
}

std::shared_ptr<Value> FunctionDefinition::eval(FunctionScope &fncScp) const {
    std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(fncScp.getGlobalScope().addFunction(std::make_shared<FunctionDefinition>(*this))));
    return nullptr;
}

std::shared_ptr<Value> FunctionApplication::eval(FunctionScope &parentScope) const {
    FunctionScope localScope(parentScope.getGlobalScope(), std::make_shared<FunctionScope>(parentScope), arguments);
    
    return parentScope.getGlobalScope().callFunction(token.data, localScope);
}

std::shared_ptr<Node> Parser::parse(std::ostream& out) {
    std::shared_ptr<Node> ast = expr(out);

    if (eof()) {
        return ast;
    }

    throw std::runtime_error("Colud not parse the input!");
}

bool Parser::eof() {
    return curr->type == Token::Type::eof;
}

Parser::Parser(std::vector<Token>::iterator begin) : curr(begin) {}

std::shared_ptr<Node> Parser::expr(std::ostream& out) {
    if (eof()) {
        throw std::runtime_error("Insufficient input provided.\n");
    }

    if (curr->type == Token::Type::OPEN_ROUND) {
        std::string err = "Unexpected '(' occurred, missing identifier before it.\n";
        err += "Token type: OPEN_ROUND";
        throw std::runtime_error(err);
    }
    if (curr->type == Token::Type::CLOSE_ROUND) {
        std::string err = "Unexpected ')' occurred.\n";
        err += "Token type: CLOSE_ROUND";
        throw std::runtime_error(err);
    }
    if (curr->type == Token::Type::CLOSE_SQUARE) {
        std::string err = "Unexpected ']' occurred.\n";
        err += "Token type: CLOSE_SQUARE";
        throw std::runtime_error(err);
    }
    if (curr->type == Token::Type::COMMA) {
        std::string err = "Unexpected ',' occurred.\n";
        err += "Token type: COMMA";
        throw std::runtime_error(err);
    }
    if (curr->type == Token::Type::ARG) {
        Token tempToken = *curr;

        ++curr;

        return std::dynamic_pointer_cast<Node>(std::make_shared<ArgumentNode>(tempToken));
    }
    if (curr->type == Token::Type::KW_INT) {
        Token tempToken = *curr;

        ++curr;

        return std::dynamic_pointer_cast<Node>(std::make_shared<IntNode>(tempToken));
    }

    if (curr->type == Token::Type::KW_DOUBLE) {
        Token tempToken = *curr;

        ++curr;

        return std::dynamic_pointer_cast<Node>(std::make_shared<DoubleNode>(tempToken));
    }

    if (curr->type == Token::Type::OPEN_SQUARE) {
        Token returnToken = *curr;

        ++curr;

        std::vector<std::shared_ptr<Node>> arguments;

        while(curr->type != Token::Type::eof && curr->type != Token::Type::CLOSE_SQUARE) {
            std::shared_ptr<Node> elem = expr(out);

            if (!elem) {
                std::string err = "Parsing List Literal error occurred";
                throw std::runtime_error(err);
            }
            arguments.push_back(elem);
        }

        if (curr->type == Token::Type::CLOSE_SQUARE) {
            ++curr;
            return std::dynamic_pointer_cast<Node>(std::make_shared<ListLiteralNode>(returnToken, arguments));
        }

        std::string err = "Expected ']'";
        throw std::runtime_error(err);
    }
    if (curr->type == Token::Type::KW_LIST) {
        Token returnToken = *curr;

        ++curr;

        if (curr->type != Token::Type::OPEN_ROUND) {
            std::string err = "Expected '(' after 'list'";
            throw std::runtime_error(err);
        }

        ++curr;

        std::vector<std::shared_ptr<Node>> arguments;

        while (curr->type != Token::Type::eof && curr->type != Token::Type::CLOSE_ROUND) {
            std::shared_ptr<Node> elem = expr(out);

            if (!elem) {
                std::string err = "Parsing List Function Call error occurred";
                throw std::runtime_error(err);
            }

            arguments.push_back(elem);

            if (curr->type == Token::Type::COMMA) {
                ++curr;
            }
        }

        if (curr->type == Token::Type::CLOSE_ROUND) {
            ++curr;
            return std::dynamic_pointer_cast<Node>(std::make_shared<ListLiteralNode>(returnToken, arguments));
        }

        std::string err = "Expected ')'";
        throw std::runtime_error(err);
    }

    Token f = *curr;
    ++curr;
    
    if (eof()) {
        std::string err = "Insufficient input";
        throw std::runtime_error(err);
    }
    if (curr->type == Token::Type::ARROW) {
        ++curr;

        std::shared_ptr<Node> definition = expr(out);

        if (definition == nullptr) {
            throw std::runtime_error("Problem while parsing function definition\n");
        }

        return std::dynamic_pointer_cast<Node>(std::make_shared<FunctionDefinition>(f, definition));
    }

    if (curr->type != Token::Type::OPEN_ROUND) {
        std::string err = "Expected '('";
        throw std::runtime_error(err);
    }

    ++curr;

    std::vector<std::shared_ptr<Node>> args;
    bool hasMoreArgs = true;

    while (!eof() && curr->type != Token::Type::CLOSE_ROUND && hasMoreArgs) {
        std::shared_ptr<Node> arg = expr(out);

        if (arg.get() == nullptr || curr->type == Token::Type::eof) {
            std::string err = "Problem while parsing function call.";
            throw std::runtime_error(err);
        }
        args.push_back(arg);
        hasMoreArgs = curr->type == Token::Type::COMMA;
        if (hasMoreArgs) {
            ++curr;
        }
    }

    if (curr->type != Token::Type::CLOSE_ROUND) {
        std::string err = "Expected ')'";
        throw std::runtime_error(err);
    }
    ++curr;
    return std::dynamic_pointer_cast<Node>(std::make_shared<FunctionApplication>(f, args));
}