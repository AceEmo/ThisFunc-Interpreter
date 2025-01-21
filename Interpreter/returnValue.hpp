#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

struct Value {
    enum class Type {
        REAL_NUMBER,
        INT_NUMBER,
        LIST_LITERAL,
    };

    Type type;

    Value(Type type) : type(type) {}

    virtual std::string toString() const = 0;

};

struct RealValue : public Value {
    const double value;

    RealValue(double value) : Value(Type::REAL_NUMBER), value(value) {}

    std::string toString() const
    {
        return std::to_string(value);
    }

};

struct IntValue : public Value {
    const int value;

    IntValue(int value) : Value(Type::INT_NUMBER), value(value) {}

    std::string toString() const {
        return std::to_string(value);
    }

};

struct ListLiteralValue : public Value {
    std::vector<std::shared_ptr<Value>> values;

    ListLiteralValue(const std::vector<std::shared_ptr<Value>> &values) : Value(Value::Type::LIST_LITERAL), values(values) {
        if (type != Value::Type::LIST_LITERAL) {
            throw std::runtime_error("Invalid type for ListValue");
        }
    }

    std::string toString() const {
        std::string result = "[";
        for (size_t i = 0; i < values.size(); ++i) {
            result += values[i]->toString();
            if (i < values.size() - 1) result += ", ";
        }
        result += "]";

        return result;
    }
};
