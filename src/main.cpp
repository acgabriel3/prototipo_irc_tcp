#include "IRC.hpp"

/* 
	Este codigo utiliza o padrao de programacao google: c++ style guide (pode ser facilmente encontrado online)

	Autoria: ****Vamos colocar aqui nossos nomes e matriculas****
*/

int main(void) {

	string hostname = "hostTeste";
	string nickname = "acgabriel";
	string canal = "canal01";

	ServidorCliente serverClient(nickname, hostname, canal);

	serverClient.criaSocket();

	cout << serverClient.get_recebeSocket();

	return 0;

}
