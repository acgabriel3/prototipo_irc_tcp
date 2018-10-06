#include<bits/stdc++.h>

#include<sys/socket.h>
#include<sys/types.h>
#include<vector>

using namespace std;

class DefServer {

	private:
		int portaHost = 65000;
		string mensagem = "boraQueDA";

	public:
		int get_portaHost();
		string get_mensagem();

		DefServer() {

		}
};

class Canal {
	public:
		string nome;
		vector<string> cliente;

		Canal() {

		} 
};

class ServidorCliente {
	private:
		string hostname;
		string nickname; 
		string canal;
		int recebeSocket;

	public:
		ServidorCliente(string nickname,string hostname, string canal) {
			this->hostname = hostname;
			this->nickname = nickname;
			this->canal = canal;
		}

		//nao tenho certeza se seria assim que deveria ser utilizado o socket do cliente
		void criaSocket();

		int get_recebeSocket();
};


class ServidorAplicativo {
	
	private:
		vector<string> clientes;
		vector<Canal> canais;

		int recebeSocket;

		//Constantes para identificar os comandos
		string SAIR = "SAIR";
		string NICK = "NICK";
		string ENTRAR = "ENTRAR";
		string SAIRC = "SAIRC";
		string LISTAR = "LISTAR";

	public:

		void criaSocket();

}