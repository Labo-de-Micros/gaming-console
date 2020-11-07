#include "gpio.h"
#include "MK64F12.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//											Definitions														//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

#define PIN_IRQ_ENABLED		true
#define PIN_IRQ_DISABLED	false
typedef void (*IRQ_callback_t)(void);

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//										Local variables													//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

static GPIO_Type* gpioPtrs[] = GPIO_BASE_PTRS;
static PORT_Type* portPtrs[] = PORT_BASE_PTRS;
static uint32_t simMasks[] = {SIM_SCGC5_PORTA_MASK, SIM_SCGC5_PORTB_MASK, SIM_SCGC5_PORTC_MASK, SIM_SCGC5_PORTD_MASK, SIM_SCGC5_PORTE_MASK };
static uint32_t IRQn_ports[] = {PORTA_IRQn, PORTB_IRQn, PORTC_IRQn, PORTD_IRQn, PORTE_IRQn};
static bool pin_ports_activated[5][32];
static SIM_Type* sim_ptr = SIM;
static IRQ_callback_t callback;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//								Local Functions definitions									//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

static	void PORT_ClearInterruptFlag(uint32_t port_num, uint32_t pin_num);

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//									Public services														//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void gpioMode (pin_t pin, uint8_t mode){
	/* * @brief Configures the specified pin to behave either as an input or an output
	 * @param pin the pin whose mode you wish to set (according PORTNUM2PIN)
	 * @param mode INPUT, OUTPUT, INPUT_PULLUP or INPUT_PULLDOWN.
	 */
	sim_ptr->SCGC5 |= simMasks[PIN2PORT(pin)]; // activo clock gating
	PORT_Type *port = portPtrs[PIN2PORT(pin)];
	GPIO_Type *gpio = gpioPtrs[PIN2PORT(pin)];
	uint32_t num = PIN2NUM(pin); // num es el numero de pin

	// connect to gpio (hay un PCR por pin)
	port->PCR[num] = 0x00;
	port->PCR[num] |= PORT_PCR_MUX(1);
	port->PCR[num] |= PORT_PCR_IRQC(0);
	switch(mode){
		case INPUT:
			gpio->PDDR &= ~ (1<<num); // seteamos el pin como input
			break;
		case OUTPUT:
			gpio->PDDR |= 1<<num;// seteamos el pin como output
			break;
		case INPUT_PULLUP:
			gpio->PDDR &= ~ (1<<num); // seteamos el pin como input
			port->PCR[num] |= HIGH<<1; //PULL ENABLE en 1
			port->PCR[num] |= HIGH<<0; //PULL SELECT en 1 (PULLUP)
			break;
		case INPUT_PULLDOWN:
			gpio->PDDR &= ~ (1<<num); // seteamos el pin como input
			port->PCR[num] |= HIGH<<1; //PULL ENABLE en 1
			port->PCR[num] &= ~(HIGH<<0); //PULL SELECT en 0 (PULLDOWN)
			break;
	}
	return;
}

void gpioWrite (pin_t pin, bool value){
	/**
	 * @brief Write a HIGH or a LOW value to a digital pin
	 * @param pin the pin to write (according PORTNUM2PIN)
	 * @param val Desired value (HIGH or LOW)
	 */
	uint32_t port_name = PIN2PORT(pin);
	uint32_t num = PIN2NUM(pin);
	GPIO_Type *gpio = gpioPtrs[port_name];
	if(value == HIGH)
		gpio->PDOR |= (1<<num);
	else
		gpio->PDOR &= ~ (1<<num);
}

void gpioToggle (pin_t pin){
	/**
	 * @brief Toggle the value of a digital pin (HIGH<->LOW)
	 * @param pin the pin to toggle (according PORTNUM2PIN)
	 */
	GPIO_Type *gpio = gpioPtrs[PIN2PORT(pin)];
	gpio->PTOR |= (1<<PIN2NUM(pin));
}

bool gpioRead (pin_t pin){
	/**
	 * @brief Reads the value from a specified digital pin, either HIGH or LOW.
	 * @param pin the pin to read (according PORTNUM2PIN)
	 * @return HIGH or LOW
	 */
	uint32_t port_name = PIN2PORT(pin);
	uint32_t num = PIN2NUM(pin);
	GPIO_Type *gpio = gpioPtrs[port_name];
	return ( (1<<num) & gpio->PDIR ) == 1<<num;
}

bool gpioIRQ (pin_t pin, uint8_t irqMode, pinIrqFun_t irqFun){
	/**
	 * @brief Configures how the pin reacts when an IRQ event ocurrs
	 * @param pin the pin whose IRQ mode you wish to set (according PORTNUM2PIN)
	 * @param irqMode disable, risingEdge, fallingEdge or bothEdges
	 * @param irqFun function to call on pin event
	 * @return Registration succeed
	 */
	uint32_t port_num = PIN2PORT(pin);
	uint32_t pin_num = PIN2NUM(pin);
	pin_ports_activated[port_num][pin_num] = PIN_IRQ_ENABLED;
	PORT_Type * addr_arrays[] = PORT_BASE_PTRS;
	PORT_Type * port = addr_arrays[port_num];

	__NVIC_EnableIRQ(IRQn_ports[port_num]);
	__NVIC_ClearPendingIRQ(IRQn_ports[port_num]);
	__NVIC_SetPriority(IRQn_ports[port_num], 3);

	switch(irqMode){
			case GPIO_IRQ_MODE_DISABLE:
				port->PCR[pin_num] &= ~(0b1111 << PORT_PCR_IRQC_SHIFT);
			case GPIO_IRQ_MODE_RISING_EDGE:
				port->PCR[pin_num] &= ~(0b1111 << PORT_PCR_IRQC_SHIFT);
				port->PCR[pin_num] |= 0b1001 << PORT_PCR_IRQC_SHIFT;
				break;
			case GPIO_IRQ_MODE_FALLING_EDGE:
				port->PCR[pin_num] &= ~(0b1111 << PORT_PCR_IRQC_SHIFT);
				port->PCR[pin_num] |= 0b1010 << PORT_PCR_IRQC_SHIFT;
				break;
			case GPIO_IRQ_MODE_BOTH_EDGES:
				port->PCR[pin_num] &= ~(0b1111 << PORT_PCR_IRQC_SHIFT);
				port->PCR[pin_num] |= 0b1011 << PORT_PCR_IRQC_SHIFT;
				break;
			case GPIO_IRQ_CANT_MODES:
				break;
			default:
				break;
		}

	callback = irqFun;
	return true;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//												Handlers														//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void PORTA_IRQHandler(void){
	for(int i=0; i<32; i++) {
		if(pin_ports_activated[PA][i]==PIN_IRQ_ENABLED)
			PORT_ClearInterruptFlag(PA,i);
	}
	callback();
	return;
}

void PORTB_IRQHandler(void){
	for(int i=0; i<32; i++) {
			if(pin_ports_activated[PB][i]==PIN_IRQ_ENABLED)
				PORT_ClearInterruptFlag(PB,i);
		}
		callback();
		return;
}

void PORTC_IRQHandler(void){
	for(int i=0; i<32; i++) {
		if(pin_ports_activated[PC][i]==PIN_IRQ_ENABLED)
			PORT_ClearInterruptFlag(PC,i);
	}
	callback();
	return;
}

void PORTD_IRQHandler(void){
	for(int i=0; i<32; i++) {
		if(pin_ports_activated[PD][i]==PIN_IRQ_ENABLED)
			PORT_ClearInterruptFlag(PD,i);
	}
	callback();
	return;
}

void PORTE_IRQHandler(void){
	for(int i=0; i<32; i++) {
		if(pin_ports_activated[PE][i]==PIN_IRQ_ENABLED)
			PORT_ClearInterruptFlag(PE,i);
	}
	callback();
	return;
}

void PORT_ClearInterruptFlag(uint32_t port_num, uint32_t pin_num) {
	PORT_Type * addr_arrays[] = PORT_BASE_PTRS;
	PORT_Type * port = addr_arrays[port_num];
	port->PCR[pin_num] |= PORT_PCR_ISF_MASK;
	return;
}
