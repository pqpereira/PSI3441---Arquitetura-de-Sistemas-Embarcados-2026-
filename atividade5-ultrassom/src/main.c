#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "MKL25Z4.h"

#define TRIG_PIN 12   // D3 = PTA12
#define ECHO_PIN 4    // D2 = PTD4

void gpio_init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

    PORTA->PCR[TRIG_PIN] = PORT_PCR_MUX(1);
    PORTD->PCR[ECHO_PIN] = PORT_PCR_MUX(1);

    PTA->PDDR |= (1 << TRIG_PIN);
    PTD->PDDR &= ~(1 << ECHO_PIN);

    PTA->PCOR = (1 << TRIG_PIN);
}

uint32_t medir_pulso_echo_us(void)
{
    uint32_t inicio, fim;
    uint32_t timeout;

    timeout = 1000000;

    while (!(PTD->PDIR & (1 << ECHO_PIN))) {
        if (--timeout == 0) {
            return 0;
        }
    }

    inicio = k_cycle_get_32();

    timeout = 1000000;

    while (PTD->PDIR & (1 << ECHO_PIN)) {
        if (--timeout == 0) {
            return 0;
        }
    }

    fim = k_cycle_get_32();

    uint32_t ciclos = fim - inicio;
    uint32_t freq = sys_clock_hw_cycles_per_sec();

    return (uint32_t)(((uint64_t)ciclos * 1000000ULL) / freq);
}

int medir_distancia_cm(void)
{
    uint32_t tempo_us;

    PTA->PCOR = (1 << TRIG_PIN);
    k_busy_wait(2);

    PTA->PSOR = (1 << TRIG_PIN);
    k_busy_wait(10);

    PTA->PCOR = (1 << TRIG_PIN);

    tempo_us = medir_pulso_echo_us();

    if (tempo_us == 0) {
        return -1;
    }

    return (int)(tempo_us / 58);
}

int main(void)
{
    int distancia;

    gpio_init();

    printk("\n====================================\n");
    printk(" Atividade 5 - Sensor Ultrassonico\n");
    printk("====================================\n");

    while (1)
    {
        distancia = medir_distancia_cm();

        if (distancia < 0) {
            printk("Falha na leitura do sensor\n");
        } else {
            printk("Distancia: %d cm\n", distancia);
        }

        k_msleep(500);
    }

    return 0;
}