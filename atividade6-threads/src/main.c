#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include <stdbool.h>
#include <stdint.h>
#include "MKL25Z4.h"

#define ADC_THREAD_STACK_SIZE     1024
#define ACCEL_THREAD_STACK_SIZE   1024

#define ADC_THREAD_PRIORITY       5
#define ACCEL_THREAD_PRIORITY     5

#define ADC_CHANNEL               8

#define MMA8451Q_ADDR             0x1D
#define MMA8451Q_STATUS           0x00
#define MMA8451Q_OUT_X_MSB        0x01
#define MMA8451Q_WHO_AM_I         0x0D
#define MMA8451Q_XYZ_DATA_CFG     0x0E
#define MMA8451Q_CTRL_REG1        0x2A

#define BUTTON_NODE DT_NODELABEL(user_button_0)

#if !DT_NODE_HAS_STATUS(BUTTON_NODE, okay)
#error "Botao user_button_0 nao definido no Device Tree"
#endif

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);
static struct gpio_callback button_cb_data;

static volatile bool modo_completo = false;
static volatile bool modo_alterado = false;

static const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

void adc_init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

    PORTB->PCR[0] = 0;

    ADC0->CFG1 = ADC_CFG1_MODE(1) | ADC_CFG1_ADIV(0) | ADC_CFG1_ADICLK(0);
    ADC0->SC2 = 0;
    ADC0->SC3 = 0;
}

uint16_t adc_read_value(void)
{
    ADC0->SC1[0] = ADC_CHANNEL;

    while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));

    return ADC0->R[0];
}

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    modo_completo = !modo_completo;
    modo_alterado = true;
}

void button_init(void)
{
    int ret;

    if (!gpio_is_ready_dt(&button)) {
        printk("Erro: botao nao esta pronto\n");
        return;
    }

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT | GPIO_PULL_UP);
    if (ret < 0) {
        printk("Erro ao configurar botao\n");
        return;
    }

    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_FALLING);
    if (ret < 0) {
        printk("Erro ao configurar interrupcao do botao\n");
        return;
    }

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);
}

void accel_init(void)
{
    uint8_t whoami;

    if (!device_is_ready(i2c_dev)) {
        printk("Erro: I2C nao esta pronto\n");
        return;
    }

    i2c_reg_read_byte(i2c_dev, MMA8451Q_ADDR, MMA8451Q_WHO_AM_I, &whoami);
    printk("MMA8451Q WHO_AM_I = 0x%02X\n", whoami);

    i2c_reg_write_byte(i2c_dev, MMA8451Q_ADDR, MMA8451Q_CTRL_REG1, 0x00);
    i2c_reg_write_byte(i2c_dev, MMA8451Q_ADDR, MMA8451Q_XYZ_DATA_CFG, 0x00);
    i2c_reg_write_byte(i2c_dev, MMA8451Q_ADDR, MMA8451Q_CTRL_REG1, 0x01);
}

void accel_read_xyz(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t data[6];

    if (i2c_burst_read(i2c_dev, MMA8451Q_ADDR, MMA8451Q_OUT_X_MSB, data, 6) == 0) {
        *x = ((int16_t)((data[0] << 8) | data[1])) >> 2;
        *y = ((int16_t)((data[2] << 8) | data[3])) >> 2;
        *z = ((int16_t)((data[4] << 8) | data[5])) >> 2;
    } else {
        *x = 0;
        *y = 0;
        *z = 0;
    }
}

void adc_thread(void)
{
    uint16_t adc_value;

    while (1) {
        if (modo_alterado) {
            modo_alterado = false;

            if (modo_completo) {
                printk("\nModo Completo: ADC + Acelerometro\n");
            } else {
                printk("\nModo ADC: apenas ADC\n");
            }
        }

        adc_value = adc_read_value();
        printk("ADC: %u\n", adc_value);

        k_msleep(500);
    }
}

void accel_thread(void)
{
    int16_t x, y, z;

    while (1) {
        if (modo_completo) {
            accel_read_xyz(&x, &y, &z);
            printk("Acelerometro -> X: %d | Y: %d | Z: %d\n", x, y, z);
        }

        k_msleep(1000);
    }
}

K_THREAD_DEFINE(adc_tid, ADC_THREAD_STACK_SIZE, adc_thread, NULL, NULL, NULL,
                ADC_THREAD_PRIORITY, 0, 0);

K_THREAD_DEFINE(accel_tid, ACCEL_THREAD_STACK_SIZE, accel_thread, NULL, NULL, NULL,
                ACCEL_THREAD_PRIORITY, 0, 0);

int main(void)
{
    printk("\n====================================\n");
    printk(" Atividade 6 - Threads\n");
    printk("====================================\n");

    adc_init();
    button_init();
    accel_init();

    printk("\nModo inicial: ADC\n");
    printk("Pressione o botao para alternar o modo\n\n");

    while (1) {
        k_msleep(1000);
    }

    return 0;
}