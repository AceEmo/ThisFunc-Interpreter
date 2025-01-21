#include <iostream>
#include <stdexcept>
#include <algorithm>

#include "interpreter.hpp"
#include "parser.hpp"

bool GlobalScope::isFunctionDefined(const std::string& name, size_t argc) {
    return definitions.find(name) != definitions.end() && definitions[name].find(argc) != definitions[name].end();
}

std::shared_ptr<Value> GlobalScope::callFunction(const std::string& name, FunctionScope& fncScp) {
    if (!isFunctionDefined(name, fncScp.paramCount())) {
        throw std::runtime_error("Called function which is not defined");
    }

    return definitions[name][fncScp.paramCount()]->definition->eval(fncScp);
}

bool GlobalScope::addFunction(std::shared_ptr<FunctionDefinition> definition) {
    size_t argc = definition->getArgc();
    bool isDefinded = isFunctionDefined(definition->token.data, argc);

	definitions[definition->token.data][argc] = definition;
	return isDefinded;
}

std::shared_ptr<Value> FunctionScope::nth(size_t idx) const {
    if (idx >= parameters.size()) {
        throw std::runtime_error("Index out of range");
    }

    return parameters[idx]->eval(*parentScope);
}

std::shared_ptr<Value> FunctionScope::headOfList() const{
    if (parameters.empty()) {
        throw std::runtime_error("head() with no parameters given");
    }

    std::shared_ptr<ListLiteralNode> l = std::dynamic_pointer_cast<ListLiteralNode>(parameters[0]);

    if (l) {
        return l->contents[0]->eval(*parentScope);
    }

    const std::shared_ptr<Value> fst = parameters[0]->eval(*parentScope);

    if (fst->type == Value::Type::LIST_LITERAL) {
		const std::shared_ptr<ListLiteralValue> lst = std::dynamic_pointer_cast<ListLiteralValue>(fst);

        if (!lst->values.empty()) {
            return lst->values.front();
        }
        throw std::runtime_error("Empty list head call"); 
    }
	throw std::runtime_error("Typing error: the argument to head() must be a list!");
}

std::shared_ptr<Value> FunctionScope::tailOfList() const {
    if (parameters.empty()) {
        throw std::runtime_error("tail() with no parameters given");
    }

    std::shared_ptr<ListLiteralNode> l = std::dynamic_pointer_cast<ListLiteralNode>(parameters[0]);
    if (l) {
        std::vector<std::shared_ptr<Value>> newVals;
        for (size_t i = 1; i < l->contents.size(); ++i) {
            newVals.push_back(l->contents[i]->eval(*parentScope));
        }

        return std::dynamic_pointer_cast<Value>(std::make_shared<ListLiteralValue>(newVals));
    }

    const std::shared_ptr<Value> fst = parameters[0]->eval(*parentScope);

    if (fst->type == Value::Type::LIST_LITERAL) {
		std::vector<std::shared_ptr<Value>> &vals = std::dynamic_pointer_cast<ListLiteralValue>(fst)->values;
        std::vector<std::shared_ptr<Value>> newVals;
        for (size_t i = 1; i < vals.size(); ++i) {
            newVals.push_back(vals[i]);
        }

        return std::dynamic_pointer_cast<Value>(std::make_shared<ListLiteralValue>(newVals));
    }
	throw std::runtime_error("Typing error: the argument to tail() must be a list!");
}

bool eqDouble(double fst, double snd) {
    return std::abs(fst - snd) < (1.0/(1<<30));
}

bool eqHelper(const std::shared_ptr<Value> fst, const std::shared_ptr<Value> snd) {
    if (fst->type == Value::Type::LIST_LITERAL && fst->type == snd->type) {
        std::vector<std::shared_ptr<Value>> &fstVals = std::dynamic_pointer_cast<ListLiteralValue>(fst)->values;
        std::vector<std::shared_ptr<Value>> &sndVals = std::dynamic_pointer_cast<ListLiteralValue>(snd)->values;

        if (fstVals.size() != sndVals.size()) {
            return false;
        }

        for (size_t i = 0; i < fstVals.size(); ++i) {
            if (!eqHelper(fstVals[i], sndVals[i])) {
                return false;
            }
        }
        return true;
    }
    else if (fst->type == Value::Type::INT_NUMBER && fst->type == snd->type) {
        return (std::dynamic_pointer_cast<IntValue>(fst)->value == std::dynamic_pointer_cast<IntValue>(snd)->value);
    }
    else if (fst->type == Value::Type::REAL_NUMBER && fst->type == snd->type) {
        return eqDouble(std::dynamic_pointer_cast<RealValue>(fst)->value, std::dynamic_pointer_cast<RealValue>(snd)->value);
    }
    else if (fst->type == Value::Type::LIST_LITERAL) {
        std::vector<std::shared_ptr<Value>> &fstVals = std::dynamic_pointer_cast<ListLiteralValue>(fst)->values;
        if (fstVals.size() != 1) {
            return false;
        }
        return eqHelper(fstVals[0], snd);
}
    else if (snd->type == Value::Type::LIST_LITERAL) {
        std::vector<std::shared_ptr<Value>> &sndVals = std::dynamic_pointer_cast<ListLiteralValue>(snd)->values;
        
        if (sndVals.size() != 1) {
            return false;
        }

        return eqHelper(fst, sndVals[0]);
    }
    
    double f, s;
    if (fst->type == Value::Type::REAL_NUMBER && snd->type == Value::Type::INT_NUMBER) {
        f = std::dynamic_pointer_cast<RealValue>(fst)->value;
        s = std::dynamic_pointer_cast<IntValue>(snd)->value;

        return eqDouble(f, s);
    }
    else if (fst->type == Value::Type::INT_NUMBER && snd->type == Value::Type::REAL_NUMBER) {
        f = std::dynamic_pointer_cast<IntValue>(fst)->value;
        s = std::dynamic_pointer_cast<RealValue>(snd)->value;

        return eqDouble(f, s);
    }
    return false;
}

std::shared_ptr<Value> eqFunc(FunctionScope &fncScp) {
    const std::shared_ptr<Value> fst = fncScp.nth(0);
    const std::shared_ptr<Value> snd = fncScp.nth(1);

    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(eqHelper(fst, snd)));
}

std::shared_ptr<Value> leFunc(FunctionScope &fncScp) {
    const std::shared_ptr<Value> fst = fncScp.nth(0);
    const std::shared_ptr<Value> snd = fncScp.nth(1);

    if (fst->type == snd->type) {
        switch (fst->type) {
		case Value::Type::INT_NUMBER:
        {
			int fstVal = std::dynamic_pointer_cast<IntValue>(fst)->value;
			int sndVal = std::dynamic_pointer_cast<IntValue>(snd)->value;
			
			return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(fstVal < sndVal));
		}
		case Value::Type::REAL_NUMBER:
		{
			double fstVal = std::dynamic_pointer_cast<RealValue>(fst)->value;
			double sndVal = std::dynamic_pointer_cast<RealValue>(snd)->value;
			
			return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(fstVal < sndVal));
		}
		case Value::Type::LIST_LITERAL:
			throw std::runtime_error("List comparison not supported");
		default:
			throw std::runtime_error("Unknown type comparison");
        }
    }

    throw std::runtime_error("Diffrent types comparison");
}

std::shared_ptr<Value> nandFunc(FunctionScope &fncScp) {
	bool res;

	for (size_t i = 0; i < 2; ++i) {
        std::shared_ptr<Value> val = fncScp.nth(i);
		switch (val->type) {
		case Value::Type::INT_NUMBER:
		{
			res = std::dynamic_pointer_cast<IntValue>(val)->value;
		}
			break;
		case Value::Type::REAL_NUMBER:
		{
			res = std::dynamic_pointer_cast<RealValue>(val)->value;
		}
			break;
		case Value::Type::LIST_LITERAL:
		{
			res = !std::dynamic_pointer_cast<ListLiteralValue>(val)->values.empty();
		}
			break;
		default:
			throw std::runtime_error("Cannot nand() unknown types!");
        }
        if (!res) {
            return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(1));
        }
	}
	return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(0));
}

std::shared_ptr<Value> lengthFunc(FunctionScope &fncScp) {
    const std::shared_ptr<Value> fst = fncScp.nth(0);

    if (fst->type != Value::Type::LIST_LITERAL) {
		return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(-1));
    }

    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(int(std::dynamic_pointer_cast<ListLiteralValue>(fst)->values.size())));
}

std::shared_ptr<Value> headFunc(FunctionScope &fncScp) {
    return fncScp.headOfList();
}

std::shared_ptr<Value> tailFunc(FunctionScope &fncScp) {
    return fncScp.tailOfList();
}

std::shared_ptr<Value> mapFunc(FunctionScope &fncScp) {
    const std::shared_ptr<Value> func = fncScp.nth(0);
    const std::shared_ptr<Value> list = fncScp.nth(1);

    if (list->type != Value::Type::LIST_LITERAL) {
        throw std::runtime_error("Typing error: the second argument to map() must be a list!");
    }
    
    const std::vector<std::shared_ptr<Value>> &listVals = std::dynamic_pointer_cast<ListLiteralValue>(list)->values;
    std::vector<std::shared_ptr<Value>> newVals;
    
    for (const auto &val : listVals) {  
        newVals.push_back(val);
    }
    return std::make_shared<ListLiteralValue>(newVals);
}

std::shared_ptr<Value> filterFunc(FunctionScope &fncScp) {
    const std::shared_ptr<Value> func = fncScp.nth(0);
    const std::shared_ptr<Value> list = fncScp.nth(1);

    if (list->type != Value::Type::LIST_LITERAL) {
        throw std::runtime_error("Typing error: the second argument to filter() must be a list!");
    }
    
    const std::vector<std::shared_ptr<Value>> &listVals = std::dynamic_pointer_cast<ListLiteralValue>(list)->values;
    std::vector<std::shared_ptr<Value>> newVals;
    
    for (const auto &val : listVals) {  
        newVals.push_back(val);
    }

    return std::make_shared<ListLiteralValue>(newVals);
}

std::shared_ptr<Value> ifFunc(FunctionScope &fncScp) {
    const std::shared_ptr<Value> fst = fncScp.nth(0);
    bool condition;

    if (fst->type == Value::Type::INT_NUMBER) {
        condition = std::dynamic_pointer_cast<IntValue>(fst)->value;
    }
    else if (fst->type == Value::Type::REAL_NUMBER) {
        condition = std::dynamic_pointer_cast<RealValue>(fst)->value;
    }
    else if (fst->type == Value::Type::LIST_LITERAL) {
        condition = !std::dynamic_pointer_cast<ListLiteralValue>(fst)->values.empty();
    }
    else {
        throw std::runtime_error("Typing error: the condition of if must be a number - int, real or list literal!");
    }

    return fncScp.nth(condition ? 1 : 2);
}

std::shared_ptr<Value> addFunc(FunctionScope &fncScp) {
    std::shared_ptr<Value> vals[2] = {fncScp.nth(0), fncScp.nth(1)};
    double res = 0;
    bool isDouble = false;

    for (size_t i = 0; i < 2; ++i) {
        if (vals[i]->type == Value::Type::REAL_NUMBER) {
            res += std::dynamic_pointer_cast<RealValue>(vals[i])->value;
            isDouble = true;
        }
        else if (vals[i]->type == Value::Type::INT_NUMBER) {
            res += std::dynamic_pointer_cast<IntValue>(vals[i])->value;
        }
        else {
            throw std::runtime_error("The arguments to add() must be numbers");
        }
    }

    if (isDouble) {
        return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(res));
    }
    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(trunc(res)));
}

std::shared_ptr<Value> subFunc(FunctionScope &fncScp) {
    std::shared_ptr<Value> vals[2] = {fncScp.nth(0), fncScp.nth(1)};
    double res = 0;
    bool isDouble = false;

    for (int i = 0; i < 2; ++i) {
        if (vals[i]->type == Value::Type::REAL_NUMBER) {
            res += (std::dynamic_pointer_cast<RealValue>(vals[i])->value * (1 - 2 * i));
            isDouble = true;
        }
        else if (vals[i]->type == Value::Type::INT_NUMBER) {
            res += (std::dynamic_pointer_cast<IntValue>(vals[i])->value * (1 - 2 * i));
        }
        else {
            throw std::runtime_error("The arguments to sub() must be numbers");
        }
    }

    if (isDouble) {
        return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(res));
    }
    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(trunc(res)));
}

std::shared_ptr<Value> mulFunc(FunctionScope &fncScp) {
    std::shared_ptr<Value> vals[2] = {fncScp.nth(0), fncScp.nth(1)};
    double res = 1.0;
    bool isDouble = false;

    for (size_t i = 0; i < 2; ++i) {
        if (vals[i]->type == Value::Type::REAL_NUMBER) {
            res *= std::dynamic_pointer_cast<RealValue>(vals[i])->value;
            isDouble = true;
        }
        else if (vals[i]->type == Value::Type::INT_NUMBER) {
            res *= std::dynamic_pointer_cast<IntValue>(vals[i])->value;
        }
        else {
            throw std::runtime_error("The arguments to mul() must be numbers");
        }
    }

    if (isDouble) {
        return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(res));
    }
    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(trunc(res)));
}

std::shared_ptr<Value> divFunc(FunctionScope &fncScp) {
    const std::shared_ptr<Value> fst = fncScp.nth(0);
    const std::shared_ptr<Value> snd = fncScp.nth(1);

    if ((fst->type != Value::Type::REAL_NUMBER && fst->type != Value::Type::INT_NUMBER) ||
        (snd->type != Value::Type::REAL_NUMBER && snd->type != Value::Type::INT_NUMBER)) {
        throw std::runtime_error("The arguments to div() must be numbers");
    }

    double fstVal = (fst->type == Value::Type::REAL_NUMBER) 
        ? std::dynamic_pointer_cast<RealValue>(fst)->value 
        : std::dynamic_pointer_cast<IntValue>(fst)->value;

    double sndVal = (snd->type == Value::Type::REAL_NUMBER) 
        ? std::dynamic_pointer_cast<RealValue>(snd)->value 
        : std::dynamic_pointer_cast<IntValue>(snd)->value;

    if (sndVal == 0.0) {
        throw std::runtime_error("Division by zero!");
    }

    double result = fstVal / sndVal;
    if (fst->type == Value::Type::REAL_NUMBER || snd->type == Value::Type::REAL_NUMBER) {
        return std::make_shared<RealValue>(result);
    } else {
        return std::make_shared<IntValue>(static_cast<int>(result));
    }
}

std::shared_ptr<Value> sqrtFunc(FunctionScope &fncScp) {
    const std::shared_ptr<Value>& fst = fncScp.nth(0);

    if (fst->type == Value::Type::INT_NUMBER) {
        return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(std::sqrt((double)std::dynamic_pointer_cast<IntValue>(fst)->value)));
    }
    else if (fst->type == Value::Type::REAL_NUMBER) {
        return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(std::sqrt(std::dynamic_pointer_cast<RealValue>(fst)->value)));
    }
    throw std::runtime_error("The argument to sqrt() must be a number");
}

std::shared_ptr<Value> sinFunc(FunctionScope &fncScp) {
    const std::shared_ptr<Value>& fst = fncScp.nth(0);

    if (fst->type == Value::Type::INT_NUMBER) {
        return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(std::sin((double)std::dynamic_pointer_cast<IntValue>(fst)->value)));
    }
    else if (fst->type == Value::Type::REAL_NUMBER) {
        return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(std::sin(std::dynamic_pointer_cast<RealValue>(fst)->value)));
    }
    throw std::runtime_error("The argument to sin() must be a number");
}

std::shared_ptr<Value> cosFunc(FunctionScope &fncScp) {
    const std::shared_ptr<Value>& fst = fncScp.nth(0);

    if (fst->type == Value::Type::INT_NUMBER) {
        return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(std::cos((double)std::dynamic_pointer_cast<IntValue>(fst)->value)));
    }
    else if (fst->type == Value::Type::REAL_NUMBER) {
        return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(std::cos(std::dynamic_pointer_cast<RealValue>(fst)->value)));
    }
    throw std::runtime_error("The argument to cos() must be a number");
}

std::shared_ptr<Value> powFunc(FunctionScope &fncScp) {
    const std::shared_ptr<Value>& fst = fncScp.nth(0);
    const std::shared_ptr<Value>& snd = fncScp.nth(1);

    if ((fst->type != Value::Type::REAL_NUMBER && fst->type != Value::Type::INT_NUMBER) ||
        (snd->type != Value::Type::REAL_NUMBER && snd->type != Value::Type::INT_NUMBER)) {
        throw std::runtime_error("The arguments to pow() must be numbers");
    }

    double fstVal = (fst->type == Value::Type::REAL_NUMBER) 
        ? std::dynamic_pointer_cast<RealValue>(fst)->value 
        : std::dynamic_pointer_cast<IntValue>(fst)->value;

    double sndVal = (snd->type == Value::Type::REAL_NUMBER) 
        ? std::dynamic_pointer_cast<RealValue>(snd)->value 
        : std::dynamic_pointer_cast<IntValue>(snd)->value;

    return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(std::pow(fstVal, sndVal)));
}

void GlobalScope::loadDefaultLibrary() {
    const std::function<std::shared_ptr<Value>(FunctionScope&)> functions[] = {
        eqFunc, leFunc, nandFunc, lengthFunc, headFunc, tailFunc,
        ifFunc, addFunc, subFunc, mulFunc, divFunc,
        sqrtFunc, mapFunc, filterFunc, sinFunc, cosFunc, powFunc };
    const std::string names[] = {
        "eq", "le", "nand", "length", "head", "tail",
        "if", "add", "sub", "mul", "div",
        "sqrt", "map", "filter", "sin", "cos", "pow" };
    const size_t arguments[] = {
        2, 2, 2, 1, 1, 1, 
        3, 2, 2, 2, 2,
        1, 2, 2, 1, 1, 2 };
    for (size_t i = 0; i < 18; ++i) {
        Token tok = {Token::Type::FUNC, names[i], -1};
        std::shared_ptr<FunctionDefinition> fDef = std::make_shared<FunctionDefinition>(tok, std::make_shared<DefaultFunctionNode>(names[i], functions[i], arguments[i]));
        addFunction(fDef);
    }
}