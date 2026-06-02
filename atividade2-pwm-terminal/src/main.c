#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <zephyr/console/console.h>

#include <pwm_z42.h>

#define TPM_MODULE 1000
#define RED_RATIO 100
#define GREEN_RATIO 35

static uint16_t duty_active_low(uint8_t intensidade, uint8_t proporcao)
{
    uint32_t duty_on;

    if (intensidade > 100) {
        intensidade = 100;
    }

    duty_on = (TPM_MODULE * intensidade * proporcao) / 10000;

    return TPM_MODULE - duty_on;
}

static void aplicar_intensidade(int intensidade)
{
    if (intensidade < 0) {
        intensidade = 0;
    }

    if (intensidade > 100) {
        intensidade = 100;
    }

    uint16_t duty_red   = duty_active_low((uint8_t)intensidade, RED_RATIO);
    uint16_t duty_green = duty_active_low((uint8_t)intensidade, GREEN_RATIO);

    pwm_tpm_CnV(TPM2, 0, duty_red);
    pwm_tpm_CnV(TPM2, 1, duty_green);

    printk("\nIntensidade aplicada: %d%% | Vermelho duty: %d | Verde duty: %d\n",
           intensidade, duty_red, duty_green);

    printk("\nDigite intensidade (0 a 100): ");
}

int main(void)
{
    printk("\n====================================\n");
    printk(" Controle PWM - LED Laranja\n");
    printk("====================================\n");

    pwm_tpm_Init(TPM2, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM);

    pwm_tpm_Ch_Init(TPM2, 0, TPM_PWM_H, GPIOB, 18);
    pwm_tpm_Ch_Init(TPM2, 1, TPM_PWM_H, GPIOB, 19);

    printk("\nDigite intensidade (0 a 100): ");

    while (1) {
        uint8_t *line = console_getline();
        if (line) {
            aplicar_intensidade(atoi(line));
        }
    }

    return 0;
}