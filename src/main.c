#define F_CPU 16000000u

#define RX_BUFFER_LENGTH 128

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

void usart_init(uint32_t baud);
void usart_write();
void usart_read();

uint8_t *rx_buffer;
uint8_t rx_buffer_position;
uint8_t rx_buffer_read_position;
uint8_t rx_buffer_overflow = 0;

int main(void)
{
    rx_buffer = malloc(RX_BUFFER_LENGTH);
    rx_buffer_position = 0;
    rx_buffer_read_position = 0;
    rx_buffer_overflow = 0;

    sei();
    usart_init(9600);

    while(1);
}

void usart_init(uint32_t baud)
{
    baud = (F_CPU/(16*baud)) - 1;

    UBRR0H = (baud & 0x0F00) >> 8;
    UBRR0L = (baud & 0x00FF);

    UCSR0B = 0x98;
    //UCSR0C = 0x46;

}

void usart_write()
{
    while(rx_buffer_read_position < rx_buffer_position || rx_buffer_overflow)
    {
        while(!(UCSR0A & 0x20));
        UDR0 = rx_buffer[rx_buffer_read_position];

        if(rx_buffer_read_position == RX_BUFFER_LENGTH)
        {
            rx_buffer_overflow = 0;
            rx_buffer_read_position = 0;
        }
        else rx_buffer_read_position++;
    }
}

void usart_read()
{
    while(UCSR0A & 0x80)
    {
        rx_buffer[rx_buffer_position] = UDR0;

        if (rx_buffer_position == RX_BUFFER_LENGTH)
        {
            rx_buffer_overflow = 1;
            rx_buffer_position = 0;
        }
        else rx_buffer_position++;

        _delay_ms(5);
    }
}

ISR(USART_RX_vect)
{
    usart_read();
    usart_write();
}