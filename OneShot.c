#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

#define LED_VERMELHO 13
#define LED_AMARELO 12
#define LED_VERDE 11
#define PUSH_BUTTON 5

int last_time = 0;
bool aceso = false;

void init_gpios()
{
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);

    gpio_init(LED_AMARELO);
    gpio_set_dir(LED_AMARELO, GPIO_OUT);

    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);

    gpio_init(PUSH_BUTTON);
    gpio_set_dir(PUSH_BUTTON, GPIO_IN);
    gpio_pull_up(PUSH_BUTTON);
}

int64_t turn_off_callback(alarm_id_t id, void *user_data)
{
    if (gpio_get(LED_VERMELHO) == 1)
    {
        gpio_put(LED_VERMELHO, 0);
    }
    else if (gpio_get(LED_AMARELO) == 1)
    {
        gpio_put(LED_AMARELO, 0);
        aceso = true;
    }
    else if (gpio_get(LED_VERDE) == 1)
    {
        gpio_put(LED_VERDE, 0);
        aceso = true;
    }
    return 0;
}

void button_irq_handler(uint gpio, uint32_t events)
{
    // Implementação de um debounce de 200ms por software
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    if ((current_time - last_time) > 200000 && gpio_get(LED_VERDE) == 0)
    {
        last_time = current_time;
        gpio_put(LED_VERMELHO, 1);
        gpio_put(LED_AMARELO, 1);
        gpio_put(LED_VERDE, 1);
        aceso = true;
        add_alarm_in_ms(3000, turn_off_callback, NULL, false);

    }
}

// Função para configurar a interrupção dos botões
void set_irq()
{
    // Ativa interrupção para o botão 'a' com uma rotina
    gpio_set_irq_enabled_with_callback(PUSH_BUTTON, GPIO_IRQ_EDGE_FALL, true, &button_irq_handler);
}

int main()
{
    stdio_init_all();
    init_gpios();
    set_irq();

    while (true)
    {
        if ((aceso == true && gpio_get(LED_VERDE) == 1 && gpio_get(LED_AMARELO) == 1 && gpio_get(LED_VERMELHO) == 0) || (aceso == true && gpio_get(LED_VERDE) == 1 && gpio_get(LED_VERMELHO) == 0))
        {
            add_alarm_in_ms(3000, turn_off_callback, NULL, false);
            aceso = false;
        }
        else
        {
            sleep_ms(1);
        }
    }
}
