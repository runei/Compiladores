#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <utility>

enum EToken
{
	TKFim,
	TKErro,
	TKId,
	TKInt,
	TKShortInt,
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
	{ "short int", TKShortInt },
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
	char c = arq.get();
	if(c == EOF) return '\0';
	return c;
}

EToken tk_palavra_reservada(const std::string & lex)
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

struct Var 
{
	std::string cod;
	std::string place;

	Var (const std::string & c, const std::string & p) : cod (c) , place (p) {}
	Var () {}
};

void debug_var(const Var & v, const std::string & message)
{
	std::cout 
		<< message << "\n"
		<< "cod\n" 
		<< v.cod << "\n"
		<< "place\n"
		<< v.place << std::endl;
}

int tmp_n = 0;

inline std::string gera_tmp() 
{
	return std::string{"T" + std::to_string(tmp_n++)};
}

inline std::string gen(const std::string & op, const std::string & tmp, const std::string & place1, const std::string & place2)
{
	if(op == "=") return std::string{tmp + "=" + place1 + "\n"};
	return std::string{tmp + "=" + place1 + op + place2 + "\n"};
}

int E(Var &);

int F(Var & Fs)
{
	if(token == TKId)
	{
		Fs.place = lex;
		le_token();
		Fs.cod = "";
		return 1;
	}
	else if (token == TKConst) 
	{
		Fs.place = gera_tmp();
		Fs.cod = gen("=", Fs.place, lex, "");
		le_token();
		return 1;
	}
	else if (token == TKAbreParenteses)
	{
		le_token();
		Var Es;
		if (E(Es)) 
		{
			if (token == TKFechaParenteses)
			{
				le_token();
				Fs.cod = Es.cod;
				Fs.place = Es.place;
				return 1;
			}
		}
	}
	return 0;
}

int P(const Var & Ph, Var & Ps)
{
	if(token == TKMult)
	{
		le_token();
		Var Fs;
		if(F(Fs)) 
		{
			std::string tmp_P1Ph = gera_tmp();
			Var P1h { Ph.cod + Fs.cod + gen("*", tmp_P1Ph, Ph.place, Fs.place), tmp_P1Ph };
			Var P1s;
			if(P(P1h, P1s))
			{
				Ps = P1s;
				return 1;
			}
		}
		return 0;
	}
	else
	{
		Ps = Ph;
	}
	return 1;
}

int T(Var & Ts)
{
	Var Fs;
	if(F(Fs))
	{
		Var Ph { Fs }, Ps;
		if(P(Ph, Ps))
		{
			Ts = Ps;
			return 1;
		}
	}
	return 0;
}

int R(const Var & Rh, Var & Rs)
{
	if(token == TKSoma)
	{
		le_token();
		Var Ts;
		if(T(Ts)) 
		{
			std::string tmp_R1ph = gera_tmp();
			Var R1h { Rh.cod + Ts.cod + gen("+", tmp_R1ph, Rh.place, Ts.place), tmp_R1ph };
			Var R1s;
			if(R(R1h, R1s))
			{
				Rs = R1s;
				return 1;
			}
		}
		return 0;
	}
	else
	{
		Rs = Rh;
	}
	return 1;
}

int E(Var & Es)
{
	Var Ts;
	if(T(Ts))
	{
		Var Rh { Ts.cod, Ts.place }, Rs;
		if(R(Rh, Rs))
		{
			Es = Rs;
			return 1;
		}
	}
	return 0;
}


int main()
{
	le_token();
	std::ofstream res("pilha.stk");
	while (token != TKFim)
	{
		Var Es;
		if (E(Es))
		{
			res << "E Place\n" << Es.place << "\nEcod\n" << Es.cod << std::endl;
		}
		else
		{
			res << "Erro linha: " << linha << " coluna: " << coluna << std::endl;
			break;
		}
	}

	return 0;
}
