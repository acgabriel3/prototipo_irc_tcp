#include "IRC.hpp"

int main(void) {

	string hostname = "hostTeste";
	string nickname = "acgabriel";
	string canal = "canal01";

	ServidorCliente serverClient(nickname, hostname, canal);

	serverClient.criaSocket();

	cout << serverClient.get_recebeSocket();

	return 0;

}
