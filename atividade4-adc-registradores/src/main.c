#include "MKL25Z4.h"

#define LED_GREEN_PIN 19
#define LED_BLUE_PIN  1

#define ADC_CHANNEL   8

#define ADC_LOW_THRESHOLD   500
#define ADC_HIGH_THRESHOLD  3500

void delay(void)
{
    volatile uint32_t i;
    for (i = 0; i < 50000; i++);
}

void leds_init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

    PORTB->PCR[LED_GREEN_PIN] = PORT_PCR_MUX(1);
    PORTD->PCR[LED_BLUE_PIN]  = PORT_PCR_MUX(1);

    PTB->PDDR |= (1 << LED_GREEN_PIN);
    PTD->PDDR |= (1 << LED_BLUE_PIN);

    PTB->PSOR = (1 << LED_GREEN_PIN);
    PTD->PSOR = (1 << LED_BLUE_PIN);
}

void adc_init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

    PORTB->PCR[0] = 0;

    ADC0->CFG1 = ADC_CFG1_MODE(1) | ADC_CFG1_ADIV(0) | ADC_CFG1_ADICLK(0);
    ADC0->SC2 = 0;
    ADC0->SC3 = 0;
}

uint16_t adc_read(void)
{
    ADC0->SC1[0] = ADC_CHANNEL;

    while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));

    return ADC0->R[0];
}

int main(void)
{
    uint16_t adc_value;

    leds_init();
    adc_init();

    while (1)
    {
        adc_value = adc_read();

        if (adc_value < ADC_LOW_THRESHOLD)
        {
            PTB->PCOR = (1 << LED_GREEN_PIN);
            PTD->PSOR = (1 << LED_BLUE_PIN);
        }
        else if (adc_value > ADC_HIGH_THRESHOLD)
        {
            PTB->PSOR = (1 << LED_GREEN_PIN);
            PTD->PCOR = (1 << LED_BLUE_PIN);
        }
        else
        {
            PTB->PSOR = (1 << LED_GREEN_PIN);
            PTD->PSOR = (1 << LED_BLUE_PIN);
        }

        delay();
    }
}