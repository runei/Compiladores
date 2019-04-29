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

struct id {
	EToken tipo;
	int tam;
};

int T(struct id & Tc)
{
	if (token == TKInt)
	{
		Tc.tipo = token;
		Tc.tam = 4;
		le_token();
		return 1;
	}
	else if (token == TKFloat)
	{
		Tc.tipo = token;
		Tc.tam = 4;
		le_token();
		return 1;
	}
	else if (token == TKDouble)
	{
		Tc.tipo = token;
		Tc.tam = 8;
		le_token();
		return 1;
	}
	return 0;
}

int V(struct id & Vc)
{
	if (token == TKId)
	{
		le_token();
	}
	return 0;
}

int R(struct id & Rc)
{
	if (token == TKVirgula)
	{
		le_token();
		struct id Vc(Rc);
		if (V(Vc))
		{
			struct id R1c(Rc);
			if (R(R1c))
			{
				return 1;
			}
		}
	}
	else
	{
		return 1;
	}
	return 0;
}

int D(std::string & Dc)
{
	struct id Tc;
	if (T(Tc))
	{
		struct id Vc(Tc);
		if (V(Vc))
		{
			struct id Rc(Tc);
			if (R(Rc))
			{
				return 1;
			}
		}
	}
	return 0;
}

int main()
{
	le_token();
	std::string Dc;
	while (token != TKFim)
	{
		if (D(Dc))
		{
			std::cout << Dc;
		}
		else
		{
			std::cout << "Erro linha: " << linha << " coluna: " << coluna << std::endl;
			break;
		}
	}
	arq.close();

	return 0;
}