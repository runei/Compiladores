#include <iostream>
#include <string>

std::string expr;
int pos = 0;
char token;
int n = 0;

struct expressao {
	std::string d, e;
};

typedef struct expressao expressao;

void letoken()
{
	token = expr[++pos];
}

int E(expressao & Eexp);

int F(expressao & Fexp)
{
	expressao Eexp;
	if (token == '(')
	{
		letoken();
		if (E(Eexp))
		{
			if (token == ')')
			{
				Fexp.d = '(' + Eexp.d + ')';
				Fexp.e = '(' + Eexp.e + ')';
				letoken();
				return 1;
			}
			return 0;
		}
		return 0;
	}
	else if (token >= '0' && token <= '9')
	{
		Fexp.d = '0';
		Fexp.e = token;
		letoken();
		return 1;
	}
	else if (token == 'x')
	{
		Fexp.d = '1';
		Fexp.e = token;
		letoken();
		return 1;
	}
	return 0;
}

int R2(expressao R2h, expressao & R2s)
{
	expressao R21h, R21s, Fexp;
	if (token == '*')
	{
		letoken();
		if (F(Fexp))
		{
			R21h.e = '(' + R2h.e + '*' + Fexp.e + ')';
			R21h.d = '(' + R2h.d + '*' + Fexp.e + '+' + Fexp.d + '*' + R2h.e + ')';
			if (R2(R21h, R21s))
			{
				R2s = R21s;
				return 1;
			}
			return 0;
		}
		return 0;
	}
	else
	{
		R2s = R2h;
		return 1;
	}
}

int T(expressao & Texp)
{
	expressao R2h, R2s, Fexp;
	if (F(Fexp))
	{
		R2h = Fexp;
		if (R2(R2h, R2s))
		{
			Texp = R2s;
			return 1;
		}
	}
	return 0;
}

int R(expressao Rh, expressao & Rs)
{
	expressao R1h, R1s, Texp;
	if (token == '+')
	{
		letoken();
		if (T(Texp))
		{
			R1h.e = '(' + Rh.e + '+' + Texp.e + ')';
			R1h.d = '(' + Rh.d + '+' + Texp.d + ')';
			if (R(R1h, R1s))
			{
				Rs = R1s;
				return 1;
			}
			return 0;
		}
		return 0;
	}
	else
	{
		Rs = Rh;
		return 1;
	}	
}

int E(expressao & Eexp)
{
	expressao Rh, Rs, Texp;
	if (T(Texp))
	{
		Rh = Texp;
		if (R(Rh, Rs))
		{
			Eexp = Rs;
			return 1;
		}
	}
	return 0;
}

int main()
{
	std::cout << "Expressao: ";
	std::cin >> expr;
	token = expr[0];
	expressao Eexpr;
	if (E(Eexpr))
	{
		std::cout << "Derivada:  " << Eexpr.d << std::endl;
	}
	else std::cout << "Erro!!!! pos=" << pos << std::endl;
	return 0;

}
