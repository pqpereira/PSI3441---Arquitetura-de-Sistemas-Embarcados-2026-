#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRIORITY 5
#define CAPACIDADE_VITRINE 10

int saldo_vitrine = 0;

K_MUTEX_DEFINE(mutex_vitrine);

K_SEM_DEFINE(sem_espacos_livres, CAPACIDADE_VITRINE, CAPACIDADE_VITRINE);
K_SEM_DEFINE(sem_paes_disponiveis, 0, CAPACIDADE_VITRINE);

void padeiro_thread(void)
{
    while (1) {
        k_sem_take(&sem_espacos_livres, K_FOREVER);

        k_mutex_lock(&mutex_vitrine, K_FOREVER);

        saldo_vitrine++;
        printk("Padeiro produziu 1 pao | Saldo da vitrine: %d\n", saldo_vitrine);

        k_mutex_unlock(&mutex_vitrine);

        k_sem_give(&sem_paes_disponiveis);

        k_msleep(1000);
    }
}

void cliente_thread(void)
{
    while (1) {
        k_sem_take(&sem_paes_disponiveis, K_FOREVER);

        k_mutex_lock(&mutex_vitrine, K_FOREVER);

        saldo_vitrine--;
        printk("Cliente retirou 1 pao | Saldo da vitrine: %d\n", saldo_vitrine);

        k_mutex_unlock(&mutex_vitrine);

        k_sem_give(&sem_espacos_livres);

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
    printk(" Atividade 7 - Parte 3\n");
    printk(" Utilizando Semaforos\n");
    printk("====================================\n\n");

    while (1) {
        k_msleep(1000);
    }

    return 0;
}