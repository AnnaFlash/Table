#pragma once

#include "common.h"
#include "formula.h"
#include "sheet.h"
#include <functional>
#include <unordered_set>

class Sheet;
struct Edges {
    // конец this
    std::unordered_set<CellInterface*> in_;
    // начало this
    std::unordered_set<CellInterface*> out_;
};
class Cell : public CellInterface {
public:
    Cell(Sheet& sheet);
    ~Cell();

    void Set(std::string text, const Position& pos_to_check);
    void SetEdges();
    void SetOutEdges(CellInterface* out_cell);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;

    std::vector<Position> GetReferencedCells() const override;
    bool IsReferenced() const;

    bool CheckCycles(std::unique_ptr<Impl> impl_to_check, const Position& pos_to_check);
    void dfs(std::unordered_set<Impl*>& used, const Position& pos);
    void InvalidateCache();
    bool CheckCache() const;
private:
    class EmptyImpl : public Impl {
    public:
        using Value = std::variant<std::string, double, FormulaError>;
        Value GetValue() const override {
            return GetText();
        }
        std::string GetText() const override {
            std::string s;
            return s;
        }
        void ClearCache() override {}
        bool CheckCache() const override {
            return false;
        }
        std::vector<Position> GetCells() const { return {}; }
    };
    class TextImpl : public Impl {
    public:
        using Value = std::variant<std::string, double, FormulaError>;
        TextImpl(std::string text) : text_(text) {}
        Value GetValue() const override {
            if (text_[0] == '\'') {
                std::string result;
                for (size_t i = 1; i < text_.size(); i++) {
                    result.push_back(text_[i]);
                }
                return result;
            }
            return GetText();
        }
        std::string GetText() const override {
            return text_;
        }
        std::vector<Position> GetCells() const { return {}; }
        void ClearCache() override {}
        bool CheckCache() const override {
            return false;
        }
    private:
        std::string text_;
    };
    class FormulaImpl : public Impl {
    public:
        using Value = std::variant<std::string, double, FormulaError>;
        FormulaImpl(std::string expr, const  SheetInterface& sheet) : formula_(std::move(ParseFormula(expr))), sheet_(sheet) {}
        Value GetValue() const override {
            if (!cache_.has_value()) {
                auto res = formula_->Evaluate(sheet_);
                if (std::holds_alternative<double>(res)) {
                    cache_ = std::get<double>(res);
                    return cache_.value();
                }
                else {
                    return std::get<FormulaError>(res);
                }
            }
            else {
                return cache_.value();
            }
        }
        std::string GetText() const override {
            std::string result("=");
            result += formula_->GetExpression();
            return result;
        }
        std::vector<Position> GetCells() const { return formula_->GetReferencedCells(); }
        void ClearCache() {
            cache_.reset();
        }
        bool CheckCache() const {
            return cache_.has_value();
        }
    private:
        std::unique_ptr<FormulaInterface> formula_;
        const  SheetInterface& sheet_;
        mutable std::optional<double> cache_;
    };
    Sheet& sheet_;
    std::unique_ptr<Impl> impl_;
    Edges edges_;
};