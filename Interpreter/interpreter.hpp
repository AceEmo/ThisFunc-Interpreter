#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "returnValue.hpp"

struct Node;
struct FunctionDefinition;
struct FunctionScope;

struct GlobalScope {
    bool isFunctionDefined(const std::string& name, size_t argc);
    std::shared_ptr<Value> callFunction(const std::string& name, FunctionScope& fncScp);
    bool addFunction(std::shared_ptr<FunctionDefinition> definition);
    void loadDefaultLibrary();

private:
    std::unordered_map<std::string, std::unordered_map<size_t, std::shared_ptr<FunctionDefinition>>> definitions;
};

struct FunctionScope {
    FunctionScope(GlobalScope &globalExecContext, std::shared_ptr<FunctionScope> parentScope, const std::vector<std::shared_ptr<Node>> &parameters)
    : globalExecContext(globalExecContext), parentScope(parentScope), parameters(parameters) {}

    FunctionScope(GlobalScope &globalExecContext, std::shared_ptr<FunctionScope> parentScope, std::vector<std::shared_ptr<Node>>&& parameters)
    : globalExecContext(globalExecContext), parentScope(parentScope), parameters(std::move(parameters)) {}

    std::shared_ptr<Value> nth(size_t idx) const;

    std::shared_ptr<Value> headOfList() const;
    std::shared_ptr<Value> tailOfList() const;

    size_t paramCount() const { return parameters.size(); }

    GlobalScope& getGlobalScope() { return globalExecContext; }

    const GlobalScope& getGlobalScope() const { return globalExecContext; }

private:
    GlobalScope& globalExecContext;

    std::shared_ptr<FunctionScope> parentScope;
    std::vector<std::shared_ptr<Node>> parameters;
};