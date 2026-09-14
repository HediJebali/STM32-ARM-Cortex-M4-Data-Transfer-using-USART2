# Project : STM32 ARM Cortex-M4 Data Transfer using USART2 

This project sets up a bare-metal, interrupt-driven USART2 communication interface on an STM32F446RE microcontroller to handle bidirectional text-based data transfer with a PC serial terminal (HTerm) and control onboard/external LEDs based on command validation:

* **Transferred Data Payload:** ASCII text strings and structured command packets exchanged via serial communication (e.g., greetings, user prompts, status responses, and LED control instructions).
* **Communication Link:** USART2 peripheral routed through the ST-Link Virtual COM Port (VCP) at **115200 Baud**, using **8 Data Bits**, **Odd Parity**, and **1 Stop Bit**.
* **Source & Destination:** Sent directly from the PC via HTerm to `RxBuffer` in SRAM, and transmitted back from FLASH/SRAM constants to HTerm over **USART2 (PA2 TX / PA3 RX)**.
* **Hardware Signaling:** Drives **GPIOA Pin 5** (Green LED) and **GPIOA Pin 6** (Red LED) HIGH or LOW dynamically based on valid textual commands (`"turn on green led"`, `"turn off red led"`, etc.).

---

##  Objective

* **Interrupt-Driven Data Processing:** Use RXNE (Receive Data Register Not Empty) and PE (Parity Error) interrupts to process serial characters in real-time without polling blocking loops.
* **Bare-Metal Register Configuration:** Manually configure USART2 and GPIO Registers (`CR1`, `CR2`, `CR3`, `BRR`, `SR`, `DR`, `MODER`, `AFR`) using raw register masks and CMSIS definitions.
* **Finite State Machine (FSM) Design:** Implement a robust state-driven application logic (`IDLE` ➔ `WAIT_FOR_RESPONCE` ➔ `ASK_FOR_NAME` ➔ `WAIT_FOR_NAME` ➔ `WAIT_FOR_COMMAND`) to structure terminal interactive flow.
* **Parity Error Handling:** Detect transmission errors via hardware flags (`USART_SR_PE`) inside `USART2_IRQ_Callback()` and send diagnostic status messages to the host.

---

##  Project Overview

This project sets up a register-based, state-driven USART communication pipeline to manage serial text interaction between an STM32 micro-controller and a PC terminal.

1. Initializes **USART2** on **PA2 (TX)** and **PA3 (RX)** at **115200 Baud** using the internal **16 MHz HSI** clock source (`BRR = 0x8B`).
2. Transmits an initial `"Hello World!"` string and advances the state machine to wait for host input terminated by a NULL character (`\0`).
3. Captures incoming characters asynchronously inside the USART interrupt handler and reconstructs the full message inside `RxBuffer`.
4. Executes interactive terminal flows, requesting the user's name and responding with dynamic greetings and command instructions.
5. Parses incoming execution commands to control **PA5** (Green LED) and **PA6** (Red LED), returning `"Done"` upon successful execution or `"Wrong command"` on mismatched inputs.

---

##  Tech Stack & Hardware

* **Microcontroller:** STM32F446RE (ARM Cortex-M4 with FPU)
* **Development Board:** NUCLEO-F446RE
* **Clock Configuration:** 16 MHz HSI (Internal High-Speed RC Oscillator)
* **Peripheral Units:** USART2, GPIOA (PA2, PA3, PA5, PA6), NVIC, RCC
* **Software Terminal:** HTerm 
* **IDE & Toolchain:** Keil uVision (Bare-Metal C)
* **Debugging Hardware:** Onboard ST-Link V2-1 (Virtual COM Port)

---

##  USART & Baud Rate Configuration Details

| Parameter | Configuration Value | Register Setting / Description |
| :--- | :--- | :--- |
| **USART Peripheral** | USART2 | Connected to APB1 Peripheral Bus |
| **Baud Rate** | 115200 bps | `BRR = 0x8B` ($f_{CK} = 16\text{ MHz}$, $\text{USARTDIV} = 8.68$) |
| **Word Length** | 9 Bits (8 Data + 1 Parity) | `M = 1` in `USART2_CR1` |
| **Parity Control** | Enabled (Odd Parity) | `PCE = 1`, `PS = 1` in `USART2_CR1` |
| **Stop Bits** | 1 Stop Bit | `STOP = 00b` in `USART2_CR2` |
| **Oversampling** | By 16 | `OVER8 = 0` in `USART2_CR1` |
| **Interrupts Enabled**| RXNE & Parity Error | `RXNEIE = 1`, `PEIE = 1` in `USART2_CR1` mapped to `USART2_IRQn` |

---

##  Debugging & Logic Analysis

* **Zero Polling Stall on RX:** The CPU stays responsive without blocking inside `while` loops waiting for RX bytes; incoming bytes trigger `USART2_IRQ_Callback()` instantly.
* **Accurate Baud Rate Match:** With $f_{CK} = 16\text{ MHz}$, calculating $\text{USARTDIV} = \frac{16,000,000}{16 \times 115200} = 8.6875$ gives `Mantissa = 8 (0x8)` and `Fraction = 11 (0xB)`, producing precise `0x8B` BRR timing.
* **Safe Null-Terminated Buffering:** `strRecieve()` prevents buffer overflows up to `MAX_BUFFER_LENGTH` (200 bytes) and correctly sets `RxMessageLength` when receiving `\0` to trigger state transitions safely.

## BRR register equation : (for USART2) 
<img width="1920" height="1080" alt="BRR register equation" src="https://github.com/user-attachments/assets/dd130d3d-8462-4b28-9a04-59681deccc70" />

## Simulation using hterm software as a Serial Terminal interface :

<img width="496" height="802" alt="Capture d’écran (1622)" src="https://github.com/user-attachments/assets/1c06db09-7b7c-4797-8cf0-178fbb73a129" />

<img width="643" height="785" alt="Capture d’écran (1623)" src="https://github.com/user-attachments/assets/168407f5-e9ce-42cc-8a34-a211b0318e91" />

##  Simulation video of transferring DATA (using hterm software as a Serial Terminal interface PC to communicate with the STM32F446RE):


https://github.com/user-attachments/assets/5d236252-b898-4f6f-bec3-010a376e1da9





