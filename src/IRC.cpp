#include "../include/IRC.hpp"

string DefServer::get_portaHost() {

	return this->portaHost;

}

string DefServer::get_mensagem() {

	return this->mensagem;

}

void ServidorCliente::criaSocket() {

	this->recebeSocket = socket(AF_INET, SOCK_STREAM, 0);

}

int ServidorCliente::get_recebeSocket() {

	return this->recebeSocket;

}

void ServidorAplicativo::criaSocket() {
	
	//ainda deve haver o tratamento de erros	
	this->recebeSocket = socket(AF_INET, SOCK_STREAM, 0);

}

void ServidorAplicativo::set_porta_ip() {

	//Ainda deve haver o tratamento de erros
	bind(this->recebeSocket, gethostname(), portaHost);

}