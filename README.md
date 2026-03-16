# STM32-BlackBox-Telemetry

### **"Solving the Invisible Bug: High-Reliability Diagnostic Logging"**

## 🚀 The Problem

In remote embedded deployments, a system crash is a nightmare. Without physical access to a debugger, developers are left guessing why a device failed in the field. "It worked on my desk" isn't an option for mission-critical hardware.

## 💡 The Solution

**STM32-BlackBox-Telemetry** is a professional-grade "Flight Recorder" for embedded systems. It utilizes a zero-CPU-overhead architecture to continuously log system health, task switches, and memory states. When a `HardFault` occurs, the system captures the final "seconds of life" into non-volatile storage for post-mortem analysis.

---

## 🛠 Key Technical Features

* **DMA-Backed Circular Buffers:** Asynchronous data transfer ensures that logging does not interfere with real-time application performance.
* **High-Speed SDIO Interface:** Uses a 4-bit wide bus for maximum throughput compared to standard SPI.
* **HardFault Intercept:** Custom assembly/C handler that dumps RAM to the SD card during a system crash.
* **Thread-Safe Logging:** Built for **FreeRTOS** with Mutex protection for multi-task telemetry.
* **Binary-to-CSV Parser:** Includes a Python-based diagnostic tool to visualize the crash data.

---

## 🏗 System Architecture

The system is designed around the **Nucleo-F446RE** (ARM Cortex-M4).

1. **Application Layer:** Tasks push data to a `Telemetry_Queue`.
2. **Middle Layer:** A low-priority logging task manages a double-buffer system.
3. **Hardware Layer:** DMA controllers stream data directly to the SDIO peripheral, bypassing the CPU.

---

## 📋 Hardware Requirements

* **MCU:** STM32 Nucleo-F446RE
* **Storage:** MicroSD Card Breakout (SDIO compatible)
* **Tools used:** STM32CubeIDE, Logic Analyzer (PulseView), Saleae.

---

## 📊 Sample Output

After a simulated crash, the "Post-Mortem" tool generates a report like this:

| Timestamp (ms) | Task ID | CPU Load | Heap Free (Bytes) | Last Event |
| --- | --- | --- | --- | --- |
| 14500 | 0x01 | 12% | 12400 | Sensor_Read_OK |
| 14600 | 0x01 | 14% | 12380 | Sensor_Read_OK |Page Structure

A good README.md often has these sections:

    Title
    Description
    Motivation
    Quick Start
    Usage
    Contributing

We will look at each of these in detail later, but for now let's work on your title.
Title Section

Give your project a good name! Don't name it "project", "API server", or "test app". Give it a name that's unique and memorable. Personality is good. Here are some examples:

    "Text Tunnel": A terminal-based real-time messaging app
    "Steamiest": A web app that tells you which of your Steam friends has the most similar game library
    "slowviz": A library that makes it easy to visualize the network latency of a Go application

Most importantly it should:

    Pique interest
    Hint at what the project does

Markdown Formatting

Your README.md should be a well-formatted Markdown file. Your title should be an H1 (heading 1, the largest heading). Subheadings should be H2s and H3s to organize the rest of your README into a neat hierarchy.

Use rich text to make your README easy to read. Don't be shy of bold, italics, numbered lists, bullet points, and other formatting options. Skimmability is good!
Assignment

Submit the link to your project's GitHub repo after you've added a title with an H1 heading.

| **14701** | **0x03** | **98%** | **12** | **STACK_OVERFLOW_DETECTED** |
| 14702 | CRASH | -- | -- | HardFault_Triggered |

---

## 🏁 How to Build

1. Clone the repo: `git clone https://github.com/yourusername/STM32-BlackBox-Telemetry.git`
2. Open in **STM32CubeIDE**.
3. Connect your Nucleo-F446RE via the Morpho headers (see `docs/wiring.md`).
4. Build and Flash.

---

## 👤 About the Author

I am an Embedded Software Developer focusing on C/C++ and ARM architecture. I specialize in building reliable, remote-ready firmware that prioritizes diagnostics and system integrity.

---

### Pro-Tip for your GitHub:

In your `docs/` folder, add a **"Lessons Learned"** file. Mention that you chose **SDIO over SPI** to learn about high-speed signal integrity, and how you managed the **HardFault** logic. This shows you have a "growth mindset," which remote employers love.

**Would you like me to help you write the Python script that converts the binary SD card data into that CSV table for the README?**