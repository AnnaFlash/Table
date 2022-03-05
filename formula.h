#pragma once

#include "common.h"
#include <memory>
#include <vector>

// �������, ����������� ��������� � ��������� �������������� ���������.
// �������������� �����������:
// * ������� �������� �������� � �����, ������: 1+2*3, 2.5*(2+3.5/7)
// * �������� ����� � �������� ����������: A1+B2*C3
// ������, ��������� � �������, ����� ���� ��� ���������, ��� � �������. ���� ���
// �����, �� �� ������������ �����, ����� ��� ����� ���������� ��� �����. ������
// ������ ��� ������ � ������ ������� ���������� ��� ����� ����.
class FormulaInterface {
public:
    using Value = std::variant<double, FormulaError>;

    virtual ~FormulaInterface() = default;

    // �������� ��������, ��� � ����� Evaluate() ������ �� ������� ��������� 
    // � �������� ���������.
    // ���������� ����������� �������� ������� ��� ����������� ����� ���� ������.
    // ���� ���������� �����-�� �� ��������� � ������� ����� �������� � ������, ��
    // ������������ ������ ��� ������. ���� ����� ������ ���������, ������������
    // �����.
    virtual Value Evaluate(const SheetInterface& sheet) const = 0;

    // ���������� ���������, ������� ��������� �������.
    // �� �������� �������� � ������ ������.
    virtual std::string GetExpression() const = 0;

    // ���������� ������ �����, ������� ��������������� ������������� � ����������
    // �������. ������ ������������ �� ����������� � �� �������� �������������
    // �����.
    virtual std::vector<Position> GetReferencedCells() const = 0;
};

// ������ ���������� ��������� � ���������� ������ �������.
// ������� FormulaException � ������, ���� ������� ������������� �����������.
std::unique_ptr<FormulaInterface> ParseFormula(std::string expression);