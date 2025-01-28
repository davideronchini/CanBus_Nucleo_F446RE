/* ***********************************************************************
 * Includes
 * ***********************************************************************/

#include "Tasks.h"

/* ***********************************************************************
 * Global Variables
 * ***********************************************************************/

uint32_t adc1_buf[ADC1_BUF_LEN]; // Buffer to store the raw ADC readings for each conversion

volatile uint16_t potentiometer = 0;
volatile uint16_t filtered_pot_signal = 0;


/* ***********************************************************************
 * General Tasks
 * ***********************************************************************/

void TaskInit(void)
{
	/* Executed once at startup (Functions for HW/SW initialization). */

	SchedulerInitFct();

	//START ADCs
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc1_buf, ADC1_BUF_LEN);
}

void TaskFast(void)
{
	/* Executed at medium scheduling intervals (e.g. every 1 ms). */

	AcquireSensorValues();
}

void TaskMed(void)
{
	/* Executed at medium scheduling intervals (e.g. every 10 ms). */

}

void TaskSlow(void)
{
	/* Executed at medium scheduling intervals (e.g. every 100 ms). */

}

/* ***********************************************************************
 * Specific Tasks
 * ***********************************************************************/

uint16_t AdaptiveEMA(uint16_t new_value) {
    static float filtered_value = 0.0f;

    // Calculate the relative change
    float delta = fabs(new_value - filtered_value);

    // Adapt ALPHA according to the variation
    float alpha = ALPHA_BASE + (ALPHA_ADAPT_FACTOR * delta / 4095.0f); // Assumes 12-bit ADC

    if (alpha > 1.0f) alpha = 1.0f; // Limit ALPHA

    // Calculate the filtered value
    filtered_value = alpha * new_value + (1 - alpha) * filtered_value;

    return (uint16_t)filtered_value;
}

/* ***********************************************************************
 * SENSOR DATA ACQUISITION
 * Acquires sensor values ​​at maximum speed and stores the readings to
 * allow averaging for each sensor based on the specified number of
 * readings.
 *
 * DMA IS UTILIZED FOR EFFICIENT ADC VALUE READING.
 *************************************************************************/
void AcquireSensorValues()
{
    // Read raw ADC values and accumulate sums for averaging
    // use: adc1_buf[]

	if (adc1_buf[0] >= APPs_ZERO_POSITION && adc1_buf[0]<=  APPs_FULL_POSITION){
		potentiometer = (uint16_t) ((((adc1_buf[0] - APPs_ZERO_POSITION) * (APPs_RANGE_SUP - APPs_RANGE_INF)) / (APPs_FULL_POSITION - APPs_ZERO_POSITION)) + APPs_RANGE_INF);
		filtered_pot_signal = (uint16_t)AdaptiveEMA(potentiometer);
	} else if (adc1_buf[0] < APPs_ZERO_POSITION){
		potentiometer = (uint16_t) APPs_RANGE_INF;
		filtered_pot_signal = (uint16_t) APPs_RANGE_INF;
	} else if (adc1_buf[0 > APPs_RANGE_SUP]) {
		potentiometer = (uint16_t) APPs_RANGE_SUP;
		filtered_pot_signal = (uint16_t) APPs_RANGE_SUP;
	}


	Display_Message(&huart2, "Potentiometer: %lu\nPotentiometer with filter: %lu\n\n", potentiometer, filtered_pot_signal);
}


void Display_Message(UART_HandleTypeDef *huart, const char *format, ...) {
    va_list args;
    va_start(args, format);

    // Calculate message length and allocate buffer dynamically
    size_t length = vsnprintf(NULL, 0, format, args) + 1;
    char *msg = (char *)malloc(length);
    if (!msg) { va_end(args); return; }

    // Format the message
    vsnprintf(msg, length, format, args);
    va_end(args);

    // Transmit the message and free the allocated memory
    HAL_UART_Transmit(huart, (uint8_t *)msg, length - 1, HAL_MAX_DELAY);
    free(msg);
}
