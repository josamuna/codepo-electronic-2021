/*!
 * Now in MCVE
 * @file uart.h
 * @author Kevin De Cuyper
 * @brief Header file pour la configuration des UART
 */

#ifndef UART2_H
#define	UART2_H

    #include <xc.h>

    #ifndef UART1_H
        typedef enum {
            UART_8N1_NO_HANDSHAKE	= 0x00,
            UART_8E1_NO_HANDSHAKE	= 0x02,
            UART_8O1_NO_HANDSHAKE	= 0x04,
            UART_9N1_NO_HANDSHAKE	= 0x06,
            UART_8N2_NO_HANDSHAKE	= 0x01,
            UART_8E2_NO_HANDSHAKE	= 0x03,
            UART_8O2_NO_HANDSHAKE	= 0x05,
            UART_9N2_NO_HANDSHAKE	= 0x07,
            UART_8N1_HARD_HANDSHAKE	= 0x200,
            UART_8E1_HARD_HANDSHAKE = 0x202,
            UART_8O1_HARD_HANDSHAKE = 0x204,
            UART_9N1_HARD_HANDSHAKE = 0x206,
            UART_8N2_HARD_HANDSHAKE = 0x201,
            UART_8E2_HARD_HANDSHAKE = 0x203,
            UART_8O2_HARD_HANDSHAKE = 0x205,
            UART_9N2_HARD_HANDSHAKE = 0x207
        } uartModeType;
    #endif

    void uart2Config(uint32_t baudrate, uint32_t fcy, uartModeType uartMode);
    void uart2SendChar(unsigned char data);
    unsigned char uart2GetChar(void);
    int16_t uart2RxDataAvailable(void);
    int16_t uart2TxReady(void);
    void uart2TxIsrEnable(void);
    void uart2TxIsrDisable(void);
    void uart2RxIsrEnable(void);
    void uart2RxIsrDisable(void);

    /*
    void __attribute__((interrupt, auto_psv)) _U1TXInterrupt(void) {
        IFS0bits.U1TXIF = 0;
    }

    void __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void) {
        IFS0bits.U1RXIF = 0;
    }
    */

#endif	/* UART2_H */

