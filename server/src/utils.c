#include"utils.h"

#include <errno.h>
#include <arpa/inet.h>

t_log* logger;

int iniciar_servidor()
{
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, PUERTO, &hints, &servinfo);

	// Creamos el socket de escucha del servidor

	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	if (socket_servidor < 0) {
		log_error(logger, "socket: %s", strerror(errno));
		return -1;
	}

	// Asociamos el socket a un puerto

	if (bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
		log_error(logger, "bind: %s", strerror(errno));
		return -1;
	}

	// Escuchamos las conexiones entrantes

	if (listen(socket_servidor, 10) < 0) {
		log_error(logger, "listen: %s", strerror(errno));
		return -1;
	}

	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;
}

static void logear_ip_cliente(struct sockaddr_storage *dir) {	
	char ip[INET6_ADDRSTRLEN];
	int puerto;

	if (dir->ss_family == AF_INET) {
		struct sockaddr_in *s = (struct sockaddr_in *)dir;
		puerto = ntohs(s->sin_port);
		inet_ntop(AF_INET, &s->sin_addr, ip, sizeof(ip));
	} else {
		struct sockaddr_in6 *s = (struct sockaddr_in6 *)dir;
		puerto = ntohs(s->sin6_port);
		inet_ntop(AF_INET6, &s->sin6_addr, ip, sizeof(ip));
	}

	log_info(logger, "Direccion cliente: %s:%d", ip, puerto);
}

int esperar_cliente(int socket_servidor)
{
	struct sockaddr_storage dir_cliente;
	socklen_t dir_cliente_tam = sizeof(dir_cliente);
	// Aceptamos un nuevo cliente
	int socket_cliente = accept(socket_servidor, (struct sockaddr *)&dir_cliente, &dir_cliente_tam);

	log_info(logger, "Se conecto un cliente!");
	logear_ip_cliente(&dir_cliente);

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}
