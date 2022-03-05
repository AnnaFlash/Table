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
        if (static_cast<int>(sheet_.size()) > pos.row) {
            if (static_cast<int>(sheet_[pos.row].size()) > pos.col) {
                if (sheet_[pos.row][pos.col].get() == nullptr) {
                    sheet_[pos.row][pos.col] = std::make_unique<Cell>(*this);
                }
                sheet_[pos.row][pos.col]->Set(text, pos);
            }
            else {
                sheet_[pos.row].resize(pos.col + 1);
                if (sheet_[pos.row][pos.col].get() == nullptr) {
                    sheet_[pos.row][pos.col] = std::make_unique<Cell>(*this);
                }
                sheet_[pos.row][pos.col]->Set(text, pos);
            }
        }
        else {
            sheet_.resize(pos.row + 1);
            sheet_[pos.row].resize(pos.col + 1);
            sheet_[pos.row][pos.col] = std::make_unique<Cell>(*this);
            sheet_[pos.row][pos.col]->Set(text, pos);
        }
    }
    else {
        throw InvalidPositionException("");
    }
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if (pos.IsValid()) {
        if (static_cast<int>(sheet_.size()) > pos.row) {
            if (static_cast<int>(sheet_[pos.row].size()) > pos.col) {
                return sheet_[pos.row][pos.col].get();
            }
            else {
                return nullptr;
            }
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
        if (static_cast<int>(sheet_.size()) > pos.row) {
            if (static_cast<int>(sheet_[pos.row].size()) > pos.col) {
                return sheet_[pos.row][pos.col].get();
            }
            else {
                return nullptr;
            }
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
        if (static_cast<int>(sheet_.size()) > pos.row) {
            if (static_cast<int>(sheet_[pos.row].size()) > pos.col) {
                sheet_[pos.row][pos.col].release();
                sheet_[pos.row].erase(sheet_[pos.row].begin() + pos.col);
            }
        }
    }
    else {
        throw InvalidPositionException("");
    }
    std::vector<bool> check_rows(sheet_.size());
    for (size_t i = 0; i < sheet_.size(); i++) {
        check_rows[i] = CheckRow(sheet_[i]);
    }
    for (int i = check_rows.size() - 1; i >= 0; i--) {
        if (check_rows[i] == false) {
            break;
        }
        sheet_.erase(sheet_.begin() + i);
    }
}

Size Sheet::GetPrintableSize() const {
   /* Size result;
    std::vector<bool> check_rows;
    for (size_t i = 0; i < sheet_.size(); i++) {
        if (sheet_[i].empty()) {
            { check_rows.push_back(false); }
        }
        else {
            bool check = false;
            for (auto& r : sheet_[i]) {
                r.get() != nullptr ? check = true : check;
            }
            check_rows.push_back(check); 
        }
    }
    for (int i = check_rows.size(); i > 0; i--) {
        if (check_rows[i - 1]) {
            result.rows = i;
            break;
        }
    }
    result.cols = 0;
    for (int i = 0; i < sheet_.size(); i++) {
        int col = 0;
        for (int j = 0; j < sheet_[i].size(); j++) {
            if (GetCell(Position{ i, j })) {
                if (sheet_[i][j]->GetText() != "") {
                    col = j + 1;
                }
            }
        }
        if (result.cols < col) {
            result.cols = col;
        }
    }
    return result;*/
    Size result;
    result.cols = 0;
    for (size_t i = 0; i < sheet_.size(); i++) {
        result.cols < static_cast<int>(sheet_[i].size()) ? result.cols = static_cast<int>(sheet_[i].size()) : result.cols;
    }
    result.rows = static_cast<int>(sheet_.size());
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
            if (GetCell(Position{ i, j })) {
                if (j == size_.cols - 1) {
                    PrintValue(output, GetCell(Position{ i, j }));
                }
                else {
                    PrintValue(output, GetCell(Position{ i, j }));
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
            if (GetCell(Position{ i, j })) {
                if (j == size_.cols - 1) {
                    output << sheet_[i][j]->GetText();
                }
                else {
                    output << sheet_[i][j]->GetText() << "\t";
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