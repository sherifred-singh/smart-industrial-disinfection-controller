/**
 * @file hal_gpio_stm32.c
 * @brief STM32F4xx Hardware Implementation for Digital GPIO
 * @author Sherifred Singh
 * 
 * Maps logical GPIO pins to STM32 Port & Pin configurations for STM32 Nucleo-F401RE / F411RE.
 */

#ifdef TARGET_STM32F4

#include "../../hal/hal_gpio.h"
#include "stm32f4xx_hal.h"

typedef struct {
    GPIO_TypeDef *port;
    uint16_t      pin;
    uint32_t      mode;
    uint32_t      pull;
    uint32_t      active_level; /* 1 = Active High, 0 = Active Low */
} stm32_gpio_map_t;

static const stm32_gpio_map_t s_gpio_map[HAL_GPIO_PIN_COUNT] = {
    [HAL_GPIO_IN_START_BTN]        = { GPIOC, GPIO_PIN_13, GPIO_MODE_INPUT, GPIO_PULLUP,   0 }, /* B1 User button */
    [HAL_GPIO_IN_STOP_BTN]         = { GPIOB, GPIO_PIN_0,  GPIO_MODE_INPUT, GPIO_PULLUP,   0 },
    [HAL_GPIO_IN_ESTOP_NC]         = { GPIOB, GPIO_PIN_1,  GPIO_MODE_INPUT, GPIO_PULLUP,   1 }, /* 1=Normal, 0=Tripped */
    [HAL_GPIO_IN_DOOR_INTERLOCK]   = { GPIOB, GPIO_PIN_2,  GPIO_MODE_INPUT, GPIO_PULLUP,   1 }, /* 1=Closed, 0=Open */
    [HAL_GPIO_IN_SERVICE_KEY]      = { GPIOB, GPIO_PIN_10, GPIO_MODE_INPUT, GPIO_PULLDOWN, 1 },
    [HAL_GPIO_IN_AIRFLOW_SW]       = { GPIOB, GPIO_PIN_11, GPIO_MODE_INPUT, GPIO_PULLUP,   1 },

    [HAL_GPIO_OUT_DISINFECT_RELAY] = { GPIOA, GPIO_PIN_8,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 1 },
    [HAL_GPIO_OUT_EXHAUST_FAN]     = { GPIOA, GPIO_PIN_9,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 1 },
    [HAL_GPIO_OUT_DOOR_LOCK]       = { GPIOA, GPIO_PIN_10, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 1 },
    [HAL_GPIO_OUT_STATUS_LED_GRN]  = { GPIOA, GPIO_PIN_5,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 1 }, /* LD2 Nucleo */
    [HAL_GPIO_OUT_STATUS_LED_YEL]  = { GPIOA, GPIO_PIN_6,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 1 },
    [HAL_GPIO_OUT_STATUS_LED_RED]  = { GPIOA, GPIO_PIN_7,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 1 },
    [HAL_GPIO_OUT_BUZZER]          = { GPIOB, GPIO_PIN_6,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 1 },
    [HAL_GPIO_OUT_RS485_DE]        = { GPIOA, GPIO_PIN_1,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 1 },
};

void hal_gpio_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef init = {0};

    for (int i = 0; i < HAL_GPIO_PIN_COUNT; ++i) {
        init.Pin   = s_gpio_map[i].pin;
        init.Mode  = s_gpio_map[i].mode;
        init.Pull  = s_gpio_map[i].pull;
        init.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(s_gpio_map[i].port, &init);

        if (s_gpio_map[i].mode == GPIO_MODE_OUTPUT_PP) {
            /* De-energize outputs upon power-up */
            HAL_GPIO_WritePin(s_gpio_map[i].port, s_gpio_map[i].pin, GPIO_PIN_RESET);
        }
    }
}

hal_gpio_level_t hal_gpio_read(hal_gpio_pin_t pin)
{
    if (pin < HAL_GPIO_PIN_COUNT) {
        GPIO_PinState state = HAL_GPIO_ReadPin(s_gpio_map[pin].port, s_gpio_map[pin].pin);
        return (state == GPIO_PIN_SET) ? HAL_GPIO_LEVEL_HIGH : HAL_GPIO_LEVEL_LOW;
    }
    return HAL_GPIO_LEVEL_LOW;
}

void hal_gpio_write(hal_gpio_pin_t pin, hal_gpio_level_t level)
{
    if (pin < HAL_GPIO_PIN_COUNT) {
        GPIO_PinState state = (level == HAL_GPIO_LEVEL_HIGH) ? GPIO_PIN_SET : GPIO_PIN_RESET;
        HAL_GPIO_WritePin(s_gpio_map[pin].port, s_gpio_map[pin].pin, state);
    }
}

void hal_gpio_toggle(hal_gpio_pin_t pin)
{
    if (pin < HAL_GPIO_PIN_COUNT) {
        HAL_GPIO_TogglePin(s_gpio_map[pin].port, s_gpio_map[pin].pin);
    }
}

#endif /* TARGET_STM32F4 */
