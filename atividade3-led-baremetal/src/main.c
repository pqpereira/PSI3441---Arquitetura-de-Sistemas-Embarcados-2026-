#include "MKL25Z4.h"

void delay(void)
{
    volatile uint32_t i;

    for(i = 0; i < 5000000; i++);
}

int main(void)
{
    // Habilita clock da PORTB
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

    // Configura PTB19 como GPIO
    PORTB->PCR[19] = PORT_PCR_MUX(1);

    // Configura PTB19 como saída
    PTB->PDDR |= (1 << 19);

    while (1)
    {
        // Liga LED verde (ativo em nível baixo)
        PTB->PCOR = (1 << 19);

        delay();

        // Desliga LED verde
        PTB->PSOR = (1 << 19);

        delay();
    }
}