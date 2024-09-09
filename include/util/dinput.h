#include <Arduino.h>
#include <functional>
#define DEBOUNCETIME 50

class DInput_c
{
protected:
    uint8_t pin;
    static bool isFirstObj;        
    volatile unsigned long reading_time = 0;
    volatile uint8_t status_DIn = LOW;
    volatile uint8_t last_status_DIn = LOW;
    volatile uint16_t pressed_DIn_time = 0;
    std::function<void(uint8_t status)> onChanged = NULL; // void (*onChanged)(uint8_t status) = NULL;
    std::function<void()> onPressed = NULL;               // void (*onPressed)(void) = NULL;
    bool debounce();

public:
    DInput_c() { DInput_c(0); }
    DInput_c(uint8_t pinDIn);
    void onValueChanged(std::function<void(uint8_t status)> f /*void (*f)(uint8_t status)*/);
    void onPressedWithTime(std::function<void()> f /*void (*f)(void)*/);
    void setPin(uint8_t pinDIn);
    uint8_t getPin(void);
    uint8_t getStatus();
    void setTimeOnPressed(uint8_t time);
    friend inline void updateDInput(void *); 
    friend void IRAM_ATTR gpio_interrupt_handler(void *args);
};

bool DInput_c::isFirstObj = false;
xQueueHandle interputQueue = xQueueCreate(10, sizeof(DInput_c *));

inline void updateDInput(void *)
{
    DInput_c *dIn;
    if (xQueueReceive(interputQueue, dIn, 0))
    {
        dIn->debounce();
    }
}

DInput_c::DInput_c(uint8_t pinDIn)
{
    if (isFirstObj == false)
    {
        xTaskCreate(updateDInput, "Task Wave", 5000, NULL, 1, NULL);
        isFirstObj = true;
    }
    setPin(pinDIn);
}

void IRAM_ATTR gpio_interrupt_handler(void *args)
{
    xQueueSendFromISR(interputQueue, (DInput_c *)args, NULL);
}

bool DInput_c::debounce()
{
    uint8_t leitura = digitalRead(pin);                                                   // A variável leitura recebe a leitura do pino do botão: HIGH (pressionado) ou LOW (Desacionado)
    if ((leitura != status_DIn) && ((xTaskGetTickCount() - reading_time) > DEBOUNCETIME)) // Se o resultado de (tempo atual - tempoUltimoDebounce) for maior que o tempo que determinamos (tempoDebounce), ou seja, já passou os 50 milissegundos que é o tempo que o botão precisa ficar pressionado para ter a certeza de que ele realmente foi pressionado? Se sim faça:
    {
        reading_time = xTaskGetTickCount(); // Reseta a variável btnData.reading_time atribuindo o tempo atual para uma nova contagem
        if (leitura != status_DIn)
        {                         // Verifica se a leitura do botão mudou, ou seja, se é diferente do status que o botão tinha da última vez. Se sim, faça:
            status_DIn = leitura; // statusBotao recebe o que foi lido na variável leitura (pressionado = 1 e não pressionado = 0)
            return (true);        // Se valor do status foi alterado
        }
    }
    return (false);
}

uint8_t DInput_c::getPin(void)
{
    return pin;
}

void DInput_c::setPin(uint8_t pinDIn)
{
    gpio_isr_handler_remove((gpio_num_t)pin);
    pin = pinDIn;
    pinMode(pin, INPUT_PULLDOWN);
    gpio_install_isr_service(0);
    gpio_isr_handler_add((gpio_num_t)pinDIn, gpio_interrupt_handler, (void *)this);
}

void DInput_c::setTimeOnPressed(uint8_t time) // Tempo em segundos
{
    pressed_DIn_time = ((uint16_t)time) * 1000;
}

uint8_t DInput_c::getStatus()
{
    return status_DIn;
}

void DInput_c::onValueChanged(std::function<void(uint8_t status)> f)
{
    onChanged = f;
}

void DInput_c::onPressedWithTime(std::function<void()> f)
{
    onPressed = f;
}

// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/queue.h"
// #include "driver/gpio.h"

// #define INPUT_PIN 15
// #define LED_PIN 2

// int state = 0;
// xQueueHandle interputQueue;

// static void IRAM_ATTR gpio_interrupt_handler(void *args)
// {
//     int pinNumber = (int)args;
//     xQueueSendFromISR(interputQueue, &pinNumber, NULL);
// }

// void LED_Control_Task(void *params)
// {
//     int pinNumber, count = 0;
//     while (true)
//     {
//         if (xQueueReceive(interputQueue, &pinNumber, portMAX_DELAY))
//         {
//             printf("GPIO %d was pressed %d times. The state is %d\n", pinNumber, count++, gpio_get_level(INPUT_PIN));
//             gpio_set_level(LED_PIN, gpio_get_level(INPUT_PIN));
//         }
//     }
// }

// void app_main()
// {
//     gpio_pad_select_gpio(LED_PIN);
//     gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

//     gpio_pad_select_gpio(INPUT_PIN);
//     gpio_set_direction(INPUT_PIN, GPIO_MODE_INPUT);
//     gpio_pulldown_en(INPUT_PIN);
//     gpio_pullup_dis(INPUT_PIN);
//     gpio_set_intr_type(INPUT_PIN, GPIO_INTR_POSEDGE);

//     interputQueue = xQueueCreate(10, sizeof(int));
//     xTaskCreate(LED_Control_Task, "LED_Control_Task", 2048, NULL, 1, NULL);

//     gpio_install_isr_service(0);
//     gpio_isr_handler_add(INPUT_PIN, gpio_interrupt_handler, (void *)INPUT_PIN);
// }