#include "uart2.h"

#define ABS(x) ((x) < 0 ? - (x) : (x))

void uart2Config(uint32_t baudrate, uint32_t fcy, uartModeType uartMode) {
	float tmp;

	if ((baudrate > 152) && (baudrate < 10E6)) {
		U2MODEbits.UARTEN = 0;          // désactive le périphérique avant de le configurer
		U2MODE = uartMode;              // spécifie le mode de fonctionnement
		tmp = fcy/(16.0*baudrate) - 1;    // on calcule la valeur exacte à placer dans le compteur pour obtenir le baudrate désiré
		U2MODEbits.BRGH = 0;            // on décide de toujours fonctionner en high speed (cela nous limite à 152 baud minimum)
		U2BRG = tmp + 0.5;              // on assigne la valeur calculée pour le baudrate, ce qui la tronque à l'entier inférieur

		tmp = fcy/(16*(float)(U2BRG + 1));
		if (ABS(tmp-baudrate) > baudrate/50) {
			U2MODEbits.BRGH = 1;				// on décide de toujours fonctionner en high speed (cela nous limite à 152 baud minimum)
			tmp = fcy/(4.0*baudrate) - 1;
			U2BRG = tmp + 0.5;			// on assigne la valeur calculée pour le baudrate, ce qui la tronque à l'entier inférieur
		}
//		U2MODEbits.UARTEN = 1;				// on active l'UART
        //U2MODEbits.UEN = 2;
//		U2STAbits.UTXEN = 1;				// on active l'émission (Attention, cela active aussi U2TXIF)
	}
}


inline void uart2SendChar(unsigned char data) {
	while(U2STAbits.UTXBF);
	U2TXREG = data;
}


inline unsigned char uart2GetChar(void) {
	while(U2STAbits.URXDA == 0);
	return U2RXREG;
}

inline int16_t uart2RxDataAvailable(void) {
	return U2STAbits.URXDA;
}

inline int16_t uart2TxReady(void) {
	return !U2STAbits.UTXBF;
}


inline void uart2RxIsrDisable() {
	IEC1bits.U2RXIE = 0;
}

inline void uart2TxIsrDisable() {
	IEC1bits.U2TXIE = 0;
}

inline void uart2RxIsrEnable() {
	IEC1bits.U2RXIE = 1;
}

inline void uart2TxIsrEnable() {
	IFS1bits.U2TXIF = 0;
	IEC1bits.U2TXIE = 1;
}

