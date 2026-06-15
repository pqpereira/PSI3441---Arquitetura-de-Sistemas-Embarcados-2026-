#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRIORITY 5

volatile int saldo_vitrine = 0;

void padeiro_thread(void)
{
    while (1) {
        saldo_vitrine++;
        printk("Padeiro produziu 1 pao | Saldo da vitrine: %d\n", saldo_vitrine);
        k_msleep(1000);
    }
}

void cliente_thread(void)
{
    while (1) {
        saldo_vitrine--;
        printk("Cliente retirou 1 pao | Saldo da vitrine: %d\n", saldo_vitrine);
        k_msleep(1500);
    }
}

K_THREAD_DEFINE(padeiro_id, STACK_SIZE, padeiro_thread, NULL, NULL, NULL,
                PRIORITY, 0, 0);

K_THREAD_DEFINE(cliente_id, STACK_SIZE, cliente_thread, NULL, NULL, NULL,
                PRIORITY, 0, 0);

int main(void)
{
    printk("\n====================================\n");
    printk(" Atividade 7 - Parte 1\n");
    printk(" Sem sincronizacao\n");
    printk("====================================\n\n");

    while (1) {
        k_msleep(1000);
    }

    return 0;
}