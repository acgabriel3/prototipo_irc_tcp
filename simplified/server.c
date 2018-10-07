#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define MAX_CLIENTES 100

static unsigned int n_clientes = 0;
static int uid = 10;


typedef struct {
	struct sockaddr_in addr;
	int connfd;
	int uid;
	char name[32];
} client_t;

client_t *clientes[MAX_CLIENTES];

void ativos_add(client_t *cl) {
	int i;
	for(i = 0; i < MAX_CLIENTES; ++i) {
		if(!clientes[i]) {
			clientes[i] = cl;
			return;
		}
	}
}

void ativos_remover(int uid) {
	int i;
	for(i = 0; i < MAX_CLIENTES; ++i) {
		if(clientes[i]) {
			if(clientes[i]->uid == uid) {
				clientes[i] = NULL;
				return;
			}
		}
	}
}

void enviar_mensagem(char *s, int uid) {
	int i;
	for(i = 0;i < MAX_CLIENTES; ++i) {
		if(clientes[i]) {
			if(clientes[i]->uid != uid) {
				write(clientes[i]->connfd, s, strlen(s));
			}
		}
	}
}

void enviar_mensagem_todos(char *s) {
	int i;
	for(i = 0; i < MAX_CLIENTES; ++i) {
		if(clientes[i]) {
			write(clientes[i]->connfd, s, strlen(s));
		}
	}
}

void enviar_mensagem_mim(const char *s, int connfd) {
	write(connfd, s, strlen(s));
}

void enviar_mensagem_client(char *s, int uid) {
	int i;
	for(i=0;i<MAX_CLIENTES;i++) {
		if(clientes[i]) {
			if(clientes[i]->uid == uid) {
				write(clientes[i]->connfd, s, strlen(s));
			}
		}
	}
}

void enviar_clientes_ativos(int connfd) {
	int i;
	char s[64];
	for(i = 0; i < MAX_CLIENTES; ++i) {
		if(clientes[i]) {
			sprintf(s, "<<CLIENT %d | %s\r\n", clientes[i]->uid, clientes[i]->name);
			enviar_mensagem_mim(s, connfd);
		}
	}
}

void remove_novalinha(char *s) {
	while(*s != '\0') {
		if(*s == '\r' || *s == '\n') {
			*s = '\0';
		}
		s++;
	}
}

void imprimir_ip_cliente(struct sockaddr_in addr) {
	printf("%d.%d.%d.%d",
		addr.sin_addr.s_addr & 0xFF,
		(addr.sin_addr.s_addr & 0xFF00)>>8,
		(addr.sin_addr.s_addr & 0xFF0000)>>16,
		(addr.sin_addr.s_addr & 0xFF000000)>>24);
}

void *handle_client(void *arg) {
	char buff_out[1024];
	char buff_in[1024];
	int rlen;

	n_clientes++;
	client_t *cli = (client_t *)arg;

	printf("<<ACCEPT ");
	imprimir_ip_cliente(cli->addr);
	printf(" REFERENCIADO PELO UID %d\n", cli->uid);

	sprintf(buff_out, "<<ENTRADA, OLÁ %s\r\n", cli->name);
	enviar_mensagem_todos(buff_out);

	/* Receive input from client */
	while((rlen = read(cli->connfd, buff_in, sizeof(buff_in)-1)) > 0) {
	        buff_in[rlen] = '\0';
	        buff_out[0] = '\0';
		remove_novalinha(buff_in);

		/* Ignore empty buffer */
		if(!strlen(buff_in)) {
			continue;
		}

		/* Special options */
		if(buff_in[0] == '\\') {
			char *command, *param;
			command = strtok(buff_in," ");
			if(!strcmp(command, "\\SAIR")) {
				break;
			}else if(!strcmp(command, "\\PING")) {
				enviar_mensagem_mim("<<PONG\r\n", cli->connfd);
			}else if(!strcmp(command, "\\NICK")) {
				param = strtok(NULL, " ");
				if(param) {
					char *old_name = strdup(cli->name);
					strcpy(cli->name, param);
					sprintf(buff_out, "<<NICK, %s PARA %s\r\n", old_name, cli->name);
					free(old_name);
					enviar_mensagem_todos(buff_out);
				}else{
					enviar_mensagem_mim("<<NICK NÃO PODE ESTAR VAZIO\r\n", cli->connfd);
				}
			}else if(!strcmp(command, "\\MENSAGEM")) {
				param = strtok(NULL, " ");
				if(param) {
					int uid = atoi(param);
					param = strtok(NULL, " ");
					if(param) {
						sprintf(buff_out, "[MP][%s]", cli->name);
						while(param != NULL) {
							strcat(buff_out, " ");
							strcat(buff_out, param);
							param = strtok(NULL, " ");
						}
						strcat(buff_out, "\r\n");
						enviar_mensagem_client(buff_out, uid);
					}else{
						enviar_mensagem_mim("<<MENSAGEM NÃO PODE ESTAR VAZIA\r\n", cli->connfd);
					}
				}else{
					enviar_mensagem_mim("<<REFERENCIA NÃO PODE ESTAR NULA\r\n", cli->connfd);
				}
			}else if(!strcmp(command, "\\LISTA")) {
				sprintf(buff_out, "<<CLIENTES %d\r\n", n_clientes);
				enviar_mensagem_mim(buff_out, cli->connfd);
				enviar_clientes_ativos(cli->connfd);
			}else if(!strcmp(command, "\\AJUDA")) {
				strcat(buff_out, "\\SAIR     Sair do servidor IRC\r\n");
				strcat(buff_out, "\\PING     Testar servidor\r\n");
				strcat(buff_out, "\\NICK     <nick> para alterar seu nickname\r\n");
				strcat(buff_out, "\\MENSAGEM  <nick> <mensagem> Enviar mensagem privada\r\n");
				strcat(buff_out, "\\LISTA   Mostrar clientes ativos\r\n");
				strcat(buff_out, "\\AJUDA     Mostrar ajuda\r\n");
				enviar_mensagem_mim(buff_out, cli->connfd);
			}else{
				enviar_mensagem_mim("<<COMANDO DESCONHECIDO\r\n", cli->connfd);
			}
		}else{
			sprintf(buff_out, "[%s] %s\r\n", cli->name, buff_in);
			enviar_mensagem(buff_out, cli->uid);
		}
	}

	close(cli->connfd);
	sprintf(buff_out, "<<SAIU, TCHAU %s\r\n", cli->name);
	enviar_mensagem_todos(buff_out);

	ativos_remover(cli->uid);
	printf("<<SAIDA ");
	imprimir_ip_cliente(cli->addr);
	printf(" REFERENCIADO POR %d\n", cli->uid);
	free(cli);
	n_clientes--;

	return NULL;
}

int main(int argc, char *argv[]) {
	int i = 0, listenfd = 0, connfd = 0, max_fd = 0, fd = 0, atividade = 0;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	int sockets[MAX_CLIENTES];
	pthread_t tid;
	fd_set fds;

	char *bemvindo = "BEM-VINDO! \r\n";

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(5000);

	if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("Socket binding failed");
		return 1;
	}

	if(listen(listenfd, 10) < 0) {
		perror("Socket listening failed");
		return 1;
	}

	for(i = 0; i < MAX_CLIENTES; ++i) {
		sockets[i] = 0;
	}

	printf("<[SERVIDOR INICIADO]>\n");

	while(1) {
		FD_ZERO(&fds);

		FD_SET(listenfd, &fds);

		max_fd = listenfd;

		for(i = 0; i < MAX_CLIENTES; ++i)
		{
			fd = sockets[i];

			if(fd > 0)
				FD_SET(fd, &fds);

			if(fd > max_fd)
				max_fd = fd;
		}

		atividade = select(max_fd + 1, &fds, NULL, NULL, NULL);

		if((atividade < 0) && (errno != EINTR))
		{
			printf("ERRO NO SELECT\n");
		}

		if(FD_ISSET(listenfd, &fds)) {
			socklen_t clilen = sizeof(cli_addr);

			if((connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen)) < 0) {
				perror("accept");
				return -1;
			}

			if((n_clientes+1) == MAX_CLIENTES) {
				printf("<<MÁXIMO DE CLIENTES ATINGIDO\n");
				printf("<<REJEITAR ");
				imprimir_ip_cliente(cli_addr);
				printf("\n");
				close(connfd);
				continue;
			}

			if( send(connfd, bemvindo, strlen(bemvindo), 0) != strlen(bemvindo))
            {
                perror("send");
            }

            printf("Mensagem de boas-vindas enviada com sucesso");
			client_t *cli = (client_t *)malloc(sizeof(client_t));
			cli->addr = cli_addr;
			cli->connfd = connfd;
			cli->uid = uid++;
			sprintf(cli->name, "%d", cli->uid);

			ativos_add(cli);
		}
		sleep(1);
	}
}
