/*
 * =============================================================================
 * Project: Firmware Over-The-Air (FOTA) Update for STM32
 * 
 * Description:
 * This project provides a Firmware Over-The-Air (FOTA) update solution 
 * designed for STM32 microcontrollers. It utilizes an ESP32 as an intermediary 
 * device to fetch a firmware binary file hosted on a remote server (e.g., GitHub)
 * and transfers it securely to the STM32 over UART. Once received, the STM32 
 * bootloader takes responsibility for storing the firmware in Flash memory and 
 * launching the updated application.
 * 
 * -----------------------------------------------------------------------------
 * Key Features:
 * 1. **Remote Firmware Download**: The ESP32 retrieves the firmware binary 
 *    from an HTTPS server (e.g., GitHub Releases).
 * 2. **UART Communication**: A robust protocol is used for transferring the 
 *    binary file from the ESP32 to the STM32.
 * 3. **STM32 Bootloader**:
 *    - Manages incoming firmware data over UART.
 *    - Stores the firmware securely in Flash memory.
 *    - Validates and launches the updated application after a successful update.
 * 
 * -----------------------------------------------------------------------------
 * Architecture Overview:
 * The project is divided into two primary modules:
 * 
 * 1. **ESP32 Module**:
 *    - Establishes a Wi-Fi connection.
 *    - Downloads the firmware binary file from the server using HTTPS.
 *    - Temporarily stores the file in SPIFFS (flash storage on ESP32).
 *    - Sends the file to the STM32 over UART using a simple and reliable 
 *      communication protocol.
 * 
 * 2. **STM32 Module**:
 *    - Includes a custom bootloader developed with STM32 HAL libraries.
 *    - The bootloader performs the following tasks:
 *      - Receives the firmware file over UART.
 *      - Writes the firmware to the Flash memory at the specified address 
 *        (`0x08004400` for the user application).
 *      - Verifies the integrity of the firmware file.
 *      - Jumps to the user application once the update is successfully completed.
 * 
 * -----------------------------------------------------------------------------
 * Required Tools and Libraries:
 * 
 * - **ESP32**:
 *   - Arduino Core for ESP32
 *   - Libraries:
 *     - `WiFi.h`: Manages Wi-Fi connections.
 *     - `WiFiClientSecure.h`: Handles secure HTTPS connections.
 *     - `SPIFFS.h`: Enables temporary file storage on ESP32.
 * 
 * - **STM32**:
 *   - STM32 HAL (Hardware Abstraction Layer).
 *   - STM32CubeIDE for development.
 *   - STM32CubeProgrammer for flashing the bootloader.
 * 
 * -----------------------------------------------------------------------------
 * How to Use:
 * 
 * 1. **Prepare the Firmware File**:
 *    - Compile the STM32 user application to generate a `.bin` firmware file.
 *    - Upload the firmware file to a remote server accessible via HTTPS, 
 *      such as GitHub Releases.
 * 
 * 2. **Configure the ESP32**:
 *    - Update the Wi-Fi credentials (`WIFI_SSID` and `WIFI_PASSWORD`) 
 *      in the ESP32 firmware source code.
 *    - Set the URL of the firmware file (`FILE_URL`) to the HTTPS link 
 *      where the binary is hosted.
 * 
 * 3. **Flash the Firmware**:
 *    - Flash the ESP32 firmware using the Arduino IDE.
 *    - Flash the STM32 bootloader using STM32CubeProgrammer.
 * 
 * 4. **Test the System**:
 *    - Power on both the ESP32 and the STM32.
 *    - Monitor the UART logs to verify:
 *      - The ESP32 successfully downloads and transfers the firmware.
 *      - The STM32 bootloader receives the firmware, writes it to Flash, 
 *        and launches the updated application.
 * 
 * -----------------------------------------------------------------------------
 * Team:
 * - Loulid Anas
 * - Nser El Hattab
 * - Ait Mouna Youssef
 * 
 * -----------------------------------------------------------------------------
 * References:
 * - Arduino Core for ESP32 Documentation: 
 *   https://docs.espressif.com/projects/arduino-esp32/en/latest/
 * - STM32 HAL Documentation:
 *   https://www.st.com/en/development-tools/stm32cubeide.html
 * - GitHub Releases:
 *   https://docs.github.com/en/repositories/releasing-projects-on-github
 * =============================================================================
 */
