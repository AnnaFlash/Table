#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text) {
    if (pos.IsValid()) {
        if (sheet1_.count(pos)) {
            if (sheet1_.at(pos).get() == nullptr) {
                sheet1_[pos] = std::make_unique<Cell>(*this);
            }
            sheet1_[pos]->Set(text, pos);
        }
        else {
            sheet1_[pos] = std::make_unique<Cell>(*this);
            sheet1_[pos]->Set(text, pos);
        }
    }
    else {
        throw InvalidPositionException("");
    }
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if (pos.IsValid()) {
        if (sheet1_.count(pos)) {
            return sheet1_.at(pos).get();
        }
        else {
            return nullptr;
        }
    }
    else {
        throw InvalidPositionException("");
    }
}
CellInterface* Sheet::GetCell(Position pos) {
    if (pos.IsValid()) {
        if (sheet1_.count(pos)) {
            return sheet1_[pos].get();
        }
        else {
            return nullptr;
        }
    }
    else {
        throw InvalidPositionException("");
    }
}

bool Sheet::CheckRow(std::vector<std::unique_ptr<CellInterface>>& row) const {
    if (row.empty()) {
        return true;
    }
    bool emptyness = true;
    for (auto& r : row) {
        r.get() != nullptr ? emptyness = false : emptyness;
    }
    return emptyness;
}

void Sheet::ClearCell(Position pos) {
    if (pos.IsValid()) {
        if (sheet1_.count(pos)) {
            sheet1_[pos].release();
            sheet1_.erase(pos);
        }
    }
    else {
        throw InvalidPositionException("");
    }
}

Size Sheet::GetPrintableSize() const {
    Size result;
    if (sheet1_.empty()) {
        result.cols = 0;
        result.rows = 0;
        return result;
    }
    auto it = --sheet1_.end();
    result.rows =((it)->first).row + 1;
    result.cols = 0;
    for (const auto& cell : sheet1_) {
        result.cols < cell.first.col ? result.cols = cell.first.col : result.cols;
    }
    result.cols++;
    return result;
}

void PrintValue(std::ostream& output, const CellInterface* cell) {
    const auto value = cell->GetValue();
    if (std::holds_alternative<std::string>(value)) {
        output << std::get<std::string>(value);
    }
    if (std::holds_alternative< double>(value)) {
        output << std::get< double>(value);
    }
    if (std::holds_alternative<FormulaError>(value)) {
        output << std::get<FormulaError>(value);
    }
}

void Sheet::PrintValues(std::ostream& output) const {
    Size size_ = GetPrintableSize();
    for (int i = 0; i < size_.rows; i++) {
        for (int j = 0; j < size_.cols; j++) {
            Position pos = { i, j };
            if (GetCell(pos)) {
                if (j == size_.cols - 1) {
                    PrintValue(output, GetCell(pos));
                }
                else {
                    PrintValue(output, GetCell(pos));
                    output << "\t";
                }
            }
            else {
                if (j != size_.cols - 1) {
                    output << "\t";
                }
            }
        }
        output << "\n";
    }
}
void Sheet::PrintTexts(std::ostream& output) const {
    Size size_ = GetPrintableSize();
    for (int i = 0; i < size_.rows; i++) {
        for (int j = 0; j < size_.cols; j++) {
            Position pos = { i, j };
            if (GetCell(pos)) {
                if (j == size_.cols - 1) {
                    output << sheet1_.at(pos)->GetText();
                }
                else {
                    output << sheet1_.at(pos)->GetText() << "\t";
                }
            }
            else {
                if (j != size_.cols - 1) {
                    output << "\t";
                }
            }
        }
        output << "\n";
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}