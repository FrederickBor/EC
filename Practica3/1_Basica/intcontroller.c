/*-------------------------------------------------------------------
**
**  Fichero:
**    intcontroller.c  3/3/2016
**
**    Estructura de Computadores
**    Dpto. de Arquitectura de Computadores y Automática
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las implementación del módulo intcontroller
**
**-----------------------------------------------------------------*/

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "intcontroller.h"

void ic_init(void)
{
	/* Configuración por defector del controlador de interrupciones:
	 *    Lineas IRQ y FIQ no habilitadas
	 *    Linea IRQ en modo no vectorizado
	 *    Todo por la línea IRQ
	 *    Todas las interrupciones enmascaradas
	 **/
	rINTMOD = 0x0; // Configura las linas como de tipo IRQ
	rINTCON = 0x7; // IRQ y FIQ enmascaradas, IRQ en modo no vectorizado
	rINTMSK = ~(0x0); // Enmascara todas las lineas
}

int ic_conf_irq(enum enable st, enum int_vec vec)
{
	int conf = rINTCON;

	if (st != ENABLE && st != DISABLE)
		return -1;

	if (vec == VEC)
		// Poner la linea IRQ en modo vectorizado
		conf |= (0x4);
	else
		// Poner la linea IRQ en modo no vectorizado
		conf &= ~(0x4); 

	if (st == ENABLE)
		// Habilitar la linea IRQ
		conf |= (0x10);
	else
		// Deshabilitar la linea IRQ
		conf &= ~(0x10);

	rINTCON = conf;
	return 0;
}

int ic_conf_fiq(enum enable st)
{
	int ret = 0;
	int conf = rINTCON; // ¿Trabaja con el registro rINTCON?

	if (st == ENABLE){
		// Habilitar la linea FIQ
		conf |= (0x11);
		rINTCON = conf;
	}
	else if (st == DISABLE){
		// Deshabilitar la linea FIQ
		conf &= ~(0x11);
		rINTCON = conf;
	}
	else
		ret = -1;

	
	return ret;
}

int ic_conf_line(enum int_line line, enum int_mode mode)
{
	unsigned int bit = INT_BIT(line);

	if (line < 0 || line > 26)
		return -1;

	if (mode != IRQ && mode != FIQ)
		return -1;

	int registro = rINTMOD;

	if (mode == IRQ)
		// Poner la linea line en modo IRQ
		registro &= ~bit;
	else
		// Poner la linea line en modo FIQ
		registro |= bit;
	
	rINTMOD = registro;

	return 0;
}

int ic_enable(enum int_line line)
{
	if (line < 0 || line > 26)
		return -1;

	// Habilitar las interrupciones por la linea line
	// ESTO SOLO ACTIVA EL LAS INT POR IRQ
	int bit = INT_BIT(line);
	rI_ISPR |= bit;
	
	return 0;
}

int ic_disable(enum int_line line)
{
	if (line < 0 || line > 26)
		return -1;

	// Enmascarar las interrupciones por la linea line
	// ESTO SOLO DESACTIVA EL LAS INT POR IRQ
	int bit = INT_BIT(line);
	rI_ISPR &= ~bit;

	return 0;
}

int ic_cleanflag(enum int_line line)
{
	int bit;

	if (line < 0 || line > 26)
		return -1;

	bit = INT_BIT(line);

	if (rINTMOD & bit)
		// Borrar el flag de interrupcion correspondiente a la linea line
		//con la linea configurada por FIQ
		rF_ISPC |= bit;
	else
		// Borrar el flag de interrupcion correspondiente a la linea line
		//con la linea configurada por IRQ
		rI_ISPC |= bit;
	
	return 0;
}



