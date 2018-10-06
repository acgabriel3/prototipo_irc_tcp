#include "IRC.hpp"

DefServer::get_portaHost() {
	return this->portaHost;
}

DefServer::get_mensagem() {
	return this->mensagem;
}

ServidorCliente::criaSocket() {
	this->recebeSocket = socket(AF_INET, SOCK_STREAM, 0);
}

ServidorCliente::get_recebeSocket() {
	return this->recebeSocket;
}
