#include <stdio.h>
#include "44b.h"
#include "button.h"
#include "leds.h"
#include "utils.h"
#include "D8Led.h"
#include "intcontroller.h"
#include "timer.h"
#include "gpio.h"
#include "keyboard.h"

#define LETRA_E 14
#define LETRA_C 12

struct RLstat {
	int moving;
	int speed;
	int direction;
	int position;
};

static struct RLstat RL = {
	.moving = 0,
	.speed = 5,
	.direction = 0,
	.position = 0,
};

static int contBUT2 = 0;
static int contTIMER2 = 0;

void timer_ISR(void) __attribute__ ((interrupt ("IRQ")));
void timer2_ISR(void) __attribute__ ((interrupt ("IRQ")));
void button_ISR(void) __attribute__ ((interrupt ("IRQ")));
void keyboard_ISR(void) __attribute__ ((interrupt ("IRQ")));

void timer_ISR(void)
{
	// Tomar el código de avance de posición del led rotante de la práctica anterior
	if (RL.moving) {
		if(RL.direction == 0) {
			RL.position--;

			if(RL.position < 0)
				RL.position = 5;
		}
		else {
			RL.position++;

			if(RL.position > 5)
				RL.position = 0;
		}
	}
	D8Led_segment(RL.position);
	ic_cleanflag(INT_TIMER0);
}

void timer2_ISR(void)
{
	if (contTIMER2 % 4 == 0) {
		D8Led_digit(LETRA_E);
		Delay(200);
		D8Led_digit(LETRA_C);
		Delay(200);
	}

	contTIMER2++;

	ic_cleanflag(INT_TIMER2);
}


void button_ISR(void)
{
	unsigned int whicheint = rEXTINTPND;
	unsigned int buttons = (whicheint >> 2) & 0x3;

	// Usar el código de la primera parte parte de atención a los
	// pulsadores
	if (buttons & BUT1) {
		// Utilizando la interfaz para los leds definida en leds.h
		// hay que apagar ambos leds
		// También hay que comutar la dirección del movimiento del led rotante
		// representado por el campo direction de la variable RL
		led1_off();
		led2_off();

		RL.direction = ~RL.direction;

	}

	if (buttons & BUT2) {
		// Utilizando la interfaz para los leds definida en leds.h
		// Incrementar contador de pulsaciones. Si es par, conumtar led1. Si es impar, conmutar el led2.
		// También hay que comutar el estado de movimiento del led rotante
		// representado por el campo moving de la variable RL, y en caso de
		// ponerlo en marcha debemos reiniciar el campo iter al valor del campo
		// speed.

		contBUT2++;

		if(contBUT2%2 == 0)
			led1_switch();
		else
			led2_switch();

		RL.moving = ~(RL.moving);

	}

	// eliminamos rebotes
	Delay(200);
	// borramos el flag en extintpnd
	// Debemos borrar las peticiones de interrupción en
	// EXTINTPND escribiendo un 1 en los flags que queremos borrar (los
	// correspondientes a los pulsadores pulsados)
	rEXTINTPND |= (0x3 << 2);

	ic_cleanflag(INT_EINT4567);
}

void keyboard_ISR(void)
{
	int key;

	/* Eliminar rebotes de presión */
	Delay(200);
	
	/* Escaneo de tecla */
	key = kb_scan();

	if (key != -1) {
		/* Visualizacion en el display */
		// Mostrar la tecla en el display utilizando el interfaz
		//definido en D8Led.h
		D8Led_digit(key);
		switch (key) {
			case 0:
				// Poner en timer0 divisor 1/8 y contador 62500
				tmr_set_divider(0, D1_8);
				tmr_set_count(TIMER0, 62500, 0);
				tmr_update(TIMER0);
				break;
			case 1:
				// Poner en timer0 timer divisor 1/8 y contador 31250
				tmr_set_divider(0, D1_8);
				tmr_set_count(TIMER0, 31250, 0);
				tmr_update(TIMER0);
				break;
			case 2:
				// Poner en timer0 timer divisor 1/8 y contador 15625
				tmr_set_divider(0, D1_8);
				tmr_set_count(TIMER0, 15625, 0);
				tmr_update(TIMER0);
				break;
			case 3:
				// Poner en timer0 timer divisor 1/4 y contador 15625
				tmr_set_divider(0, D1_4);
				tmr_set_count(TIMER0, 15625, 0);
				tmr_update(TIMER0);
				break;
			default:
				break;
		}
		
		/* Esperar a que la tecla se suelte, consultando el registro de datos */		
		while (/* True si está pulsada la tecla (leer del registro rPDATG)*/ !(rPDATG & 0x2));
	}

    /* Eliminar rebotes de depresión */
    Delay(200);
     
    /* Borrar interrupciones pendientes */
	// Borrar la interrupción por la línea EINT1 en el registro rI_ISPC
	rI_ISPC &= ~(0x4 << 16);

	ic_cleanflag(INT_EINT1);
}

int setup(void)
{
	leds_init();
	D8Led_init();

	D8Led_digit(LETRA_E);
	Delay(200);
	D8Led_digit(LETRA_C);
	Delay(200);

	D8Led_segment(RL.position);

	/* Port G: configuración para generación de interrupciones externas,
	 *         botones y teclado
	 **/

	// Utilizando el interfaz para el puerto G definido en gpio.h
	//configurar los pines 1, 6 y 7 del puerto G para poder generar interrupciones
	//externas por flanco de bajada por ellos y activar las correspondientes
	//resistencias de pull-up.

	portG_conf(6, EINT);
	portG_eint_trig(6, FALLING);
	portG_conf_pup(6, ENABLE);

	portG_conf(7, EINT);
	portG_eint_trig(7,FALLING);
	portG_conf_pup(7, ENABLE);

	portG_conf(1, EINT);
	portG_eint_trig(1, FALLING);
	portG_conf_pup(1, ENABLE);
	/********************************************************************/


	/* Configuración del timer */

	// Tomar el código de la segunda parte
	tmr_set_prescaler(0, 255);
	tmr_set_divider(TIMER0, D1_8);
	tmr_set_count(TIMER0, 62500, 2000);
	tmr_set_mode(TIMER0, RELOAD);
	tmr_update(TIMER0);
	tmr_stop(TIMER0);

	tmr_set_prescaler(2, 255);
	tmr_set_divider(TIMER2, D1_8);
	tmr_set_count(TIMER2, 62500, 0);
	tmr_set_mode(TIMER2, RELOAD);
	tmr_update(TIMER2);
	tmr_stop(TIMER2);

	//if (RL.moving)
	tmr_start(TIMER0);
	tmr_start(TIMER2);

	/***************************/


	// Registramos las ISRs
	pISR_TIMER0   = (unsigned) timer_ISR;// Registrar la RTI del timer
	pISR_TIMER2 = (unsigned) timer2_ISR;
	pISR_EINT4567 = (unsigned) button_ISR;// Registrar la RTI de los botones
	pISR_EINT1    = (unsigned) keyboard_ISR;// Registrar la RTI del teclado

	/* Configuración del controlador de interrupciones
	 * Habilitamos la línea IRQ, en modo vectorizado 
	 * Configuramos el timer 0 en modo IRQ y habilitamos esta línea
	 * Configuramos la línea EINT4567 en modo IRQ y la habilitamos
	 * Configuramos la línea EINT1 en modo IRQ y la habilitamos
	 */

	ic_init();
	// Utilizando el interfaz definido en intcontroller.h
	// habilitar la línea IRQ en modo vectorizado
	ic_conf_irq(ENABLE,VEC);
	// deshabilitar la línea FIQ
	ic_conf_fiq(DISABLE);
	// configurar la línea INT_TIMER0 en modo IRQ
	ic_conf_line(INT_TIMER0,IRQ);
	// configurar la línea INT_TIMER2 en modo IRQ
	ic_conf_line(INT_TIMER2,IRQ);
	// configurar la línea INT_EINT4567 en modo IRQ
	ic_conf_line(INT_EINT4567,IRQ);
	// configurar la línea INT_EINT1 en modo IRQ
	ic_conf_line(INT_EINT1,IRQ);
	// habilitar la línea INT_TIMER0
	ic_enable(INT_TIMER0);
	// habilitar la línea INT_TIMER2
	ic_enable(INT_TIMER2);
	// habilitar la línea INT_EINT4567
	ic_enable(INT_EINT4567);
	// habilitar la línea INT_EINT1
	ic_enable(INT_EINT1);

	/***************************************************/

	Delay(0);
	return 0;
}

int loop(void)
{
	return 0;
}


int main(void)
{
	setup();

	while (1) {
		loop();
	}
}
