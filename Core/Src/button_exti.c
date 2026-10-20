#include "button_exti.h"
#include "fsm.h"

/* Lọc chống dội trước khi báo nút nhấn cho FSM. */
typedef struct
{
  volatile uint8_t edgePending;
  volatile uint8_t stablePressed;
  uint8_t lowSamples;
  uint8_t highSamples;
} ButtonDebounceState_t;

static ButtonDebounceState_t pedestrianButton;
static ButtonDebounceState_t nightButton;

static void Button_StartDebounce(ButtonDebounceState_t *button)
{
  if ((button->stablePressed == 0U) && (button->edgePending == 0U))
  {
    button->lowSamples = 0U;
    button->highSamples = 0U;
    button->edgePending = 1U;
  }
}

static uint8_t Button_DebounceOne(ButtonDebounceState_t *button,
                                  GPIO_TypeDef *port,
                                  uint16_t pin)
{
  GPIO_PinState pinState = HAL_GPIO_ReadPin(port, pin);

  if (button->edgePending != 0U)
  {
    if (pinState == GPIO_PIN_RESET)
    {
      button->highSamples = 0U;
      if (button->lowSamples < BUTTON_DEBOUNCE_TIME_MS)
      {
        button->lowSamples++;
      }

      if (button->lowSamples >= BUTTON_DEBOUNCE_TIME_MS)
      {
        button->stablePressed = 1U;
        button->edgePending = 0U;
        button->lowSamples = 0U;
        return 1U;
      }
    }
    else
    {
      /* A short low pulse was contact bounce or electrical noise. */
      button->lowSamples = 0U;
      if (button->highSamples < BUTTON_DEBOUNCE_TIME_MS)
      {
        button->highSamples++;
      }
      if (button->highSamples >= BUTTON_DEBOUNCE_TIME_MS)
      {
        button->edgePending = 0U;
        button->highSamples = 0U;
      }
    }
  }
  else if (button->stablePressed != 0U)
  {
    if (pinState == GPIO_PIN_SET)
    {
      if (button->highSamples < BUTTON_DEBOUNCE_TIME_MS)
      {
        button->highSamples++;
      }
      if (button->highSamples >= BUTTON_DEBOUNCE_TIME_MS)
      {
        button->stablePressed = 0U;
        button->highSamples = 0U;
        /* Recover a new press whose EXTI edge was ignored before the
         * stable-release state was cleared. */
        if (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET)
        {
          Button_StartDebounce(button);
        }
      }
    }
    else
    {
      button->highSamples = 0U;
    }
  }

  return 0U;
}

void Button_Init(void)
{
  pedestrianButton.edgePending = 0U;
  pedestrianButton.stablePressed = 0U;
  pedestrianButton.lowSamples = 0U;
  pedestrianButton.highSamples = 0U;

  nightButton.edgePending = 0U;
  nightButton.stablePressed = 0U;
  nightButton.lowSamples = 0U;
  nightButton.highSamples = 0U;

  /* MX_GPIO_Init() owns the GPIO/EXTI/NVIC configuration. */
  __HAL_GPIO_EXTI_CLEAR_IT(BUTTON_PEDESTRIAN_PIN);
  __HAL_GPIO_EXTI_CLEAR_IT(BUTTON_NIGHT_PIN);
  HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
}

void Button_DebounceTask1ms(void)
{
  if (Button_DebounceOne(&pedestrianButton,
                         BUTTON_PEDESTRIAN_PORT,
                         BUTTON_PEDESTRIAN_PIN) != 0U)
  {
    FSM_TriggerPedestrian();
  }

  if (Button_DebounceOne(&nightButton,
                         BUTTON_NIGHT_PORT,
                         BUTTON_NIGHT_PIN) != 0U)
  {
    if (currentState == STATE_NIGHT_MODE)
    {
      FSM_SetAutoMode();
    }
    else
    {
      FSM_SetNightMode();
    }
  }
}

void Button_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == BUTTON_PEDESTRIAN_PIN)
  {
    Button_StartDebounce(&pedestrianButton);
  }
  else if (GPIO_Pin == BUTTON_NIGHT_PIN)
  {
    Button_StartDebounce(&nightButton);
  }
}

/* HAL calls this weak hook from HAL_GPIO_EXTI_IRQHandler(). Forward each
 * hardware EXTI event to the button debounce state machine above. */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  Button_GPIO_EXTI_Callback(GPIO_Pin);
}
