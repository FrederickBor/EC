/*-------------------------------------------------------------------
**
**  Fichero:
**    intcontroller.c
**
**    Estructura de Computadores
**    Dpto. de Arquitectura de Computadores y Autom�tica
**    Facultad de Inform�tica. Universidad Complutense de Madrid
**
**  Prop�sito:
**    Contiene las implementaci�n del m�dulo intcontroller
**
**-----------------------------------------------------------------*/

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "intcontroller.h"

void ic_init(void)
{
	/* Configuraci�n por defector del controlador de interrupciones:
	 *    L�neas IRQ y FIQ no habilitadas
	 *    L�nea IRQ en modo no vectorizado
	 *    Todo por la l�nea IRQ
	 *    Todas las interrupciones enmascaradas
	 **/
	rINTMOD = 0x0; // Configura las l�neas como de tipo IRQ
	rINTCON = 0x7; // IRQ y FIQ enmascaradas, IRQ en modo no vectorizado
	rINTMSK = ~(0x0); // Enmascara todas las l�neas
}

int ic_conf_irq(enum enable st, enum int_vec vec)
{
	int conf = rINTCON;

	if (st != ENABLE && st != DISABLE)
		return -1;

	if (vec == VEC){
		//COMPLETAR: poner la l�nea IRQ en modo vectorizado
		conf &= ~(0x4);
	}
	else{
		//COMPLETAR: poner la l�nea IRQ en modo no vectorizado
		conf |= (0x4);
	}
	if (st == ENABLE){
		//COMPLETAR: habilitar la l�nea IRQ
		conf &= ~(0x2);

	}
	else{
		//COMPLETAR: deshabilitar la l�nea IRQ
		conf |= (0x2);

	}
	rINTCON = conf;
	return 0;
}

int ic_conf_fiq(enum enable st)
{
	int ret = 0;

	if (st == ENABLE){
		//COMPLETAR: habilitar la l�nea FIQ
		rINTCON &= ~(0x1);

	}
	else if (st == DISABLE){
		//COMPLETAR: deshabilitar la l�nea FIQ
		rINTCON |= (0x1);
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

	if (mode == IRQ){
		//COMPLETAR: poner la l�nea indicada en line en modo IRQ
		rINTMOD &= ~bit;
	}
	else{
		//COMPLETAR: poner la l�nea indicada en line en modo FIQ
		rINTMOD |= bit;
	}
	return 0;
}

int ic_enable(enum int_line line)
{
	if (line < 0 || line > 26)
		return -1;

	//COMPLETAR: habilitar las interrupciones por la l�nea indica en line
	//habilitar tambi�n bit global

	rINTMSK &= ~INT_BIT(line);
	rINTMSK &= ~INT_BIT(26);

	return 0;
}

int ic_disable(enum int_line line)
{
	if (line < 0 || line > 26)
		return -1;

	//COMPLETAR: enmascarar las interrupciones por la l�nea indicada en line
	rINTMSK |= INT_BIT(line);

	return 0;
}

int ic_cleanflag(enum int_line line)
{
	int bit;

	if (line < 0 || line > 26)
		return -1;

	bit = INT_BIT(line);

	if (rINTMOD & bit){
		//COMPLETAR: borrar el flag de interrupci�n correspondiente a la l�nea indicada en line
		//con la l�nea configurada por FIQ
		rF_ISPC |= bit;


	}
	else{
		//COMPLETAR: borrar el flag de interrupci�n correspondiente a la l�nea indicada en line
		//con la l�nea configurada por IRQ
		rI_ISPC |= bit;


	}
	return 0;
}
