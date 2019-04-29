#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <numeric>

#define BLOCOGLOBAL 0
#define INIPARAM 12

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
	TKConstInt,
	TKConstFloat,
	TKConstChar,
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
	TKResto,
	TKReturn
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
	ESTDiferente,
	ESTFloat,
	ESTChar,
	ESTFimChar
};

enum ETipo
{
	TPInt,
	TPFloat,
	TPChar,
	TPFunc
};

enum ETipoDec
{
	TDVar,
	TDFunc
};

struct Var {
	std::string id;
	std::string cod;
	ETipo tipo;
	std::string place;
	ETipoDec tipo_dec;
	int bloco;
	int endereco;
	int tamanho;
	std::vector<Var> args;
	bool isArg;

	Var(const std::string & i, const std::string & c, const ETipo & t, const std::string & p) : place(p), cod(c), tipo(t), id(i), tipo_dec(TDVar), isArg(false) {}

	Var(const std::string & i, const std::string & p, const ETipo & t, const ETipoDec & td, const int b, const int e, const int tam) : id(i), place(p), tipo(t), tipo_dec(td), bloco(b), endereco(e), tamanho(tam), isArg(false) {}

	Var() {}

	std::string getEnderecoPilha()
	{
		if (endereco == 0)
		{
			return "[_BP]";
		}
		else if (isArg)
		{
			return "[_BP + " + std::to_string(endereco) + "]";
		}
		else
		{
			return "[_BP - " + std::to_string(endereco) + "]";
		}
	}

};

struct Control {
	std::string cont;
	std::string fim;

	Control(const std::string c, const std::string f) : cont(c), fim(f) {}

	Control() {}
};

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
	{ "return", TKReturn },
	{ "fim", TKId }
};

std::map<ETipo, int> tam_tipo
{
	{ TPInt, 4 },
	{ TPFloat, 4 },
	{ TPChar, 1 }
};

std::ifstream arq("prog.c");
std::string lex{ "" };
EToken token;
int linha{ 0 }, coluna{ 0 }, bloco_atual{ 0 }, deslocamento{ 0 };
std::vector<Var> tabela_simbolos;

/***********************************************************************************/
/*                                                                                 */
/*  INÍCIO DO LÉXICO - Não entre a não ser que tenha interesse pessoal em léxicos  */
/*                                                                                 */
/***********************************************************************************/

char le_char()
{
	char c = arq.get();
	if (c == EOF) return '\0';
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

			if (lex[0] == '\'')
			{
				estado = ESTChar;
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
			else if (c == '.')
			{
				lex.push_back(c);
				c = le_char();
				estado = ESTFloat;
				break;
			}
			else
			{
				//return TKConst;
				return TKConstInt;
			}

		case ESTFloat:
			if (c >= '0' && c <= '9')
			{
				lex.push_back(c);
				c = le_char();
				break;
			}
			else
			{
				//return TKConst;
				return TKConstFloat;
			}

		case ESTChar:
			if (c < 256)
			{
				lex.push_back(c);
				c = le_char();
				estado = ESTFimChar;
				break;
			}
			else
			{
				return TKErro;
			}

		case ESTFimChar:
			if (c == '\'')
			{
				lex.push_back(c);
				c = le_char();
				return TKConstChar;
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

/***********************************************************************************/
/*																				   */
/*  FIM DO LÉXICO																   */
/*                                                                                 */
/***********************************************************************************/

int CMD(const Control &, std::string &);
int ENVIAPARAM1(std::vector<Var> &);
int COMP0(Var &);
int COMP2(Var &);
int COMP4(Var &);
int COMP6(Var &);
int EXP1(Var &);
int EXP3(Var &);
int PARAM(std::vector<Var> &);
int DEC(Var &);
int TIPO(ETipo &);
int LISTA_COM(const Control &, std::string &);
int VAR(const Var &, Var &);
int LISTA_CHAVES(const Control &, std::string &);
int COMANDO(const Control &, std::string &);
int ATRIB(const Var &, Var &);
int P(Var &);

std::string geralabel()
{
	static int cont{ 0 };
	std::string res{ "L" + std::to_string(cont) };
	++cont;
	return res;
}

std::string geratemp()
{
	static int cont{ 0 };
	std::string res{ "T" + std::to_string(cont) };
	++cont;
	return res;
}

std::string print_tipo(const ETipo & tipo)
{
	if (tipo == TPInt)
		return "int";
	else if (tipo == TPFloat)
		return "float";
	else if (tipo == TPChar)
		return "char";
	else
		return "sem tipo";
}

void printar_tipo(const std::string & s, const Var & v)
{
	std::cout << "var " << s << " " << print_tipo(v.tipo) << "\n";
}

int convertertipo(Var & tde, const ETipo & tpara)
{
	if (tde.tipo == tpara)
	{
		return 1;
	}
	else
	{
		const std::string place{ geratemp() };
		if (tde.tipo == TPInt && tpara == TPFloat)
		{
			tde.tipo = tpara;
			tde.cod += "\t" + place + " = inttofloat(" + tde.place + ")\n";
			tde.place = place;
			return 1;
		}
		else if (tde.tipo == TPFloat && tpara == TPInt)
		{
			tde.tipo = tpara;
			tde.cod += "\t" + place + " = floattoint(" + tde.place + ")\n";
			tde.place = place;
			return 1;
		}
		else if (tde.tipo == TPChar && tpara == TPInt)
		{
			tde.tipo = tpara;
			tde.cod += "\t" + place + " = chartoint(" + tde.place + ")\n";
			tde.place = place;
			return 1;
		}
		else if (tde.tipo == TPInt && tpara == TPChar)
		{
			tde.tipo = tpara;
			tde.cod += "\t" + place + " = inttochar(" + tde.place + ")\n";
			tde.place = place;
			return 1;
		}
		else if (tde.tipo == TPChar && tpara == TPFloat)
		{
			tde.tipo = tpara;
			tde.cod += "\t" + place + " = chartoint(" + tde.place + ")\n";
			tde.place = geratemp();
			tde.cod += "\t" + tde.place + " = inttofloat(" + place + ")\n";
			return 1;
		}
		else if (tde.tipo == TPFloat && tpara == TPChar)
		{
			tde.tipo = tpara;
			tde.cod += "\t" + place + " = floattoint(" + tde.place + ")\n";
			tde.place = geratemp();
			tde.cod += "\t" + tde.place + " = inttochar(" + place + ")\n";
			return 1;
		}
	}
	std::cout << "Erro ao converter valor!\tLinha: " << linha << " Coluna: " << coluna << "\n";
	return 0;
}

int verificatipo(const Var & t1, const Var & t2, Var & tres)
{
	if (t1.tipo == t2.tipo)
	{
		tres.tipo = t1.tipo;
		return 1;
	}
	else if ((t1.tipo == TPInt && t2.tipo == TPFloat) || (t1.tipo == TPChar && t2.tipo == TPInt) || (t1.tipo == TPChar && t2.tipo == TPFloat))
	{
		tres.tipo = t1.tipo;
		tres.place = t1.place;
		convertertipo(tres, t2.tipo);
		return 1;
	}
	else if ((t1.tipo == TPFloat && t2.tipo == TPInt) || (t1.tipo == TPInt && t2.tipo == TPChar) || (t1.tipo == TPFloat && t2.tipo == TPChar))
	{
		tres.tipo = t2.tipo;
		tres.place = t2.place;
		convertertipo(tres, t1.tipo);
		return 1;
	}
	return 0;
}

std::string gen(const std::string & op, const std::string & tmp, const std::string & place1, const std::string & place2)
{
	return std::string{ "\t" + tmp + " = " + place1 + " " + op + " " + place2 + "\n" };
}

void setEstado(bool set)
{
	static int static_tmp_col, static_tmp_linha;
	static std::streamoff static_pos_arq;
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

std::vector<Var>::iterator buscaTabelaSimbolos(const std::string & lex)
{
	int bloco = bloco_atual;
	std::vector<Var> ::iterator it = tabela_simbolos.end();
	while (bloco >= BLOCOGLOBAL && it == tabela_simbolos.end())
	{
		it = std::find_if(tabela_simbolos.begin(), tabela_simbolos.end(), [lex, bloco](Var s) { return s.id == lex && s.bloco == bloco; });
		--bloco;
	}

	return it;
}

int insereTabelaSimbolos(const Var & simb)
{
	auto it = std::find_if(tabela_simbolos.begin(), tabela_simbolos.end(), [simb](Var s) { return s.id == simb.id && s.bloco == simb.bloco; });

	if (it == tabela_simbolos.end())
	{
		tabela_simbolos.push_back(simb);
		return 1;
	}
	return 0;
}

std::vector<Var>::iterator getFuncaoAtual()
{
	auto it = std::find_if(tabela_simbolos.rbegin(), tabela_simbolos.rend(), [](Var v) { return v.tipo_dec == TDFunc; });
	return (it + 1).base();
}

void atualizaTamanhoFuncao()
{
	auto it = std::find_if(tabela_simbolos.rbegin(), tabela_simbolos.rend(), [](Var v) { return v.tipo_dec == TDFunc; });
	if (it != tabela_simbolos.rend())
	{
		it->tamanho = std::max(it->tamanho, deslocamento);
	}
}

void deletaBlocoTabelaSimbolos(int & bloco, int & deslocamento)
{
	atualizaTamanhoFuncao();
	auto func = [bloco](int sum, Var s) { return s.bloco == bloco ? s.tamanho : 0; };
	deslocamento -= std::accumulate(tabela_simbolos.begin(), tabela_simbolos.end(), 0, func);
	tabela_simbolos.erase(
		std::remove_if(tabela_simbolos.begin(), tabela_simbolos.end(), [bloco](Var s) { return s.bloco == bloco; }),
		tabela_simbolos.end()
	);
	--bloco;
}

int isGlobal(const int bloco)
{
	if (bloco == BLOCOGLOBAL)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

std::string getPlace(const std::string & id, const int & end)
{
	std::string place{ id };
	if (!isGlobal(bloco_atual))
	{
		place = geratemp();
	}
	return place;
}

int addVar(const ETipo & tipo)
{
	int res = deslocamento;
	if (!isGlobal(bloco_atual))
	{
		deslocamento += tam_tipo.at(tipo);
	}
	return res;
}

int ENVIAPARAMS(std::vector<Var> & ENVIAPARAMSs)
{
	if (token == TKVirgula)
	{
		le_token();
		if (ENVIAPARAM1(ENVIAPARAMSs))
		{
			return 1;
		}
	}
	else
	{
		return 1;
	}
	return 0;
}

int ENVIAPARAM1(std::vector<Var> & ENVIAPARAM1s)
{
	Var COMP0s;
	if (COMP0(COMP0s))
	{
		ENVIAPARAM1s.push_back(COMP0s);
		if (ENVIAPARAMS(ENVIAPARAM1s))
		{
			return 1;
		}
	}
	return 0;
}

int ENVIAPARAM(std::vector<Var> & ENVIAPARAMs)
{
	if (ENVIAPARAM1(ENVIAPARAMs))
	{
		return 1;
	}
	else
	{
		return 1;
	}
	return 0;
}

int ATRIB(const Var & ATRIBh, Var & ATRIBs)
{
	setEstado(true);
	if (token == TKAtrib)
	{
		le_token();
		auto id = buscaTabelaSimbolos(ATRIBh.id);
		if (id->tipo_dec == TDVar)
		{
			Var COMP0s;
			if (COMP0(COMP0s))
			{

				ATRIBs = *id;

				if (convertertipo(COMP0s, ATRIBs.tipo))
				{
					ATRIBs.cod = COMP0s.cod + ATRIBh.cod + "\t" + ATRIBs.place + " = " + COMP0s.place + "\n";
					if (!isGlobal(ATRIBs.bloco))
					{
						ATRIBs.cod += "\t" + ATRIBs.getEnderecoPilha() + " = " + ATRIBs.place + "\n";
					}
					return 1;
				}
			}
		}
		else
		{
			std::cout << "Identificador e uma funcao\n";
		}

	}
	setEstado(false);
	return 0;
}

int PROXID(const Var & PROXIDh, Var & PROXIDs)
{
	Var ATRIBh{ PROXIDh }, ATRIBs;
	if (ATRIB(ATRIBh, ATRIBs))
	{
		PROXIDs = ATRIBs;
		return 1;
	}
	else if (token == TKAbreParenteses)
	{
		le_token();
		std::vector<Var> ENVIAPARAMs;
		if (ENVIAPARAM(ENVIAPARAMs))
		{
			if (token == TKFechaParenteses)
			{
				auto id = buscaTabelaSimbolos(PROXIDh.place);
				if (ENVIAPARAMs.size() == id->args.size())
				{
					le_token();
					PROXIDs = *id;
					PROXIDs.place = geratemp();
					PROXIDs.cod = "\t_SP = _SP - " + std::to_string(tam_tipo.at(id->tipo)) + "\n";
					int tam_params{ 0 };
					for (Var v : ENVIAPARAMs)
					{
						PROXIDs.cod += v.cod + "\tpush " + v.place + "\n";
						tam_params += tam_tipo.at(PROXIDs.tipo);
					}
					PROXIDs.cod += "\tcall " + id->id + "\n";
					PROXIDs.cod += "\t_SP = _SP + " + std::to_string(tam_params) + "\n\tpop " + PROXIDs.place + "\n";
					return 1;
				}
				else if (ENVIAPARAMs.size() > id->args.size())
				{
					std::cout << "Muitos parametros na chamada da funcao " << id->id << "\n";
				}
				else
				{
					std::cout << "Poucos parametros na chamada da funcao " << id->id << "\n";
				}
			}
		}
	}
	else
	{
		PROXIDs = PROXIDh;
		if (!isGlobal(PROXIDs.bloco))
		{
			PROXIDs.cod = "\t" + PROXIDs.place + " = " + PROXIDs.getEnderecoPilha() + "\n";
		}
		return 1;
	}
	return 0;
}

int EXPFIM(Var & EXPFIMs)
{
	Var Ps;
	if (P(Ps))
	{
		EXPFIMs = Ps;
		return 1;
	}
	else if (token == TKId)
	{
		auto id = buscaTabelaSimbolos(lex);
		if (id != tabela_simbolos.end())
		{
			le_token();
			Var PROXIDh{ *id }, PROXIDs;

			if (PROXID(PROXIDh, PROXIDs))
			{
				EXPFIMs = PROXIDs;
				return 1;
			}
		}
		else
		{
			std::cout << "Identificador nao declarado!\n";
		}
	}
	else if (token == TKConstInt || token == TKConstFloat || token == TKConstChar)
	{
		EXPFIMs.place = geratemp();
		EXPFIMs.cod = "\t" + EXPFIMs.place + " = " + lex + "\n";
		if (token == TKConstChar)
		{
			EXPFIMs.tipo = TPChar;
		}
		else
		{
			EXPFIMs.tipo = token == TKConstFloat ? TPFloat : TPInt;
		}
		le_token();
		return 1;
	}
	return 0;
}

int EXP5(Var & EXP5s)
{
	Var EXPFIMs;
	if (token == TKSubtracao)
	{
		le_token();
		Var EXP5_1s;
		if (EXP5(EXP5_1s))
		{
			EXP5s.place = geratemp();
			EXP5s.cod = EXP5s.cod + EXP5_1s.cod + "\t" + EXP5s.place + " = - " + EXP5_1s.place + "\n";
			return 1;
		}
	}
	else if (token == TKSoma)
	{
		le_token();
		Var EXP5_2s;
		if (EXP5(EXP5_2s))
		{
			EXP5s.place = geratemp();
			EXP5s.cod = EXP5s.cod + EXP5_2s.cod + "\t" + EXP5s.place + " = + " + EXP5_2s.place + "\n";
			return 1;
		}
	}
	else if (token == TKNot)
	{
		le_token();
		Var EXP5_3s;
		if (EXP5(EXP5_3s))
		{
			EXP5s.place = geratemp();
			EXP5s.cod = EXP5s.cod + EXP5_3s.cod + "\t" + EXP5s.place + " = not " + EXP5_3s.place + "\n";
			return 1;
		}
	}
	else if (EXPFIM(EXPFIMs))
	{
		EXP5s = EXPFIMs;
		return 1;
	}
	return 0;
}

int EXP4(const Var & EXP4h, Var & EXP4s)
{
	if (token == TKMult)
	{
		le_token();
		Var EXP5s;
		if (EXP5(EXP5s))
		{
			Var EXP4_1h, EXP4_1s;
			if (verificatipo(EXP4h, EXP5s, EXP4_1h))
			{
				EXP4_1h.place = geratemp();
				EXP4_1h.cod = EXP4h.cod + EXP5s.cod + gen("*", EXP4_1h.place, EXP4h.place, EXP5s.place);

				if (EXP4(EXP4_1h, EXP4_1s))
				{
					EXP4s = EXP4_1s;
					return 1;
				}
			}
		}
	}
	else if (token == TKDiv)
	{
		le_token();
		Var EXP5s;
		if (EXP5(EXP5s))
		{
			Var EXP4_1h, EXP4_1s;
			if (verificatipo(EXP4h, EXP5s, EXP4_1h))
			{
				EXP4_1h.place = geratemp();
				EXP4_1h.cod = EXP4h.cod + EXP5s.cod + gen("/", EXP4_1h.place, EXP4h.place, EXP5s.place);

				if (EXP4(EXP4_1h, EXP4_1s))
				{
					EXP4s = EXP4_1s;
					return 1;
				}
			}
		}
	}
	else if (token == TKResto)
	{
		le_token();
		Var EXP5s;
		if (EXP5(EXP5s))
		{
			Var EXP4_1h, EXP4_1s;
			if (EXP4h.tipo == TPInt && EXP5s.tipo == TPInt)
			{
				EXP4_1h.place = geratemp();
				EXP4_1h.tipo = TPInt;
				EXP4_1h.cod = EXP4h.cod + EXP5s.cod + gen("%", EXP4_1h.place, EXP4h.place, EXP5s.place);

				if (EXP4(EXP4_1h, EXP4_1s))
				{
					EXP4s = EXP4_1s;
					return 1;
				}
			}
			else
			{
				std::cout << "Operador % exige números inteiros!\n";
			}
		}
	}
	else
	{
		EXP4s = EXP4h;
		return 1;
	}
	return 0;
}

int EXP3(Var & EXP3s)
{
	Var EXP5s;
	if (EXP5(EXP5s))
	{
		Var EXP4h{ EXP5s };
		Var EXP4s;
		if (EXP4(EXP4h, EXP4s))
		{
			EXP3s = EXP4s;
			return 1;
		}
	}
	return 0;
}

int EXP2(const Var & EXP2h, Var & EXP2s)
{
	if (token == TKSoma)
	{
		le_token();
		Var EXP3s;
		if (EXP3(EXP3s))
		{
			Var EXP2_1h, EXP2_1s;
			if (verificatipo(EXP2h, EXP3s, EXP2_1h))
			{
				EXP2_1h.place = geratemp();
				EXP2_1h.cod = EXP2h.cod + EXP3s.cod + EXP2_1h.cod + gen("+", EXP2_1h.place, EXP2h.place, EXP3s.place);

				if (EXP2(EXP2_1h, EXP2_1s))
				{
					EXP2s = EXP2_1s;
					return 1;
				}
			}
		}
	}
	else if (token == TKSubtracao)
	{
		le_token();
		Var EXP3s;
		if (EXP3(EXP3s))
		{
			Var EXP2_1h, EXP2_1s;
			if (verificatipo(EXP2h, EXP3s, EXP2_1h))
			{
				EXP2_1h.place = geratemp();
				EXP2_1h.cod = EXP2h.cod + EXP3s.cod + EXP2_1h.cod + gen("-", EXP2_1h.place, EXP2h.place, EXP3s.place);
				if (EXP2(EXP2_1h, EXP2_1s))
				{
					EXP2s = EXP2_1s;
					return 1;
				}
			}
		}
	}
	else
	{
		EXP2s = EXP2h;
		return 1;
	}
	return 0;
}

int EXP1(Var & EXP1s)
{
	Var EXP3s;
	if (EXP3(EXP3s))
	{
		Var EXP2h{ EXP3s };
		Var EXP2s;
		if (EXP2(EXP2h, EXP2s))
		{
			EXP1s = EXP2s;
			return 1;
		}
	}
	return 0;
}

int COMPOPR(std::string & op)
{
	if (token == TKMaiorIgual)
	{
		op = lex;
		le_token();
		return 1;
	}
	else if (token == TKMaior)
	{
		op = lex;
		le_token();
		return 1;
	}
	else if (token == TKMenor)
	{
		op = lex;
		le_token();
		return 1;
	}
	else if (token == TKMenorIgual)
	{
		op = lex;
		le_token();
		return 1;
	}
	return 0;
}

int COMP7(const Var & COMP7h, Var & COMP7s)
{
	std::string ops;
	if (COMPOPR(ops))
	{
		Var EXP1s;
		if (EXP1(EXP1s))
		{
			Var COMP7_1h, COMP7_1s;
			COMP7_1h.place = geratemp();
			COMP7_1h.tipo = TPInt;
			std::string rtrue = geralabel();
			std::string rfalse = geralabel();

			COMP7_1h.cod = COMP7h.cod + EXP1s.cod + "\tif " + COMP7h.place + " " + ops + " " + EXP1s.place + " goto " + rtrue + "\n\t" + COMP7_1h.place + " = 0\n" + "\tgoto " + rfalse + "\n" + rtrue + ":\n\t" + COMP7_1h.place + " = 1\n" + rfalse + ":\n";

			if (COMP7(COMP7_1h, COMP7_1s))
			{
				COMP7s = COMP7_1s;
				return 1;
			}
		}
	}
	else
	{
		COMP7s = COMP7h;
		return 1;
	}
	return 0;
}

int COMP6(Var & COMP6s)
{
	Var EXP1s;
	if (EXP1(EXP1s))
	{
		Var COMP7h{ EXP1s };
		Var COMP7s;
		if (COMP7(COMP7h, COMP7s))
		{
			COMP6s = COMP7s;
			return 1;
		}
	}
	return 0;
}

int COMPIGDIFF(std::string & op)
{
	if (token == TKIgual)
	{
		op = lex;
		le_token();
		return 1;
	}
	else if (token == TKDiferente)
	{
		op = lex;
		le_token();
		return 1;
	}
	return 0;
}

int COMP5(const Var & COMP5h, Var & COMP5s)
{
	std::string ops;
	if (COMPIGDIFF(ops))
	{
		Var COMP6s;
		if (COMP6(COMP6s))
		{
			Var COMP5_1h, COMP5_1s;
			COMP5_1h.place = geratemp();
			COMP5_1h.tipo = TPInt;
			std::string rtrue{ geralabel() };
			std::string rfalse{ geralabel() };

			COMP5_1h.cod = COMP5h.cod + COMP6s.cod + "\tif " + COMP5h.place + " " + ops + " " + COMP6s.place + " goto " + rtrue + "\n\t" + COMP5_1h.place + " = 0\n" + "\tgoto " + rfalse + "\n" + rtrue + ":\n\t" + COMP5_1h.place + " = 1\n" + rfalse + ":\n";

			if (COMP5(COMP5_1h, COMP5_1s))
			{
				COMP5s = COMP5_1s;
				return 1;
			}
		}
	}
	else
	{
		COMP5s = COMP5h;
		return 1;
	}
	return 0;
}

int COMP4(Var & COMP4s)
{
	Var COMP6s;
	if (COMP6(COMP6s))
	{
		Var COMP5h{ COMP6s };
		Var COMP5s;
		if (COMP5(COMP5h, COMP5s))
		{
			COMP4s = COMP5s;
			return 1;
		}
	}
	return 0;
}

int COMP3(const Var & COMP3h, Var & COMP3s)
{
	if (token == TKE)
	{
		le_token();
		Var COMP4s;
		if (COMP4(COMP4s))
		{
			Var COMP3_1h, COMP3_1s;
			COMP3_1h.place = geratemp();
			COMP3_1h.tipo = TPInt;
			COMP3_1h.cod = COMP3h.cod + COMP4s.cod + gen("and", COMP3_1h.place, COMP3h.place, COMP4s.place);
			if (COMP3(COMP3_1h, COMP3_1s))
			{
				COMP3s = COMP3_1s;
				return 1;
			}
		}
	}
	else
	{
		COMP3s = COMP3h;
		return 1;
	}
	return 0;
}

int COMP2(Var & COMP2s)
{
	Var COMP4s;
	if (COMP4(COMP4s))
	{
		Var COMP3h{ COMP4s };
		Var COMP3s;
		if (COMP3(COMP3h, COMP3s))
		{
			COMP2s = COMP3s;
			return 1;
		}
	}
	return 0;
}

int COMP1(const Var & COMP1h, Var & COMP1s)
{
	if (token == TKOu)
	{
		le_token();
		Var COMP2s;
		if (COMP2(COMP2s))
		{
			Var COMP1_1h, COMP1_1s;
			COMP1_1h.place = geratemp();
			COMP1_1h.tipo = TPInt;
			COMP1_1h.cod = COMP1h.cod + COMP2s.cod + gen("or", COMP1_1h.place, COMP1h.place, COMP2s.place);
			if (COMP1(COMP1_1h, COMP1_1s))
			{
				COMP1s = COMP1_1s;
				return 1;
			}
		}
	}
	else
	{
		COMP1s = COMP1h;
		return 1;
	}
	return 0;
}

int COMP0(Var & COMP0s)
{
	Var COMP2s;
	if (COMP2(COMP2s))
	{
		Var COMP1h{ COMP2s };
		Var COMP1s;
		if (COMP1(COMP1h, COMP1s))
		{
			COMP0s = COMP1s;
			return 1;
		}
	}
	return 0;
}

int PARAMS(std::vector<Var> & PARAMSs)
{
	if (token == TKVirgula)
	{
		le_token();
		if (PARAM(PARAMSs))
		{
			return 1;
		}
	}
	else
	{
		return 1;
	}
	return 0;
}

int PARAM1(std::vector<Var> & PARAM1s)
{
	ETipo tipo;
	if (TIPO(tipo))
	{
		if (token == TKId)
		{
			auto it = std::find_if(PARAM1s.begin(), PARAM1s.end(), [](Var s) { return s.place == lex; });
			if (it == PARAM1s.end())
			{
				Var v{ lex, geratemp(), tipo, TDVar, bloco_atual + 1, 0, tam_tipo.at(tipo) };
				v.isArg = true;
				int pos = PARAM1s.size();
				PARAM1s.push_back(v);
				le_token();
				if (PARAMS(PARAM1s))
				{
					auto it = std::prev(PARAM1s.rend(), pos);
					PARAM1s[pos].endereco = std::accumulate(PARAM1s.rbegin(), it, INIPARAM, [](int sum, Var v) { return sum + v.tamanho; });
					PARAM1s[pos].endereco -= PARAM1s[pos].tamanho;
					insereTabelaSimbolos(PARAM1s[pos]);
					return 1;
				}
			}
			else
			{
				std::cout << "Parametro já declarado!\n";
			}
		}
	}
	return 0;
}

int PARAM(std::vector<Var> & PARAMs)
{
	if (PARAM1(PARAMs))
	{
		return 1;
	}
	else
	{
		return 1;
	}
}

int X(const Var & Xh, Var & Xs)
{
	if (token == TKVirgula)
	{
		le_token();
		Var VARh{ Xh }, VARs;
		if (VAR(VARh, VARs))
		{
			Xs = VARs;
			return 1;
		}
	}
	else if (token == TKPontoVirgula)
	{
		le_token();
		Xs = Xh;
		return 1;
	}
	return 0;
}

int ATRIBDEC(const Var & ATRIBDECh, Var & ATRIBDECs)
{
	Var ATRIBh{ ATRIBDECh }, Xh{ ATRIBDECh }, ATRIBs, Xs;
	if (ATRIB(ATRIBh, ATRIBs))
	{
		Xh = ATRIBs;
		if (X(Xh, Xs))
		{
			ATRIBDECs = Xs;
			return 1;
		}
	}
	else if (X(Xh, Xs))
	{
		ATRIBDECs = Xs;
		return 1;
	}
	return 0;
}

int VAR(const Var & VARh, Var & VARs)
{
	if (token == TKId)
	{
		int end = addVar(VARh.tipo);
		Var id{ lex, getPlace(lex, end), VARh.tipo, TDVar, bloco_atual, end, tam_tipo.at(VARh.tipo) };
		if (insereTabelaSimbolos(id))
		{
			le_token();
			Var ATRIBDECh{ id.id, VARh.cod, id.tipo, id.place }, ATRIBDECs;
			if (ATRIBDEC(ATRIBDECh, ATRIBDECs))
			{
				VARs = ATRIBDECs;
				return 1;
			}
		}
		else
		{
			std::cout << "Identificador ja declarado!\n";
		}
	}
	return 0;
}

int TIPO(ETipo & tipo)
{
	if (token == TKInt)
	{
		le_token();
		tipo = TPInt;
		return 1;
	}
	else if (token == TKChar)
	{
		le_token();
		tipo = TPChar;
		return 1;
	}
	else if (token == TKDouble)
	{
		le_token();
		tipo = TPFloat;
		return 1;
	}
	else if (token == TKFloat)
	{
		le_token();
		tipo = TPFloat;
		return 1;
	}
	return 0;
}

int VARFUNC(const Var & VARFUNCh, Var & VARFUNCs)
{
	Var ATRIBDECh{ VARFUNCh };
	Var ATRIBDECs;
	if (ATRIBDEC(ATRIBDECh, ATRIBDECs))
	{
		VARFUNCs = ATRIBDECs;
		return 1;
	}
	else if (token == TKAbreParenteses)
	{
		le_token();
		int pos = tabela_simbolos.size() - 1;
		tabela_simbolos[pos].tipo_dec = TDFunc;
		tabela_simbolos[pos].tamanho = 0;
		if (isGlobal(tabela_simbolos[pos].bloco))
		{
			std::vector<Var> PARAMs;
			if (PARAM(PARAMs))
			{
				if (token == TKFechaParenteses)
				{
					le_token();
					tabela_simbolos[pos].args = PARAMs;
					const Control LISTA_CHAVESh;
					std::string LISTA_CHAVESs;
					if (LISTA_CHAVES(LISTA_CHAVESh, LISTA_CHAVESs))
					{
						VARFUNCs.place = VARFUNCh.place;
						VARFUNCs.id = VARFUNCh.id;
						VARFUNCs.tipo = VARFUNCh.tipo;
						VARFUNCs.cod = "\n" + VARFUNCs.place + ":\n\tpush _BP\n\t_BP = _SP\n";
						std::string str_tam = std::to_string(tabela_simbolos[pos].tamanho);
						VARFUNCs.cod += "\t_SP = _SP - " + str_tam + "\n";
						VARFUNCs.cod += LISTA_CHAVESs + "fim_" + VARFUNCs.id + ":\n\t_SP = _SP + " + str_tam + "\n\tpop _BP\nreturn\n\n";
						return 1;
					}
				}
			}
		}
		else
		{
			std::cout << "Nao é possivel declarar uma funcao dentro de outra!\n";
		}
	}
	return 0;
}

int DEC(Var & DECs)
{
	ETipo tipo;
	if (TIPO(tipo))
	{
		if (token == TKId)
		{
			int end = addVar(tipo);
			Var id{ lex, getPlace(lex, end), tipo, TDVar, bloco_atual, end, tam_tipo.at(tipo) };
			if (insereTabelaSimbolos(id))
			{
				le_token();
				Var VARFUNCh{ id };
				Var VARFUNCs;
				if (VARFUNC(VARFUNCh, VARFUNCs))
				{
					DECs = VARFUNCs;
					return 1;
				}
			}
			else
			{
				std::cout << "Identificador ja declarado\n";
			}
		}
	}
	return 0;
}

int P(Var & Ps)
{
	if (token == TKAbreParenteses)
	{
		le_token();
		Var COMP0s;
		if (COMP0(COMP0s))
		{
			if (token == TKFechaParenteses)
			{
				le_token();
				Ps = COMP0s;
				return 1;
			}
		}
	}
	return 0;
}

int ELSE(const Control & ELSEh, Control & ELSEs, std::string & ELSEscod)
{
	if (token == TKElse)
	{
		le_token();
		Control COMANDOh{ ELSEh };
		std::string COMANDOscod;
		if (COMANDO(COMANDOh, COMANDOscod))
		{
			ELSEs.cont = geralabel();
			ELSEs.fim = geralabel();
			ELSEscod = ELSEs.cont + ":\n" + COMANDOscod;
			return 1;
		}
	}
	else
	{
		ELSEs.cont = geralabel();
		ELSEs.fim = ELSEs.cont;
		return 1;
	}
	return 0;
}

int LISTA_COM_OBR(const Control & LISTA_COM_OBRh, std::string & LISTA_COM_OBRs)
{
	const Control COMANDOh{ LISTA_COM_OBRh };
	std::string COMANDOs{ "" };
	if (COMANDO(COMANDOh, COMANDOs))
	{
		const Control LISTA_COMh{ LISTA_COM_OBRh };
		std::string LISTA_COMs{ "" };
		if (LISTA_COM(LISTA_COMh, LISTA_COMs))
		{
			LISTA_COM_OBRs = COMANDOs + LISTA_COMs;
			return 1;
		}
	}
	return 0;
}

int CASE(const Control & CASEhcontrol, const Var & CASEhvar, std::string & CASEscod)
{
	if (token == TKCase)
	{
		le_token();
		Var EXPFIMs;
		if (EXPFIM(EXPFIMs))
		{
			if (token == TKDoisPontos)
			{
				le_token();
				const Control LISTA_COMh{ CASEhcontrol };
				std::string LISTA_COMscod;
				if (LISTA_COM(LISTA_COMh, LISTA_COMscod))
				{
					const Control CASE_1hcontrol{ CASEhcontrol };
					const Var CASE_1hvar{ CASEhvar };
					std::string CASE_1scod;
					if (CASE(CASE_1hcontrol, CASE_1hvar, CASE_1scod))
					{
						if ((EXPFIMs.tipo == TPInt || EXPFIMs.tipo == TPChar) && verificatipo(CASEhvar, EXPFIMs, EXPFIMs))
						{
							std::string label = geralabel();
							CASEscod = EXPFIMs.cod + "\tif " + CASEhvar.place + " != " + EXPFIMs.place + " goto " + label + "\n" + LISTA_COMscod + label + ":\n" + CASE_1scod;
							return 1;
						}
					}
				}
			}
		}
	}
	else if (token == TKDefault)
	{
		le_token();
		if (token == TKDoisPontos)
		{
			le_token();
			const Control LISTA_COM_OBRh{ CASEhcontrol };
			std::string LISTA_COM_OBRscod;
			if (LISTA_COM_OBR(LISTA_COM_OBRh, LISTA_COM_OBRscod))
			{
				CASEscod = LISTA_COM_OBRscod + "\tgoto " + CASEhcontrol.fim + "\n";
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

int PV()
{
	if (token == TKPontoVirgula)
	{
		le_token();
		return 1;
	}
	else
	{
		return 1;
	}
}

int SPV(const Control & SPVh, std::string & SPVscod)
{
	Var DECs;
	if (DEC(DECs))
	{
		SPVscod = DECs.cod;
		return 1;
	}
	else if (token == TKWhile)
	{
		le_token();
		Var Ps;
		if (P(Ps))
		{
			const Control COMANDOh{ geralabel(), geralabel() };
			std::string COMANDOscod;
			if (COMANDO(COMANDOh, COMANDOscod))
			{
				SPVscod = COMANDOh.cont + ":\n" + Ps.cod + "\tif " + Ps.place + " == 0 goto " + COMANDOh.fim + "\n" + COMANDOscod + "\tgoto " + COMANDOh.cont + "\n" + COMANDOh.fim + ":\n";
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
			Var COMP0s;
			if (COMP0(COMP0s))
			{
				if (token == TKPontoVirgula)
				{
					le_token();
					Var COMP0s_2;
					if (COMP0(COMP0s_2))
					{
						if (token == TKPontoVirgula)
						{
							le_token();
							Var COMP0s_3;
							if (COMP0(COMP0s_3))
							{
								if (token == TKFechaParenteses)
								{
									le_token();
									std::string inicio{ geralabel() };
									const Control COMANDOh{ geralabel(), geralabel() };
									std::string COMANDOscod;
									if (COMANDO(COMANDOh, COMANDOscod))
									{
										SPVscod = COMP0s.cod + inicio + ":\n" + COMP0s_2.cod + "\tif " + COMP0s_2.place + " == 0 goto " + COMANDOh.fim + "\n" + COMANDOscod + COMANDOh.cont + ":\n" + COMP0s_3.cod + "\tgoto " + inicio + "\n" + COMANDOh.fim + ":\n";
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
	else if (token == TKIf)
	{
		le_token();
		Var Ps;
		if (P(Ps))
		{
			const Control COMANDOh{ SPVh };
			std::string COMANDOscod;
			if (COMANDO(COMANDOh, COMANDOscod))
			{
				Control ELSEs;
				Control ELSEh{ SPVh };
				std::string ELSEscod;
				if (ELSE(ELSEh, ELSEs, ELSEscod))
				{
					SPVscod = Ps.cod + "\tif " + Ps.place + " == 0 goto " + ELSEs.cont + "\n" + COMANDOscod + "\tgoto " + ELSEs.fim + "\n" + ELSEscod + ELSEs.fim + ":\n";
					return 1;
				}
			}
		}
	}
	else if (token == TKSwitch)
	{
		le_token();
		if (token == TKAbreParenteses)
		{
			le_token();
			if (token == TKId)
			{
				auto id = buscaTabelaSimbolos(lex);
				if (id != tabela_simbolos.end() && id->tipo_dec == TDVar)
				{
					le_token();
					if (token == TKFechaParenteses)
					{
						le_token();
						if (token == TKAbreChaves)
						{
							le_token();
							++bloco_atual;
							const Control CASEhcontrol{ SPVh.cont, geralabel() };
							Var CASEhvar{ id->id, "", id->tipo, id->place };
							std::string CASEscod;
							if (CASEhvar.tipo == TPInt || CASEhvar.tipo == TPChar)
							{
								if (CASE(CASEhcontrol, CASEhvar, CASEscod))
								{
									if (token == TKFechaChaves)
									{
										le_token();
										deletaBlocoTabelaSimbolos(bloco_atual, deslocamento);
										SPVscod = CASEscod + CASEhcontrol.fim + ":\n";
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
	return 0;
}

int COMANDO(const Control & COMANDOh, std::string & COMANDOs)
{
	const Control
		CMDh{ COMANDOh },
		SPVh{ COMANDOh },
		LISTA_CHAVESh{ COMANDOh };

	std::string
		CMDs{ "" },
		LISTA_CHAVESs{ "" },
		SPVs{ "" };

	setEstado(true);
	if (CMD(CMDh, CMDs))
	{
		if (token == TKPontoVirgula)
		{
			COMANDOs = CMDs;
			le_token();
			return 1;
		}
	}
	else if (LISTA_CHAVES(LISTA_CHAVESh, LISTA_CHAVESs))
	{
		COMANDOs = LISTA_CHAVESs;
		return 1;
	}
	else if (SPV(SPVh, SPVs))
	{
		if (PV())
		{
			COMANDOs = SPVs;
			return 1;
		}
	}
	else if (token == TKPontoVirgula)
	{
		le_token();
		return 1;
	}

	setEstado(false);
	return 0;
}

int LISTA_COM(const Control & LISTA_COMh, std::string & LISTA_COMs)
{
	if (LISTA_COM_OBR(LISTA_COMh, LISTA_COMs))
	{
		return 1;
	}
	else
	{
		return 1;
	}
	return 0;
}

int LISTA_CHAVES(const Control & LISTA_CHAVESh, std::string & LISTA_CHAVESs)
{
	if (token == TKAbreChaves)
	{
		le_token();
		++bloco_atual;
		if (LISTA_COM(LISTA_CHAVESh, LISTA_CHAVESs))
		{
			if (token == TKFechaChaves)
			{
				le_token();
				deletaBlocoTabelaSimbolos(bloco_atual, deslocamento);
				return 1;
			}
		}
	}
	return 0;
}

int CMD(const Control & CMDh, std::string & CMDs)
{
	Var COMP0s;
	if (token == TKDo)
	{
		le_token();
		Control COMANDOh{ geralabel(), geralabel() };
		std::string COMANDOs{ "" };
		if (COMANDO(COMANDOh, COMANDOs))
		{
			if (token == TKWhile)
			{
				le_token();
				Var Ps;
				if (P(Ps))
				{
					CMDs = COMANDOh.cont + ":\n" + COMANDOs + Ps.cod + "\tif " + Ps.place + " == 0 goto " + COMANDOh.fim + "\n\tgoto " + COMANDOh.cont + "\n" + COMANDOh.fim + ":\n";
					return 1;
				}
			}
		}
	}
	else if (token == TKBreak)
	{
		le_token();
		CMDs = "\tgoto " + CMDh.fim + "\n";
		return 1;
	}
	else if (token == TKContinue)
	{
		le_token();
		CMDs = "\tgoto " + CMDh.cont + "\n";
		return 1;
	}
	else if (token == TKReturn)
	{
		le_token();
		if (COMP0(COMP0s))
		{
			auto func = getFuncaoAtual();
			if (func != tabela_simbolos.end())
			{
				int tam_args = std::accumulate(func->args.begin(), func->args.end(), 0, [](int sum, Var v) { return sum + v.tamanho; });
				CMDs = COMP0s.cod + "\t[_BP + " + std::to_string(tam_args + INIPARAM) + "] = " + COMP0s.place + "\n";
				CMDs += "\tgoto fim_" + getFuncaoAtual()->id + "\n";
				return 1;
			}
		}
	}
	else if (COMP0(COMP0s))
	{
		CMDs = COMP0s.cod;
		return 1;
	}
	return 0;
}

int main()
{
	le_token();
	const Control control;
	std::string cod;
	std::ofstream result("result.tac");
	while (token != TKFim)
	{
		if (COMANDO(control, cod))
		{
			std::cout << "OK" << std::endl;
			result << cod;
		}
		else
		{
			std::cout << "Erro linha: " << linha << " coluna: " << coluna << " token: " << lex << std::endl;
			system("pause");
			break;
		}
	}
	arq.close();
	result.close();
	return 0;
}
