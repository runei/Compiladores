#include <iostream>
#include <fstream>
#include <string>

enum EToken
{
	TKFim,
	TKErro,
	TKId,
	TKInt,
	TKFloat,
	TKDouble,
	TKChar,
	TKVirgula,
	TKPontoVirgula,
	TKAbreParenteses,
	TKFechaParenteses,
	TKAbreColchete,
	TKFechaColchete,
	TKAbreChaves,
	TKFechaChaves,
	TKAtrib,
	TKConst,
	TKSoma,
	TKSubtracao,
	TKMult,
	TKDiv,
	TKMaior,
	TKMenor,
	TKIgual,
	TKWhile,
	TKFor,
	TKDo,
	TKBreak,
	TKContinue
};

enum EEstado
{
	ESTGeral,
	ESTId,
	ESTConst,
	ESTIgual
};

/***********************************************************************************/
/*                                                                                 */
/*  INÍCIO DO LÉXICO - Não entre a não ser que tenha interesse pessoal em léxicos  */
/*                                                                                 */
/***********************************************************************************/

struct palavra_reservada {
	std::string palavra;
	EToken token;
};

const struct palavra_reservada lista_palavras[]
{
	{ "int", TKInt },
	{ "float", TKFloat },
	{ "double", TKDouble },
	{ "char", TKChar },
	{ "while", TKWhile },
	{ "for", TKFor },
	{ "do", TKDo },
	{ "break", TKBreak },
	{ "continue", TKContinue },
	{ "fim", TKId }
};

std::ifstream arq("prog.c");
std::string lex{ "" };
EToken token;
int cont{ 0 }, linha{ 0 }, coluna{ 0 };

char le_char()
{
	char *c = new char[1];
	arq.read(c, 1);
	coluna++;
	if (arq)
	{
		return c[0];
	}
	return '\0';
}

EToken tk_palavra_reservada(const std::string lex)
{
	int pos{ 0 };
	while (lista_palavras[pos].palavra != "fim")
	{
		if (lista_palavras[pos].palavra == lex)
		{
			return lista_palavras[pos].token;
		}
		++pos;
	}
	return TKId;
}

EToken ret_token()
{
	EEstado estado{ ESTGeral };
	static char c{ '\n' };
	lex = "";

	while (1)
	{
		switch (estado)
		{
		case ESTGeral:

			lex = c;
			c = le_char();

			if ((lex[0] >= 'a' && lex[0] <= 'z') || (lex[0] >= 'A' && lex[0] <= 'Z') || lex[0] == '_')
			{
				estado = ESTId;
				break;
			}

			if (lex[0] >= '0' && lex[0] <= '9')
			{
				estado = ESTConst;
				break;
			}

			if (lex[0] == '\n' || lex[0] == '\r')
			{
				linha++;
				coluna = 0;
				break;
			}

			if (lex[0] == '\t' || lex[0] == ' ')
			{
				break;
			}

			if (lex[0] == '=') {
				estado = ESTIgual;
				break;
			};

			if (lex[0] == ',') { return TKVirgula; };
			if (lex[0] == ';') { return TKPontoVirgula; };
			if (lex[0] == '(') { return TKAbreParenteses; };
			if (lex[0] == ')') { return TKFechaParenteses; };
			if (lex[0] == '[') { return TKAbreColchete; };
			if (lex[0] == ']') { return TKFechaColchete; };
			if (lex[0] == '{') { return TKAbreChaves; };
			if (lex[0] == '}') { return TKFechaChaves; };
			if (lex[0] == '+') { return TKSoma; };
			if (lex[0] == '-') { return TKSubtracao; };
			if (lex[0] == '*') { return TKMult; };
			if (lex[0] == '/') { return TKDiv; };
			if (lex[0] == '>') { return TKMaior; };
			if (lex[0] == '<') { return TKMenor; };
			if (lex[0] == '\0') { return TKFim; };
			return TKErro;

		case ESTId:
			if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || (c >= '0' && c <= '9'))
			{
				lex.push_back(c);
				c = le_char();
				break;
			}
			else
			{
				return tk_palavra_reservada(lex);
			}

		case ESTConst:
			if (c >= '0' && c <= '9')
			{
				lex.push_back(c);
				c = le_char();
				break;
			}
			else
			{
				return TKConst;
			}

		case ESTIgual:
			if (c == '=')
			{
				lex.push_back(c);
				c = le_char();
				return TKIgual;
			}
			else
			{
				return TKAtrib;
			}

		}
	}

}

void le_token()
{
	token = ret_token();
}

/***********************************************************************************/
/*																				   */
/*  FIM DO LÉXICO																   */
/*                                                                                 */
/***********************************************************************************/

int CMD(std::string & CMDc, const std::string & rotuloInicial, const std::string & rotuloFinal);
int M(std::string & Mc);

std::string gera_rotulo()
{
	++cont;
	std::string res{ "L" + std::to_string(cont) };
	return res;
}

int P(std::string & Pc)
{
	if (token == TKAbreParenteses)
	{
		le_token();
		std::string Mc;
		if (M(Mc))
		{
			if (token == TKFechaParenteses)
			{
				le_token();
				Pc = Mc;
				return 1;
			}
		}
	}
	return 0;
}

int F(std::string & Fc)
{
	std::string Pc;
	if (token == TKId)
	{
		Fc = "\tvalor-r " + lex + "\n";
		le_token();
		return 1;
	}
	else if (token == TKConst)
	{
		Fc = "\tpush " + lex + "\n";
		le_token();
		return 1;
	}
	else if (P(Pc)) {
		Fc = Pc;
		return 1;
	}
	return 0;
}

int R2(const std::string R2h, std::string & R2s)
{
	std::string R21h, R21s, Fc;
	if (token == TKMult)
	{
		le_token();
		if (F(Fc))
		{
			R21h = R2h + Fc + "\t*\n";
			if (R2(R21h, R21s))
			{
				R2s = R21s;
				return 1;
			}
		}
	}
	else if (token == TKDiv)
	{
		le_token();
		if (F(Fc))
		{
			R21h = R2h + Fc + "\t/\n";
			if (R2(R21h, R21s))
			{
				R2s = R21s;
				return 1;
			}
		}
	}
	else
	{
		R2s = R2h;
		return 1;
	}
	return 0;
}

int T(std::string & Tc)
{
	std::string Fc, R2h, R2s;
	if (F(Fc))
	{
		R2h = Fc;
		if (R2(R2h, R2s))
		{
			Tc = R2s;
			return 1;
		}
	}
	return 0;
}

int R(const std::string Rh, std::string & Rs)
{
	std::string R1h, R1s, Tc;
	if (token == TKSoma)
	{
		le_token();
		if (T(Tc))
		{
			R1h = Rh + Tc + "\t+\n";
			if (R(R1h, R1s))
			{
				Rs = R1s;
				return 1;
			}
		}
	}
	else if (token == TKSubtracao)
	{
		le_token();
		if (T(Tc))
		{
			R1h = Rh + Tc + "\t-\n";
			if (R(R1h, R1s))
			{
				Rs = R1s;
				return 1;
			}
		}
	}
	else
	{
		Rs = Rh;
		return 1;
	}
	return 0;
}

int E(std::string & Ec)
{
	std::string Rh, Rs, Tc;
	if (T(Tc))
	{
		Rh = Tc;
		if (R(Rh, Rs))
		{
			Ec = Rs;
			return 1;
		}
	}
	return 0;
}

int L(std::string & Lc, const std::string & rotuloInicial, const std::string & rotuloFinal)
{
	std::string CMDc;
	if (CMD(CMDc, rotuloInicial, rotuloFinal))
	{
		if (L(Lc, rotuloInicial, rotuloFinal))
		{
			Lc = CMDc + Lc;
			return 1;
		}
	}
	else
	{
		Lc = "";
		return 1;
	}
	return 0;
}

int lista_com(std::string & lista_com_c, const std::string & rotuloInicial, const std::string & rotuloFinal)
{
	std::string CMDc, Lc;
	if (CMD(CMDc, rotuloInicial, rotuloFinal))
	{
		if (L(Lc, rotuloInicial, rotuloFinal))
		{
			lista_com_c = CMDc + Lc;
			return 1;
		}
	}
	return 0;
}

int M(std::string & Mc)
{
	std::string E1c, E2c;
	if (E(E1c))
	{
		if (token == TKMaior)
		{
			le_token();
			if (E(E2c))
			{
				Mc = E1c + E2c + "\t>\n";
				return 1;
			}
		}
		else if (token == TKMenor)
		{
			le_token();
			if (E(E2c))
			{
				Mc = E1c + E2c + "\t<\n";
				return 1;
			}
		}
		else if (token == TKIgual)
		{
			le_token(); std::cout << lex;
			if (E(E2c))
			{
				Mc = E1c + E2c + "\t=\n";
				return 1;
			}
		}
		else
		{
			Mc = E1c;
			return 1;
		}
	}
	return 0;
}

int A(std::string & Ac)
{
	if (token == TKId)
	{
		std::string idnome{ lex };
		le_token();
		if (token == TKAtrib)
		{
			le_token();
			std::string Mc;
			if (M(Mc))
			{
				Ac = "\tvalor-l " + idnome + "\n" + Mc + "\t:=\n\tpop\n";
				return 1;
			}
		}
	}
	return 0;
}

int CMD(std::string & CMDc, const std::string & rotuloInicial, const std::string & rotuloFinal)
{
	std::string Ac;
	if (token == TKAbreChaves)
	{
		le_token();
		std::string lista_com_c;
		if (lista_com(lista_com_c, rotuloInicial, rotuloFinal))
		{
			if (token == TKFechaChaves)
			{
				le_token();
				CMDc = lista_com_c;
				return 1;
			}
		}
	}
	else if (A(Ac))
	{
		if (token == TKPontoVirgula)
		{
			le_token();
			CMDc = Ac;
			return 1;
		}
	}
	else if (token == TKWhile)
	{
		le_token();
		std::string Pc, CMD1c;
		if (P(Pc))
		{
			const std::string inicio{ gera_rotulo() };
			const std::string saida{ gera_rotulo() };
			if (CMD(CMD1c, inicio, saida))
			{
				CMDc = "rotulo " + inicio + ":\n" + Pc + "\tgofalse " + saida + "\n" + CMD1c + "\tgoto " + inicio + "\n" + "rotulo " + saida + ":\n";
				return 1;
			}
		}
	}
	else if (token == TKFor)
	{
		le_token();
		if (token == TKAbreParenteses)
		{
			le_token();
			std::string A1c, Mc, A2c, CMD1c;
			if (A(A1c))
			{
				if (token == TKPontoVirgula)
				{
					le_token();
					if (M(Mc))
					{
						if (token == TKPontoVirgula)
						{
							le_token();
							if (A(A2c))
							{
								if (token == TKFechaParenteses)
								{
									le_token();
									const std::string inicio{ gera_rotulo() };
									const std::string saida{ gera_rotulo() };
									if (CMD(CMD1c, inicio, saida))
									{
										CMDc = A1c + "rotulo " + inicio + ":\n" + Mc + "\tgofalse " + saida + "\n" + CMD1c + A2c + "\tgoto " + inicio + "\n" + "rotulo " + saida + ":\n";
										return 1;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else if (token == TKDo)
	{
		le_token();
		std::string CMD1c, Pc;
		const std::string inicio{ gera_rotulo() };
		const std::string saida{ gera_rotulo() };
		if (CMD(CMD1c, inicio, saida))
		{
			if (token == TKWhile)
			{
				le_token();
				if (P(Pc))
				{
					if (token == TKPontoVirgula)
					{
						le_token();
						CMDc = "rotulo " + inicio + ":\n" + CMD1c + Pc + "\tgofalse " + saida + "\n" + "\tgoto " + inicio + "\n" + "rotulo " + saida + ":\n";
						return 1;
					}
				}
			}
		}
	}
	else if (token == TKBreak)
	{
		le_token();
		if (token == TKPontoVirgula)
		{
			le_token();
			CMDc = "\tgoto " + rotuloFinal + "\n";
			return 1;
		}
	}
	else if (token == TKContinue)
	{
		le_token();
		if (token == TKPontoVirgula)
		{
			le_token();
			CMDc = "\tgoto " + rotuloInicial + "\n";
			return 1;
		}
	}
	return 0;
}

int main()
{
	le_token();
	std::string CMDc;
	std::ofstream res("pilha.stk");
	while (token != TKFim)
	{
		if (CMD(CMDc, "", ""))
		{
			res << CMDc;
		}
		else
		{
			res << "Erro linha: " << linha << " coluna: " << coluna << std::endl;
			break;
		}
	}
	arq.close();
	res.close();

	return 0;
}