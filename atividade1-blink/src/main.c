#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#define SLEEP_TIME_MS 1000

// Define o LED usando Device Tree
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)

// Verifica se o LED está definido no Device Tree
#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
#else
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

#if DT_NODE_HAS_STATUS(LED1_NODE, okay)
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
#else
#error "Unsupported board: led1 devicetree alias is not defined"
#endif

#if DT_NODE_HAS_STATUS(LED2_NODE, okay)
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
#else
#error "Unsupported board: led2 devicetree alias is not defined"
#endif

//CRIANDO OS NOMES DOS ESTADOS PARA MINHA MAQUINA DE ESTADOS
typedef enum{
    ESTADO_VERDE,
    ESTADO_AMARELO,
    ESTADO_VERMELHO
} estado_semaforo_t;


void main(void){
    int ret;

    // Verifica se o device está pronto
    if (!gpio_is_ready_dt(&led0)) {
        printk("Error: LED device %s is not ready\n", led0.port->name);
        return;
    }

    if (!gpio_is_ready_dt(&led1)) {
        printk("Error: LED device %s is not ready\n", led1.port->name);
        return;
    }

    if (!gpio_is_ready_dt(&led2)) {
        printk("Error: LED device %s is not ready\n", led2.port->name);
        return;
    }

    // Configura o pino como saída
    ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        printk("Error %d: failed to configure LED pin\n", ret);
        return;
    }

    ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        printk("Error %d: failed to configure LED pin\n", ret);
        return;
    }

    ret = gpio_pin_configure_dt(&led2, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        printk("Error %d: failed to configure LED pin\n", ret);
        return;
    }

    printk("LED blinking on %s pin %d\n", led0.port->name, led0.pin);
    printk("LED blinking on %s pin %d\n", led1.port->name, led1.pin);
    printk("LED blinking on %s pin %d\n", led2.port->name, led2.pin);

    estado_semaforo_t estado = ESTADO_VERDE;   // o semaforo comeca no estado verde

    while (1) {
        switch (estado){
            
            case ESTADO_VERDE:
                gpio_pin_set_dt(&led0, 1);  // verde
                gpio_pin_set_dt(&led1, 0);  // azul
                gpio_pin_set_dt(&led2, 0);  // vermelho
                k_msleep(SLEEP_TIME_MS);
                
                estado = ESTADO_AMARELO;
                
                break;

            case ESTADO_AMARELO:
                gpio_pin_set_dt(&led0, 1);
                gpio_pin_set_dt(&led1, 0);
                gpio_pin_set_dt(&led2, 1);
                k_msleep(SLEEP_TIME_MS);
                
                estado = ESTADO_VERMELHO;

                break;

            case ESTADO_VERMELHO:
                gpio_pin_set_dt(&led0, 0);
                gpio_pin_set_dt(&led1, 0);
                gpio_pin_set_dt(&led2, 1);
                k_msleep(SLEEP_TIME_MS);

                estado = ESTADO_VERDE;

                break;
        }

    }
}