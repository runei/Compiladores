#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <algorithm>

enum EToken {
	TKFim,
	TKErro,
	TKId,
	TKConst,
	TKValorL,
	TKValorR,
	TKPush,
	TKPop,
	TKSoma,
	TKSubtracao,
	TKMult,
	TKDiv,
	TKAtrib,
	TKRotulo,
	TKDoisPontos,
	TKGoto,
	TKGoFalse,
	TKGoTrue,
	TKMenor,
	TKMaior,
	TKIgual
};

enum EEstado
{
	ESTGeral,
	ESTId,
	ESTConst,
	ESTAtrib
};

struct palavra_reservada {
	std::string palavra;
	EToken token;
};

struct tabela_simbolos {
	std::string lex;
	EToken token;
	int valor;
};

const std::vector<struct palavra_reservada> lista_palavras
{
	{ "valor-l", TKValorL },
	{ "valor-r", TKValorR },
	{ "push", TKPush },
	{ "pop", TKPop },
	{ "rotulo", TKRotulo },
	{ "goto", TKGoto },
	{ "gofalse", TKGoFalse },
	{ "gotrue", TKGoTrue},
	{ "fim", TKId }
};


std::string entrada{ "" };
unsigned int posbuffer{ 0 };
std::string lex{ "" };
EToken token;
std::stack<int> pilha;
std::vector<struct tabela_simbolos> tab_simbolos;

char le_char()
{
	if (posbuffer >= entrada.size())
	{
		return '\0';
	}
	return entrada[posbuffer++];

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

			if (lex[0] == ':')
			{
				estado = ESTAtrib;
				break;
			}

			if (lex[0] == '\n' || lex[0] == '\r')
			{
				break;
			}

			if (lex[0] == '\t' || lex[0] == ' ')
			{
				break;
			}

			if (lex[0] == '+') { return TKSoma; };
			if (lex[0] == '-') { return TKSubtracao; };
			if (lex[0] == '*') { return TKMult; };
			if (lex[0] == '/') { return TKDiv; };
			if (lex[0] == '<') { return TKMenor; };
			if (lex[0] == '>') { return TKMaior; };
			if (lex[0] == '=') { return TKIgual; };
			if (lex[0] == '\0') { return TKFim; };
			return TKErro;

		case ESTId:
			if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '-' || (c >= '0' && c <= '9'))
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
		case ESTAtrib:
			if (c == '=')
			{
				lex.push_back(c);
				c = le_char();
				return TKAtrib;
			}
			else
			{
				return TKDoisPontos;
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

int pop()
{
	if (pilha.empty())
	{
		return 0;
	}

	pilha.pop();

	return 1;
}

int top(int & result)
{
	if (pilha.empty())
	{
		return 0;
	}

	result = pilha.top();

	return 1;
}

int pegaToposPilha(int & topo, int & subtopo)
{
	if (top(topo))
	{
		if (pop())
		{
			if (top(subtopo))
			{
				if (pop())
				{
					return 1;
				}
			}
		}
	}
	return 0;
}

int CMD()
{
	if (token == TKPush)
	{
		le_token();
		if (token == TKConst)
		{
			pilha.push(std::stoi(lex));
			le_token();
			return 1;
		}
	}
	else if (token == TKPop)
	{
		le_token();
		if (pop())
		{
			return 1;
		}
	}
	else if (token == TKSoma)
	{
		le_token();
		int topo, subtopo;
		if (pegaToposPilha(topo, subtopo))
		{
			const int res{ subtopo + topo };
			pilha.push(res);
			return 1;
		}
	}
	else if (token == TKSubtracao)
	{
		le_token();
		int topo, subtopo;
		if (pegaToposPilha(topo, subtopo))
		{
			const int res{ subtopo - topo };
			pilha.push(res);
			return 1;
		}
	}
	else if (token == TKMult)
	{
		le_token();
		int topo, subtopo;
		if (pegaToposPilha(topo, subtopo))
		{
			const int res{ subtopo * topo };
			pilha.push(res);
			return 1;
		}
	}
	else if (token == TKDiv)
	{
		le_token();
		int topo, subtopo;
		if (pegaToposPilha(topo, subtopo))
		{
			const int res{ subtopo / topo };
			pilha.push(res);
			return 1;
		}
	}
	else if (token == TKMaior)
	{
		le_token();
		int topo, subtopo;
		if (pegaToposPilha(topo, subtopo))
		{
			const int res{ subtopo > topo };
			pilha.push(res);
			return 1;
		}
	}
	else if (token == TKMenor)
	{
		le_token();
		int topo, subtopo;
		if (pegaToposPilha(topo, subtopo))
		{
			const int res{ subtopo < topo };
			pilha.push(res);
			return 1;
		}
	}
	else if (token == TKIgual)
	{
		le_token();
		int topo, subtopo;
		if (pegaToposPilha(topo, subtopo))
		{
			const int res{ subtopo == topo };
			pilha.push(res);
			return 1;
		}
	}
	else if (token == TKValorR)
	{
		le_token();
		if (token == TKId)
		{
			auto f = std::find_if(tab_simbolos.begin(), tab_simbolos.end(), [](struct tabela_simbolos t) { return t.lex == lex; });
			if (f != tab_simbolos.end())
			{
				pilha.push(f->valor);
				le_token();
				return 1;
			}
		}
	}
	else if (token == TKValorL)
	{
		le_token();
		if (token == TKId)
		{
			auto f = std::find_if(tab_simbolos.begin(), tab_simbolos.end(), [](struct tabela_simbolos t) { return t.lex == lex; });
			int pos;

			if (f == tab_simbolos.end())
			{
				struct tabela_simbolos ts = { lex, token, 0 };
				pos = tab_simbolos.size();
				tab_simbolos.push_back(ts);
			}
			else
			{
				pos = std::distance(tab_simbolos.begin(), f);
			}

			pilha.push(pos);

			le_token();

			return 1;
		}
	}
	else if (token == TKAtrib)
	{
		le_token();
		int valor;
		if (top(valor))
		{
			if (pop())
			{
				int pos;
				if (top(pos))
				{
					tab_simbolos[pos].valor = valor;
					std::cout << tab_simbolos[pos].lex << " = " << tab_simbolos[pos].valor << std::endl;
					return 1;
				}
			}
		}
	}
	else if (token == TKRotulo)
	{
		le_token();
		if (token == TKId)
		{
			le_token();
			if (token == TKDoisPontos)
			{
				le_token();
				return 1;
			}
		}
	}
	else if (token == TKGoto)
	{
		le_token();
		if (token == TKId)
		{
			int pos = entrada.find("rotulo " + lex + ":");
			if (pos != std::string::npos)
			{
				posbuffer = pos;
				le_token();
				return 1;
			}
		}
	}
	else if (token == TKGoFalse)
	{
		le_token();
		if (token == TKId)
		{
			int val;
			if (top(val))
			{
				if (pop())
				{
					const std::string rotulo{ "rotulo " + lex + ":" };
					le_token();
					if (!val)
					{
						const int pos = entrada.find(rotulo) ;
						if (pos != std::string::npos)
						{
							posbuffer = pos;
							le_token();
							return 1;
						}
						return 0;
					}
					return 1;
				}
			}
		}
	}
	else if (token == TKGoTrue)
	{
		le_token();
		if (token == TKId)
		{
			int val;
			if (top(val))
			{
				if (pop())
				{
					const std::string rotulo{ "rotulo " + lex + ":" };
					le_token();
					if (val)
					{
						int pos = entrada.find(rotulo);
						if (pos != std::string::npos)
						{
							posbuffer = pos;
							le_token();
							return 1;
						}
						return 0;
					}
					return 1;
				}
			}
		}
	}
	return 0;
}

void le_arquivo()
{
	std::ifstream arq("pilha.stk");
	std::string linha;
	while (std::getline(arq, linha))
	{
		entrada += linha + "\n";
	}
}

int main()
{
	le_arquivo();
	le_token();
	while (token != TKFim)
	{
		if (CMD())
		{
			//		std::cout << pilha.top();
		}
		else
		{
			std::cout << "Erro lex " << lex << " token " << token << std::endl;
			break;
		}
	}
//	system("pause");
	return 0;
}
