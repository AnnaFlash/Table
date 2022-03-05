#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#DIV/0!";
}

namespace {
    class Formula : public FormulaInterface {
    public:
        // Реализуйте следующие методы:
        explicit Formula(std::string expression) : ast_(ParseFormulaAST(expression)) {}
        Value Evaluate(const SheetInterface& sheet) const override {
            try {
                std::function<CellInterface::Value(const Position&)> result = [&sheet](const Position& cell) {
                    if (sheet.GetCell(cell)) {
                        return sheet.GetCell(cell)->GetValue();
                    }
                    else {
                        return CellInterface::Value(0.0);
                    }
                };
                auto a = ast_.Execute(result);
                if (std::holds_alternative<double>(a)) {
                    if (std::isinf(std::get<double>(a))) {
                        return FormulaError::Category::Div0;
                    }
                    else {
                        return std::get<double>(a);
                    }
                }
                if (std::holds_alternative<FormulaError>(a)) {
                    return std::get<FormulaError>(a);
                }
            }
            catch (FormulaError const& e) {
                return e;
            }
            return FormulaError::Category::Value;
        }
        std::string GetExpression() const override {
            std::ostringstream expression;
            ast_.PrintFormula(expression);
            return expression.str();
        }
        std::vector<Position> GetReferencedCells() const override {
            std::vector<Position> v(ast_.GetCells().begin(), ast_.GetCells().end());
            std::sort(v.begin(), v.end());
            v.erase(std::unique(v.begin(), v.end()), v.end());
            return v;
        }
    private:
        FormulaAST ast_;
    };
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try {
        ParseFormulaAST(expression);
    }
    catch (std::exception const& e) {
        throw FormulaException("");
    }
    return std::make_unique<Formula>(std::move(expression));
}