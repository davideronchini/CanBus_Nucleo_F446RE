# CanBus Communication
**`Nucleo F446RE ⚙️`**

This repository contains the code for managing the CAN Bus protocol, reading sensors via ADC with DMA, and serial communication for an STM32 Nucleo-F446RE board. It is primarily used as a testing environment to accelerate the development of features that will later be implemented on a Nucleo-F756ZG board.

## Features
- ✅ **ADC with DMA**: Continuous reading from multiple sensors efficiently, without overloading the CPU.
- ✅ **CAN Bus Protocol**: Sending and receiving messages via CAN Bus, ideal for automotive or industrial automation applications.
- ✅ **Serial Communication**: Debugging and logging through the serial interface, with the ability to monitor the system's operation in real-time.

## 🔗 Project Structure

The project structure used allows for the separation of feature implementation contained in the `tasks` folder from the control logic of the components used. The `MCP2515` folder contains the code that enables proper management of the MCP2515 CAN bus transceiver, and the `Can` folder contains the logic for utilizing both transceivers used in the project (SN65HVD230 and MCP2515).
```
└── Core/
  ├── Inc/
  │ ├── MCP2515/
  │ ├── ..
  │ └── main.h
  ├── Src/
  │ ├── MCP2515/
  │ ├── Can/
  │ ├── Tasks/
  │ ├── Scheduler/
  │ ├── ..
  │ ├── stm32f4xx_it.c
  │ └── main.c
  └── Startup/
```
### Scheduler Implementation

The scheduler in this project is crucial for precise event timing, achieved through the use of the SysTick internal timer. The SysTick timer generates an interrupt every millisecond, which drives the timing mechanism for various tasks in the system.

#### Configuration Details

The SysTick timer is initialized to create a 1 ms interrupt using the following configuration. The logic for this setup is defined in the `Scheduler.c` file:

```c
void SchedulerInitFct(void)
{
    // Configure SysTick to generate an interrupt every 1 ms.
    // The calculation for the divider is as follows:
    // 128 MHz (HCLK frequency) / 4 (APB1 Prescaler) / 2 (APB2 Prescaler) * 10
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 160);

    // Initialize any other necessary variables and flags
    ...
}
```
This configuration ensures that the SysTick interrupt occurs at regular 1 ms intervals, essential for maintaining consistent timing across different system tasks.

### Interrupt Handling
The actual handling of the SysTick interrupt is defined in the `stm32f4xx_it.c` file. Within the `SysTick_Handler`, the `SchTimerInterruptCallback()` function is called to process timing-related tasks:
```c
void SysTick_Handler(void)
{
    /* USER CODE BEGIN SysTick_IRQn 0 */

    /* USER CODE END SysTick_IRQn 0 */
    HAL_IncTick();  // Increment the system tick

    /* USER CODE BEGIN SysTick_IRQn 1 */
    SchTimerInterruptCallback();  // Custom callback for the scheduler
    /* USER CODE END SysTick_IRQn 1 */
}
```
This implementation ensures that every time the SysTick interrupt is triggered, the system tick counter is incremented, and the scheduler's callback function is executed. This method is fundamental to the operation of time-dependent tasks within the system, ensuring they execute at the correct intervals.

## 🛠️ Commands & Installation

To get started with this project, follow these steps:

1. **Clone the repository**:
   ```bash git clone https://github.com/davideronchini/CanBus_Nucleo_F446RE.git ```
2. **Open the project with STM32CubeIDE**
3. **Compile and upload the code to the Nucleo F446RE**
4. **Set up your CAN Bus network and connect the board**
5. **Monitor serial communication for debugging and verification**
   
## Credits

Some of the functionalities used were learned through the following resources: (Insert the appropriate emoji next to each resource in the list: a video emoji for video content and a document emoji for PDFs or websites)

- **MCP2515**:  
   📄[MCP2515 Usage Guide](https://blog.naver.com/eziya76/221188525127)  
   🎞️[Video Tutorial](https://www.youtube.com/watch?v=sd6FKkfzU2I)

- **SN65HVD230**:  
   🎞️[Video Tutorial 1](https://youtu.be/KHNRftBa1Vc?si=2C673-Au-6wTgh2l)  
   🎞️[Video Tutorial 2](https://www.youtube.com/watch?v=-lcrrRrKdFg)

- **Potentiometer & MCP2551**:  
   🎞️[Usage Guide](https://www.micropeta.com/video115)

- **ADC**:  
   🎞️[Video Tutorial](https://www.youtube.com/watch?v=zipjCtiHYr8)  
   📄[In-depth Guide](https://deepbluembedded.com/stm32-adc-multi-channel-scan-continuous-mode-dma-poll-examples/#stm32-adc-multichannel-scan-continuous-mode-dma-example)


## 👀 Interested in Learning More?

If you have any questions, would like to discuss this project further, or are interested in potential collaboration opportunities, please feel free to connect with me through the following channels:

- LinkedIn: [davideronchini](www.linkedin.com/in/davideronchini)
- Email: [ronchinidavid3@gmail.com](mailto:ronchinidavid3@gmail.com)
