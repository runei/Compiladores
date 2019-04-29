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
	TKMaiorIgual,
	TKMenor,
	TKMenorIgual,
	TKIgual,
	TKWhile,
	TKFor,
	TKDo,
	TKBreak,
	TKContinue,
	TKIf,
	TKElse,
	TKSwitch,
	TKCase,
	TKDefault,
	TKDoisPontos,
	TKOu,
	TKE,
	TKNot,
	TKDiferente,
	TKResto
};

enum EEstado
{
	ESTGeral,
	ESTId,
	ESTConst,
	ESTIgual,
	ESTOu,
	ESTE,
	ESTMaior,
	ESTMenor,
	ESTDiferente
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
	{ "if", TKIf },
	{ "else", TKElse },
	{ "switch", TKSwitch },
	{ "case", TKCase },
	{ "default", TKDefault },
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
	coluna++;
	return c;
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

			if (lex[0] == '!') {
				estado = ESTDiferente;
				break;
			};

			if (lex[0] == '<') {
				estado = ESTMenor;
				break;
			};

			if (lex[0] == '>') {
				estado = ESTMaior;
				break;
			};

			if (lex[0] == '|') {
				estado = ESTOu;
				break;
			};

			if (lex[0] == '&') {
				estado = ESTE;
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
			if (lex[0] == ':') { return TKDoisPontos; };
			if (lex[0] == '%') { return TKResto; };
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

		case ESTDiferente:
			if (c == '=')
			{
				lex.push_back(c);
				c = le_char();
				return TKDiferente;
			}
			else
			{
				return TKNot;
			}

		case ESTMenor:
			if (c == '=')
			{
				lex.push_back(c);
				c = le_char();
				return TKMenorIgual;
			}
			else
			{
				return TKMenor;
			}

		case ESTMaior:
			if (c == '=')
			{
				lex.push_back(c);
				c = le_char();
				return TKMaiorIgual;
			}
			else
			{
				return TKMaior;
			}

		case ESTOu:
			if (c == '|')
			{
				lex.push_back(c);
				c = le_char();
				return TKOu;
			}

		case ESTE:
			if (c == '&')
			{
				lex.push_back(c);
				c = le_char();
				return TKE;
			}
		}
	}

}

void le_token()
{
	token = ret_token();
}

void setEstado(bool set)
{
	static int static_pos_arq, static_tmp_col, static_tmp_linha;
	static EToken static_tmp_tk;
	static std::string static_tmp_lex;
	if (set) 
	{
		static_pos_arq = arq.tellg();
		static_tmp_tk = token;
		static_tmp_lex = lex;
		static_tmp_col = coluna;
		static_tmp_linha = linha;
	}
	else
	{
		arq.seekg(static_pos_arq);
		token = static_tmp_tk;
		lex = static_tmp_lex;
		coluna = static_tmp_col;
		linha = static_tmp_linha;
	}
}

/***********************************************************************************/
/*																				   */
/*  FIM DO LÉXICO																   */
/*                                                                                 */
/***********************************************************************************/

struct tipo {
	std::string place;
	std::string cod;
	std::string incr;
};

int contTemp = 1;

std::string geraTemp()
{
	std::string res = "T" + std::to_string(contTemp);
	contTemp++;
	return res;
}

int F(struct tipo & Ft)
{
	setEstado(true);
	if (token == TKSoma)
	{
		le_token();
		if (token == TKSoma)
		{
			le_token();
			Ft.incr = Ft.place + " = " + Ft.place + " + 1\n";
			return 1;
		}
	}
	setEstado(false);
	Ft.incr = "";
	return 1;
}

int T(struct tipo & Tt);

int R(struct tipo & Rt)
{
	if (token == TKSoma)
	{
		le_token();
		struct tipo Tt;
		if (T(Tt))
		{
			struct tipo R1t;
			R1t.place = Rt.place;
			R1t.cod = Rt.cod + Tt.cod + R1t.place + " = " + R1t.place + " + " + Tt.place + "\n";
			R1t.incr = Rt.incr;
			if (R(R1t))
			{
				Rt = R1t;
				return 1;
			}
		}
	}
	else
	{
		return 1;	
	}
	
}

int T(struct tipo & Tt)
{
	if (token == TKConst)
	{
		Tt.place = geraTemp();
		Tt.cod = Tt.place + " = " + lex + "\n";
		Tt.incr = "";
		le_token();
		return 1;
	}
	else if (token == TKId)
	{
		struct tipo Ft;
		Tt.place = lex;
		Tt.cod = "";
		Ft.place = Tt.place;
		le_token();
		if (F(Ft))
		{
			Tt.incr = Ft.incr;
			return 1;
		}
	}
	return 0;
}

int E(struct tipo & Et)
{
	struct tipo Tt;
	if (T(Tt))
	{
		struct tipo Rt;
		Rt = Tt;
		if (R(Rt))
		{
			Et.place = geraTemp();
			Et.incr = Rt.incr;
			Et.cod = Rt.cod + Et.incr;
			return 1;
		}
	}
	return 0;
}

int main()
{
	le_token();
	while (token != TKFim)
	{
		struct tipo Et;
		if (E(Et))
		{
			std::cout << Et.cod << std::endl;
		}
		else
		{
			std::cout << "Erro linha: " << linha << " coluna: " << coluna << " token: " << lex << std::endl;
			break;
		}
	}

	return 0;
}