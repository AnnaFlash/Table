#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>


// Реализуйте следующие методы
Cell::Cell(Sheet& sheet) : sheet_(sheet) {}

Cell::~Cell() {}

void Cell::Set(std::string text, const Position& pos_to_check) {
	if (text.empty()) {
		impl_ = std::make_unique<EmptyImpl>();
	}
	else if (text[0] == '=' && text.size() > 1) {
		text.erase(0, 1);
		std::unique_ptr<Impl> impl_to_check = std::make_unique<FormulaImpl>(text, sheet_);
		if (CheckCycles(std::move(impl_to_check), pos_to_check)) {
			impl_ = std::make_unique<FormulaImpl>(text, sheet_);
			SetEdges();
			InvalidCash();
		}
	}
	else {
		impl_ = std::make_unique<TextImpl>(text);
	}
}
void Cell::SetEdges() {
	if (!edges_.in_.empty()) {
		for (auto it = edges_.in_.begin(); it != edges_.in_.end(); ) {
			it = edges_.in_.erase(it);
		}
	}
	std::vector<Position> cells = GetReferencedCells();
	if (!cells.empty()) {
		for (auto& cell : cells) {
			if (sheet_.GetCell(cell) == nullptr) {
				sheet_.SetCell(cell, "");
			}
			edges_.in_.insert(sheet_.GetCell(cell));
			sheet_.GetCell(cell)->SetOutEdges(this);
		}
	}
}

void Cell::SetOutEdges(CellInterface* out_cell) {
	if (!edges_.out_.empty()) {
		for (auto it = edges_.out_.begin(); it != edges_.out_.end(); ) {
			it = edges_.out_.erase(it);
		}
	}
	edges_.out_.insert(out_cell);
}
void Cell::Clear() {}

Cell::Value Cell::GetValue() const {
	return impl_->GetValue();
}
std::string Cell::GetText() const {
	return impl_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const
{
	return impl_->GetCells();
}

bool Cell::IsReferenced() const
{
	return false;
}

bool Cell::CheckCycles(const std::unique_ptr<Impl> impl_to_check, const Position& pos) {
	std::unordered_set<Impl*> used;
	used.insert(impl_.get());
	for (const auto& cell : impl_to_check->GetCells()) {
		if (cell == pos) {
			throw CircularDependencyException("");
		}
		else {
			if (sheet_.GetCell(cell)) {
				sheet_.GetCell(cell)->dfs(used, pos);
			}
		}
	}
	return true;
}

void Cell::dfs(std::unordered_set<Impl*>& used, const Position& pos) {
	if (used.count(impl_.get())==0){
		used.insert(impl_.get());
		for (const auto& cell : impl_->GetCells()) {
			if (cell == pos) {
				throw CircularDependencyException("");
			}
			else {
				sheet_.GetCell(cell)->dfs(used, pos);
			}
		}
	}
	else {
		throw CircularDependencyException("");
	}
}

bool Cell::CheckCash() const {
	return impl_->CheckCash();
}
void Cell::InvalidCash() {
	impl_->ClearCash();
	for (auto& out_refernces : edges_.out_) {
		if (out_refernces->CheckCash()) {
			out_refernces->InvalidCash();
		}
	}
}