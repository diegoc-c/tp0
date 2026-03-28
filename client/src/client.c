#include "client.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	if (!logger) {
		return 1;
	}

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();

	if (!config) {
		log_error(logger, "No se pudo crear el config");
		return 1;
	}

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'

	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	valor = config_get_string_value(config, "CLAVE");

	// Loggeamos el valor de config

	log_info(logger, "Hola! Soy un log: %s", valor);

	/* ---------------- LEER DE CONSOLA ---------------- */

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	log_info(logger, "Conectandose a %s:%s", ip, puerto);

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto, logger);

	if (conexion < 0) {
		log_error(logger, "No se pudo conectar al servidor");
		return 1;
	}

	// Enviamos al servidor el valor de CLAVE como mensaje

	enviar_mensaje(valor, conexion);

	// Armamos y enviamos el paquete
	paquete(conexion, logger);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log* iniciar_logger(void)
{
	return log_create("tp0.log", "TP0", true, LOG_LEVEL_INFO);
}

t_config* iniciar_config(void)
{
	return config_create("cliente.config");
}

static bool linea_valida(char *leido) {
	return leido && leido[0] != '\0';
}

void paquete(int conexion, t_log *logger)
{
	// Ahora toca lo divertido!
	char* leido = NULL;
	t_paquete* paquete = crear_paquete();

	// Leemos y esta vez agregamos las lineas al paquete

	do {
		free(leido);
		leido = readline("> ");

		if (linea_valida(leido)) {
			agregar_a_paquete(paquete, leido, strlen(leido) + 1);
			log_info(logger, "Se agrego '%s' a paquete", leido);
		}
	} while (linea_valida(leido));

	enviar_paquete(paquete, conexion);

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
}
