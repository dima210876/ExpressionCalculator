#include "pch.h"
#include <iostream>
#include <string>

using namespace std;

struct stack
{
	double inf;
	stack *next;
};

stack* Add_In_Stack(stack *, const double&);
void Read_From_Stack(stack*&, double&);
int CheckCorrect(const char const*);
void FindVariables(const char const*, bool* const&, double* const&);
int priority(const char&);
void Translate_To_OPZ(const char const*, char*);
double Calculate(const char const*, const double const*, bool&);
stack* Delete_Stack(stack*);

int main()
{
	setlocale(LC_ALL, "Russian");
	char input[100], output[100];
	cout << "Вводить допускается лишь символы латинского алфавита нижнего регистра и символы '+', '-', '*', '/', '^', '(', ')'." << endl << "В противном случае запись считается некорректной!" << endl;
	cout << "Введите выражение для вычисления: " << endl;
	gets_s(input);
	int errcode = CheckCorrect(input);
	if (errcode)
	{
		cout << "Некорректное выражение: ";
		switch (errcode)
		{
		case 0: break;
		case 1: cout << "неверный первый символ." << endl; break;
		case 2: cout << "неверный последний символ." << endl; break;
		case 3: cout << "операция после открывающей скобки." << endl; break;
		case 4: cout << "операция перед закрывающей скобкой." << endl; break;
		case 5: cout << "в выражении недопустимый символ." << endl; break;
		case 6: cout << "нарушен порядок следования операндов и операций." << endl; break;
		case 7: cout << "недопустимое сочетание скобок \"()\" ." << endl; break;
		case 8: cout << "нарушен порядок следования открывающих и закрывающих скобок." << endl; break;
		case 9: cout << "нарушен количественный баланс открывающих и закрывающих скобок." << endl; break;
		}
		return 0;
	}

	bool VarTable[26] = { 0 };
	double Variables[26];
	FindVariables(input, VarTable, Variables);
	Translate_To_OPZ(input, output);
	cout << endl << "Полученная ОПЗ: " << output << endl;
	bool divisionbyzero = 0;
	double result = Calculate(output, Variables, divisionbyzero);
	if (!divisionbyzero) cout << "Результат вычислений: " << result << endl;
	return 0;
}

stack* Add_In_Stack(stack *sp, const double& inf)
{
	stack *spt = new stack;
	spt->inf = inf;
	spt->next = sp;
	return spt;
}

void Read_From_Stack(stack *&sp, double& inf)
{
	stack *spt = sp;
	inf = sp->inf;
	sp = sp->next;
	delete spt;
	return;
}

int CheckCorrect(const char const* a)
{
	int i = 0, len = strlen(a);
	if ((a[0] < 97 || a[0] > 122) && a[0] != '(') return 1;
	if ((a[len - 1] < 97 || a[len - 1] > 122) && a[len - 1] != ')') return 2;
	char pred = -1, cur, balance = 0;
	//0-операция, 1-операнд, 2-открыв.ск., 3-закрыв.ск.
	while (i != len)
	{
		if (a[i] == '+' || a[i] == '-' || a[i] == '*' || a[i] == '/' || a[i] == '^')
		{
			cur = 0;
			if (pred == 2) return 3;
		}
		else if (a[i] > 96 && a[i] < 123) { cur = 1; if (pred == 3) return 6; }
		else if (a[i] == '(') { cur = 2; if (pred == 1 || pred == 3) return 6; balance++; }
		else if (a[i] == ')') { cur = 3; if (pred == 0) return 4; balance--; }
		else return 5;
		if (cur == pred && cur < 2) return 6;
		else if (cur == 3 && pred == 2) return 7;
		if (balance < 0) return 8;
		pred = cur;
		i++;
	}
	if (balance != 0) return 9;
	return 0;
}

void FindVariables(const char const* a, bool* const& VarTable, double* const& Variables)
{
	int i = 0;
	cout << endl;
	for (; a[i] != '\0'; i++)
		if (a[i] > 96 && a[i] < 123)
			if (!VarTable[a[i] - 97])
			{
				VarTable[a[i] - 97]++;
				cout << "Введите значение переменной " << a[i] << ": ";
				cin >> Variables[a[i] - 97];
			}
	return;
}

int priority(const char& symbol)
{
	switch (symbol)
	{
	case '(': case ')': return 0;
	case '+': case '-': return 1;
	case '*': case '/': return 2;
	case '^': return 3;
	default: return -1;
	}
}

void Translate_To_OPZ(const char const* input, char* output)
{
	stack* sp = NULL;
	int n = 0, len = strlen(input);
	char symbol;
	double inf;
	for (int i = 0; i < len; i++)
	{
		symbol = input[i];
		if (symbol > 96 && symbol < 123) { output[n++] = symbol; continue; }
		if (sp == NULL || symbol == '(') { sp = Add_In_Stack(sp, symbol); continue; }
		if (symbol == ')')
		{
			while (sp->inf != '(')
			{
				Read_From_Stack(sp, inf);
				output[n++] = static_cast <char> (inf);
			}
			Read_From_Stack(sp, inf);
			continue;
		}
		int prior = priority(symbol);
		while (sp && priority(static_cast <char> (sp->inf)) >= prior)
		{
			Read_From_Stack(sp, inf);
			output[n++] = static_cast <char> (inf);
		}
		sp = Add_In_Stack(sp, symbol);
	}
	while (sp)
	{
		Read_From_Stack(sp, inf);
		output[n++] = static_cast <char> (inf);
	}
	output[n++] = '\0';
}

double Calculate(const char const* output, const double const* Variables, bool& divisionbyzero)
{
	stack *sp = NULL;
	char symbol;
	double v1, v2;
	int len = strlen(output);
	for (int i = 0; i < len; i++)
	{
		symbol = output[i];
		if (symbol > 96 && symbol < 123) { sp = Add_In_Stack(sp, Variables[static_cast <int> (symbol) - 97]); continue; }
		Read_From_Stack(sp, v2);
		Read_From_Stack(sp, v1);
		switch (symbol)
		{
		case '+': sp = Add_In_Stack(sp, v1 + v2); break;
		case '-': sp = Add_In_Stack(sp, v1 - v2); break;
		case '*': sp = Add_In_Stack(sp, v1 * v2); break;
		case '/':
			if (v2 == 0)
			{
				cout << "Деление на 0!" << endl;
				Delete_Stack(sp);
				divisionbyzero = 1;
				return 0;
			}
			sp = Add_In_Stack(sp, v1 / v2); break;
		case '^': sp = Add_In_Stack(sp, pow(v1, v2)); break;
		}
	}
	Read_From_Stack(sp, v1);
	return v1;
}

stack* Delete_Stack(stack *sp)
{
	stack* current;
	while (sp != NULL)
	{
		current = sp;
		sp = sp->next;
		delete current;
	}
	return NULL;
}

