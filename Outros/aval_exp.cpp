#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int pos = 0;
char expr[20];

int E(float *Eval);

int H(float *Hval)
{
	float Eval;
	if (expr[pos] >= '0' && expr[pos] <= '9')
	{
		*Hval = expr[pos++] - '0';
		return 1;
	}
	else if (expr[pos] == '(')
	{
		pos++;
		if (E(&Eval))
		{
			*Hval = Eval;
			if (expr[pos] == ')')
			{
				pos++;
				return 1;
			}
			return 0;
		}
		return 0;
	}
	else if (expr[pos] == 'r')
	{
		pos++;
		if (expr[pos] == '(')
		{
			pos++;
			if (E(&Eval))
			{
				*Hval = sqrt(Eval);
				if (expr[pos] == ')')
				{
					pos++;
					return 1;
				}
				return 0;
			}
			return 0;
		}
		return 0;
	}
	return 0;
}

int F(float *Fval)
{
	float Hval;		
	if (expr[pos] == '-')
	{
		pos++;
		if (F(Fval))
		{
			*Fval = (*Fval) * -1;	
			return 1;
		}
		return 0;
	}
	else if (H(&Hval))
	{
		*Fval = Hval;
		return 1;
	}
	return 0;
}

int R3(float R3h, float *R3s)
{
	float R31h, R31s, Fval;
	if (expr[pos] == '^')
	{
		pos++;
		if (F(&Fval))
		{
			R31h = pow(R3h, Fval);
			if (R3(R31h, &R31s))
			{
				*R3s = R31s;
				return 1;
			}
			return 0;
		}
		return 0;
	}
	else
	{
		*R3s = R3h;
		return 1;
	}
}

int Y(float *Yval)
{
	float R3h, R3s, Fval;
	if (F(&Fval))
	{
		R3h = Fval;
		if (R3(R3h, &R3s))
		{
			*Yval = R3s;
			return 1;
		}
		return 0;
	}
	return 0;
}

int R2(float R2h, float *R2s)
{
	float R21h, R21s, Yval;
	if (expr[pos] == '*')
	{
		pos++;
		if (Y(&Yval))
		{
			R21h = R2h * Yval;
			if (R2(R21h, &R21s))
			{
				*R2s = R21s;
				return 1;
			}
			return 0;
		}
		return 0;
	}
	else if (expr[pos] == '/')
	{
		pos++;
		if (Y(&Yval))
		{
			R21h = R2h / Yval;
			if (R2(R21h, &R21s))
			{
				*R2s = R21s;
				return 1;
			}
			return 0;
		}
	}
	else
	{
		*R2s = R2h;
		return 1;
	}
}

int T(float *Tval)
{
	float Yval, R2h, R2s;
	if (Y(&Yval))
	{
		R2h = Yval;
		if (R2(R2h, &R2s))
		{
			*Tval = R2s;
			return 1;
		}
		return 0;
	}
	return 0;
}

int R(float Rh, float *Rs)
{
	float R1h, R1s, Tval;
	if (expr[pos] == '+')
	{
		pos++;
		if (T(&Tval))
		{
			R1h = Rh + Tval;
			if (R(R1h, &R1s))
			{
				*Rs = R1s;
				return 1;
			}
			return 0;
		}
		return 0;
	}
	else if (expr[pos] == '-')
	{
		pos++;
		if (T(&Tval))
		{
			R1h = Rh - Tval;
			if (R(R1h, &R1s))
			{
				*Rs = R1s;
				return 1;
			}
			return 0;
		}
		return 0;
	}
	else
	{
		*Rs = Rh;
		return 1;
	}
}

int E(float *Eval)
{
	float Tval, Rh, Rs;
	if (T(&Tval))
	{
		Rh = Tval;
		if (R(Rh, &Rs))
		{
			*Eval = Rs;
			return 1;
		}
		return 0;
	}
	return 0;
}

int main()
{
	float Eval;
	printf("Digite a exprressao:");
	scanf("%s", &expr);
	if (E(&Eval)) printf("O valor é:%f\n", Eval);
	else printf("Erro na posição %d\n", pos);
//	system("pause");
}
