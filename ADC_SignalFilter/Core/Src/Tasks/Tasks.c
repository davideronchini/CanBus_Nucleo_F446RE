/* ***********************************************************************
 * Includes
 * ***********************************************************************/

#include "Tasks.h"

/* ***********************************************************************
 * Global Variables
 * ***********************************************************************/

uint32_t adc1_buf[ADC1_BUF_LEN]; // Buffer to store the raw ADC readings for each conversion

volatile uint16_t APPs_val = 0;
volatile uint16_t APPs_flt = 0;

volatile uint8_t APPs_counter = 0;
volatile uint32_t APPs_sum = 0, APPs_avg = 0;

volatile int8_t STEERING_val = 0;

volatile int16_t rpm_left = 0;
volatile int16_t rpm_right = 0;


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
	APPs_avg = (uint16_t) ((float)APPs_sum / (float) APPs_counter);
	APPs_counter = 0;
	APPs_sum = 0;
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

/**
 * @brief Applies an Exponential Moving Average (EMA) filter.
 *
 * This function computes a filtered value from a new sensor reading using the
 * formula:
 *    EMA = ALPHA * new_value + (1 - ALPHA) * previous_ema
 *
 * A static variable 'ema' (of type float) is used to hold the internal state.
 * It is initialized with a sentinel value (-1) to indicate that the filter is
 * not yet initialized. Since sensor readings (uint16_t) are always non-negative,
 * the first call initializes the filter with the actual sensor value.
 *
 * @param new_value The new sensor reading (uint16_t).
 * @return The updated filtered value (uint16_t), rounded to the nearest integer.
 */
uint16_t EMA_Filter(uint16_t new_value) {
    static float ema = -1.0f;  // Sentinel: negative value indicates uninitialized state.

    if (ema < 0) {
        // Initialize the EMA with the first sensor reading.
        ema = (float)new_value;
    } else {
        // Update the EMA using the formula.
        ema = ALPHA * (float)new_value + (1.0f - ALPHA) * ema;
    }

    // Return the filtered value rounded to the nearest integer.
    return (uint16_t)(ema + 0.5f);
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

	if (adc1_buf[0] >= APPs_ZERO_POSITION && adc1_buf[0]<=  APPs_FULL_POSITION) {
		APPs_val = (uint16_t) ((((adc1_buf[0] - APPs_ZERO_POSITION) * (APPs_RANGE_SUP - APPs_RANGE_INF)) / (APPs_FULL_POSITION - APPs_ZERO_POSITION)) + APPs_RANGE_INF);
		APPs_flt = (uint16_t)EMA_Filter(APPs_val);
	} else if (adc1_buf[0] < APPs_ZERO_POSITION) {
		APPs_val = (uint16_t) APPs_RANGE_INF;
		APPs_flt = (uint16_t) APPs_RANGE_INF;
	} else if (adc1_buf[0 > APPs_RANGE_SUP]) {
		APPs_val = (uint16_t) APPs_RANGE_SUP;
		APPs_flt = (uint16_t) APPs_RANGE_SUP;
	}

	APPs_sum += APPs_val;
	APPs_counter++;

	STEERING_val = (int8_t)((((adc1_buf[1] - STEERING_ZERO_POSITION) * (STEERING_RANGE_SUP - STEERING_RANGE_INF)) / (STEERING_FULL_POSITION - STEERING_ZERO_POSITION)) + STEERING_RANGE_INF);

	//Calculate_Motor_RPMs(STEERING_val, APPs_flt, &rpm_left, &rpm_right);

	Display_Message(&huart2, "APPs: %lu\nAPPs with Filter: %lu\nSTEERING: %d\n\n", APPs_val, APPs_flt, STEERING_val);
}

/**
 * @brief Calculates the RPM for the left and right motors based on the steering angle and throttle input.
 *
 * This function computes the motor RPMs required to simulate a differential drive system with independent motors
 * for the left and right wheels. The calculation accounts for the steering angle and throttle input to adjust
 * the motor speeds accordingly.
 *
 * The process involves:
 * - Converting the steering wheel angle to the corresponding wheel angle using the steering ratio.
 * - Using the steering angle to determine the turning radius and adjusting it to ensure it does not fall below a
 *   predefined minimum radius.
 * - Calculating scale factors for the left and right wheels based on their position relative to the turning radius.
 * - Adjusting the RPM for each motor by scaling the base RPM based on the calculated scale factors, ensuring the
 *   wheel on the inside of the turn slows down, while the outside wheel speeds up.
 * - Ensuring that both motor RPMs remain within the allowed limits, including the base RPM limit and the maximum RPM.
 *
 * The goal of this function is to simulate realistic motor behavior in a vehicle with independent motors for each wheel,
 * with the motor RPMs dynamically adjusted to match the steering input and throttle. This is important for providing
 * precise control over the vehicle's movement.
 *
 * @param steering_wheel_angle The steering wheel angle in degrees (-90 to 90).
 * @param base_rpm The base RPM from the throttle potentiometer.
 * @param rpm_left Pointer to store the calculated left motor RPM.
 * @param rpm_right Pointer to store the calculated right motor RPM.
 */

void Calculate_Motor_RPMs(int8_t steering_wheel_angle, uint16_t base_rpm, int16_t *rpm_left, int16_t *rpm_right) {
    // Convert steering wheel angle to wheel angle
    float steering_angle = (float)steering_wheel_angle / STEERING_RATIO;

    // If steering angle is 0, both motors receive the same RPM
    if (steering_angle == 0.0f) {
        *rpm_left = base_rpm;
        *rpm_right = base_rpm;
        return;
    }

    // Convert degrees to radians
    float steering_rad = steering_angle * (M_PI / 180.0f);

    // Compute turning radius
    float radius = WHEELBASE / fabsf(tanf(steering_rad));

    // Ensure radius does not go below the minimum allowed
    if (radius < MIN_RADIUS) {
        radius = MIN_RADIUS; // aumentando il raggio ...
    }

    // Compute scaling factors
    float scale_left  = (radius + (DRIVEN_WHEEL_DISTANCE / 2.0f)) / radius;
    float scale_right = (radius - (DRIVEN_WHEEL_DISTANCE / 2.0f)) / radius;

    // Ensure the correct wheel slows down
    if (steering_angle > 0) { // Sterzata a destra → ruota destra più lenta
        *rpm_left  = (uint16_t)(base_rpm * scale_left);
        *rpm_right = (uint16_t)(base_rpm * scale_right);
    } else { // Sterzata a sinistra → ruota sinistra più lenta
        *rpm_left  = (uint16_t)(base_rpm * scale_right);
        *rpm_right = (uint16_t)(base_rpm * scale_left);
    }

    // Ensure RPMs do not exceed limits
	if (*rpm_left > MAX_RPM) *rpm_left = MAX_RPM;
	if (*rpm_right > MAX_RPM) *rpm_right = MAX_RPM;
	if (*rpm_left < 0) *rpm_left = 0;
	if (*rpm_right < 0) *rpm_right = 0;

	// Check if it's necessary to also increase the speed of the outer wheel.
	// The current logic only reduces the speed of the inner wheel during turns.
	// If the goal is to increase the speed of the inner wheel, the current code
	// should be modified. Specifically, after reaching the maximum RPM, the
	// speed reduction should be less significant, as the outer wheel would
	// already be at its maximum RPM, allowing for a smaller reduction in the
	// inner wheel's speed.
	if (*rpm_left > base_rpm) *rpm_left = base_rpm;
	if (*rpm_right > base_rpm) *rpm_right = base_rpm;
}

void Display_Message(UART_HandleTypeDef *huart, const char *format, ...)
{
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
