#pragma once

#include <functional>
#include <memory>
#include <cmath>

#include "lexer.hpp"
#include "returnValue.hpp"

struct FunctionScope;

struct Node {
    Token token;

	explicit Node(Token token);

    virtual std::shared_ptr<Value> eval(FunctionScope &fncScp) const = 0;

    virtual size_t getArgc() const = 0;

};

struct IntNode : public Node {
	explicit IntNode(Token token);

    std::shared_ptr<Value> eval(FunctionScope &fncScp) const;

    size_t getArgc() const { return 0; }

};

struct DoubleNode : public Node {
	explicit DoubleNode(Token token);

    std::shared_ptr<Value>eval(FunctionScope &fncScp) const;

    size_t getArgc() const { return 0; }
};

struct ListLiteralNode : public Node {
	std::vector<std::shared_ptr<Node>> contents;

	ListLiteralNode(Token token, const std::vector<std::shared_ptr<Node>> &contents);

    std::shared_ptr<Value> eval(FunctionScope &fncScp) const;

    size_t getArgc() const {
        size_t res = 0;
        for (std::shared_ptr<Node> node : contents) {
            res = std::max(res, node->getArgc());
        }
        return res;
    }
};

struct ArgumentNode : public Node {
	explicit ArgumentNode(Token token);

    std::shared_ptr<Value> eval(FunctionScope &fncScp) const;

    size_t getArgc() const {
        return std::stoi(token.data) + 1;
    }
};

struct FunctionDefinition : public Node {
    const std::shared_ptr<Node> definition;

    FunctionDefinition(Token token, const std::shared_ptr<Node> definition) : Node(token), definition(definition) {}

    std::shared_ptr<Value> eval(FunctionScope &fncScp) const;

    size_t getArgc() const {
        return definition->getArgc();
    }
};

struct FunctionApplication : public Node {
    const std::vector<std::shared_ptr<Node>> arguments;

    FunctionApplication(Token token, const std::vector<std::shared_ptr<Node>> &arguments) : Node(token), arguments(arguments) {}
	~FunctionApplication() = default;

    std::shared_ptr<Value> eval(FunctionScope &parentScp) const;

    size_t getArgc() const {
        size_t res = 0;
        for (std::shared_ptr<Node> node : arguments) {
            res = std::max(res, node->getArgc());
        }
        return res;
    }
};

struct DefaultFunctionNode : public Node {
    const std::function<std::shared_ptr<Value>(FunctionScope&)> func;
    const size_t argc;

    DefaultFunctionNode(const std::string &name, const std::function<std::shared_ptr<Value>(FunctionScope&)>& func, size_t argc) 
    : Node({Token::Type::FUNC, name, -1}), func(func), argc(argc) {}

    std::shared_ptr<Value> eval(FunctionScope &fncScp) const {
        return func(fncScp);
    }

    size_t getArgc() const {
        return argc;
    }
};

class Parser {
public:
    Parser(std::vector<Token>::iterator begin);
    std::shared_ptr<Node> parse(std::ostream& out);

private:
    std::vector<Token>::iterator curr;
    std::shared_ptr<Node> expr(std::ostream& out);
    bool eof();
};