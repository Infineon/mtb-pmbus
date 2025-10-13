/***************************************************************************//**
* \file mtb_pmbus.h
* \version 1.0
*
* Provides API declarations for the PMBus Middleware.
*
********************************************************************************
* \copyright
* (c) (2025), Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation. All rights reserved.
********************************************************************************
* This software, including source code, documentation and related materials
* ("Software") is owned by Cypress Semiconductor Corporation or one of its
* affiliates ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress's integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*******************************************************************************/

/**
* \mainpage
*
********************************************************************************
* \section section_mainpage_overview Overview
********************************************************************************
*
* The purpose of PMBus middleware is to provide a solution for the 
  implementation of the SMBus/PMBus target device.
*
********************************************************************************
* \section section_pmbus_general General Description
********************************************************************************
*
* The PMBus middleware provides the in-build handling of pages and phases and supports:
* - Target Mode
* - PEC (Packet Error Code)
* - SMBus protocols: Quick Command, Send Byte, Receive Byte,
*   Write Byte/Word, Read Byte/Word, Process Call, Block Write/Read, Block Write-Read
*   Process Call, Write 32/64, and Read 32/64
* - Zone Read and Zone Write protocols
* - Group Command Protocol
* - Multi-instances
*
********************************************************************************
* \section section_pmbus_quick_start Quick Start Guide
********************************************************************************
* This section provides a step-by-step guide to quickly get started with the PMBus Middleware. 
* Namely: how to set up the PMBus Middleware in your project, configure the 
* necessary hardware, and implement the basic SMBus/PMBus target functionality
*
********************************************************************************
* \subsection pmbus_qs_add_middleware 1. Add mtb-pmbus middleware to your project
********************************************************************************
*
*  \image html pmbus_qsg_library_manager.png
* - If you work in the ModusToolbox IDE, use the ModusToolbox Library Manager to add the mtb-pmbus middleware to your project.
*   Otherwise, ensure that mtb-pmbus middleware is included into your project.
*     \note Middleware uses printf() for logging purposes. 
*     To use printf() for the terminal output, add
*     [<b>retarget-io middleware </b>] (https://infineon.github.io/retarget-io/html/index.html)
*     from the Library Manager or in any other way.
*
********************************************************************************
* \subsection pmbus_qs_configure_scb_and_gpio 2. Configure SCB blocks and GPIO pins
********************************************************************************
*
* 1.  Open the Device Configurator and switch to the Peripherals tab (#1.0).
* 2.  Enable the SCB block under Communication (#1.1) and select the I2C Personality (#1.2). Select the desired name for the SCB (e.g., PMBUS_I2C).
* 3.  In the "General" section (#1.3), choose Slave mode and set the I2C Data Rate to the desired value (e.g., 100 kHz).
*     Disable the "Use TX FIFO" option and enable the "Accept Matching Address" in RX FIFO option.
* 4.  In the "Slave" section (#1.4), set the Slave Address to the desired SMBus/PMBus device address (e.g., 0x18).
* 5.  In the "Connect" section (#1.5), select the desired Clock for the SCB block. 
*     Also, select the desired pins for SDA and SCL lines, the Device Configurator will automatically configure them to Open Drain mode.
*  \image html pmbus_qsg_scb_cfg.png
* 6.  Enable another SCB block under Communication and select UART Personality (#2.0). 
*     Select the desired name for the SCB. This block will be used for the debug output.
* 7.  Select the desired pins and clock for the SCB (#2.1). The other UART options can be set by default, see the screenshot.
*  \image html pmbus_qsg_uart_cfg.png
* 8.  Switch to the Pins tab (#3.0) and
*     enable any User LED GPIO pin (#3.1). In the "General" section (#3.2) select the
*     Strong Drive, input buffer off Drive mode.
*  \image html pmbus_qsg_led_cfg.png
* 9.  Select File->Save to generate initialization code.
*
********************************************************************************
* \subsection pmbus_qs_add_code 3. Add SMBus/PMBus code to your project
********************************************************************************
*
* This section describes the implementation of either an SMBus or PMBus target device.
* The implementation is similar for both protocols, with some differences in the command table and configuration.
* Both implementations support the Quick Command and Read Byte protocols,
* while the PMBus implementation additionally supports the PAGE command and Write/Read Word protocols with pages.
*
* 1. Fill the mtb_pmbus_conf.h file with the desired compile-time configuration macros. 
*    This file will appear in the project import folder after adding the mtb-pmbus middleware.
* \snippet code_snippets/qsg_pmbus_example/mtb_pmbus_conf.h PMBUS_CONF_EXAMPLE
* 2. Include the necessary headers files in main.c file:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_INCLUDED_FILES
* 3. Create variables for:
*     - Debug UART HAL object and context:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_UART_GLOBALS
*     - I2C context:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_HW_CFG_I2C_HAL
*     - SMBus/PMBus instance:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_HW_CFG_GLOBALS
* 4. Implement General event callback function:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_GEN_CALLBACK
* 5. Create buffers for SMBus/PMBus test commands:
*     - For **PMBus** project:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_CMD_BUFFERS
*     - For **SMBus** project:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c SMBUS_CMD_BUFFERS
* 6. Implement the SMBus/PMBus command callback functions:
*     - For **PMBus** project:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_CMD1_CALLBACK
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_CMD2_CALLBACK
*     - For **SMBus** project:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_CMD1_CALLBACK
* 7. Implement the SMBus/PMBus command table:
*     - For **PMBus** project:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_CMD_TABLE
*     - For **SMBus** project:
*  \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c SMBUS_CMD_TABLE
* 8. Implement I2C interrupt handler function:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_I2C_ISR
* 9. Implement callback functions for:
*     - Enabling and disabling the I2C:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_hw_resource_enable_callback
*     - Enabling and disabling I2C interrupts:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_ISR_ENABLE_DISABLE
* 10. Implement the SMBus/PMBus hardware configuration structure:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_HW_CONFIG_STRUCT
* 11. Implement the SMBus/PMBus configuration structure:
*     - For **PMBus** project:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_CONFIG_STRUCT
*     - For **SMBus** project:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c SMBUS_CONFIG_STRUCT
* 12. (From this step add code to the main() function) Create variables for result statuses:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_STATUS_VARIABLES
* 13. Initialize the device and board peripherals:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_INIT_CYBSP
* 14. Initialize the UART for debug output:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_UART_SETUP
* 15. Enable irq:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_ENABLE_IRQ
* 16. Initialize the I2C hardware:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_HW_INIT
* 17. Initialize the SMBus/PMBus middleware instance:
* \snippet code_snippets/qsg_pmbus_example/qsg_snippet.c PMBUS_INIT_ENABLE
*
********************************************************************************
* \subsection pmbus_qs_check_workability 4. Check the SMBus/PMBus workability
********************************************************************************
* 1.  Build and program your project.
* 2.  Open a terminal application (e.g., PuTTY, Tera Term) and connect to the device's COM port.
* 3.  Check if I2C transport is initialized correctly and all commands are registered.
* \image html pmbus_qsg_pmbus_start.png
* 4.  Connect the I2C Controller to the device.
*
* Quick Command and Read Byte protocols test(SMBus/PMBus mode):
* 1.  Send Quick Command with the write direction to the device address(The User LED will toggle(#1.0))).
* \image html pmbus_qsg_quick_cmd.png
* 2.  Repeat the previous step with Quick Command several more times.
* 3.  Send Read Byte protocol with CMD1 command code (0x10) to the device address (the controller will read User LED toggle counter(#2.0)).
* \image html pmbus_qsg_read_byte.png
*
* Implemented PAGE command and Write/Read Word protocols with page support test(PMBus mode only):
* 1.  Send Write Word to the device address with PAGE command code(0x00) and page number (0x01) (Target will switch to page 1).
* \image html pmbus_qsg_page_cmd.png
* 9.  Send Write Word to the device address with CMD2 command code (0x11) and data bytes (0xAB 0xCD) (Target will write command data buffer on page 1).
* \image html pmbus_qsg_write_word.png
* 10. Send Read Word to the device address with CMD2 command code (Controller will read data from command data buffer on page 1).
* \image html pmbus_qsg_read_word.png
*
********************************************************************************
* \section section_pmbus_design Design Considerations
********************************************************************************
*
********************************************************************************
* \subsection section_pmbus_init Initialization sequence
********************************************************************************
* First, initialize the hardware resources for the PMBus Middleware
* without enabling it. The middleware determines when to enable the hardware resources.
* For this purpose, provide the following callbacks:
* - \ref mtb_pmbus_stc_config_hw_t::hw_resource_ctrl_callback
* - \ref mtb_pmbus_stc_config_hw_t::enable_hw_irq_callback
* - \ref mtb_pmbus_stc_config_hw_t::disable_hw_irq_callback
*
* After the hardware resources are initialized, call the \ref mtb_pmbus_init function.
* After initialization, you can update the default configuration of the middleware:
* - Update the contents of the command buffer
* - Disable and/or protect specific commands
* - Update the default Zone Read/Write settings (required only in specific cases,
*   typically the controller assigns the Zone Read/Write values)
* - Select the active page/phase (required only in specific cases,
*   typically the controller selects the active page/phase)
*
* \note The \ref mtb_pmbus_init function must be called before any other APIs to
* initialize the instance structure. A call to the other APIs before the init function
* can lead to unexpected behavior including a hard fault error.
*
* After completing the initial configuration, call the \ref mtb_pmbus_enable function.
* After calling \ref mtb_pmbus_enable, the middleware starts responding on the bus.
*
* The contents of the \ref mtb_pmbus_stc_config_hw_t, \ref mtb_pmbus_stc_config_t,
* and \ref mtb_pmbus_stc_config_cmd_t structures are not modified by the middleware,
* so they can be allocated in flash or RAM as needed. However, these structures must
* remain available for as long as the PMBus instance is active, and their contents
* are expected to remain unchanged after calling \ref mtb_pmbus_init.
*
* The contents of \ref mtb_pmbus_stc_t are internal and not subject to modification by the application.
* The \ref mtb_pmbus_stc_t structure is continuously updated by the middleware and
* therefore must be located in RAM.
*
* \ref mtb_pmbus_enable can only be called after \ref mtb_pmbus_init.
*
* The middleware does not provide a deinit function. To update the configuration,
* use the following sequence:
* 1. \ref mtb_pmbus_disable
* 2. \ref mtb_pmbus_init
* 3. \ref mtb_pmbus_enable
*
* The PMBus Middleware does not deinitialize the hardware resources. If the PMBus
* instance is disabled and no longer required, releasing the hardware resources is
* the responsibility of the application.
*
********************************************************************************
* \subsection section_pmbus_protocols Communication Protocols
********************************************************************************
* The PMBus Middleware supports multiple protocols defined in SMBus and PMBus
* specifications. This section provides implementation specific details in addition
* to the specifications.
*
* <b> Quick Command vs Received Byte Protocols: </b>
* The Quick Command with the Read direction bit and the Received Byte protocol are
* identical during the Address stage of the transfer. As a result, the target device
* cannot distinguish between them in time, which may lead to an error condition on the bus.
* \note After the last SCL clock for the ACK/NACK bit of the address part, the target starts to
* set the first bit of the Received Byte protocol. At the same time, if
* a Quick Command is initiated, the controller sets the SDA line to
* a low level to generate the STOP condition. Then, after setting the SCL line to a high level,
* the following cases can occur on the bus:
* - If the first bit of the Received Byte is 1, the controller wins arbitration
* and completes the transfer with a STOP condition. From the target's point of view, such
* a sequence of events on the bus is considered as arbitration lost.
* - If the first bit of the Received Byte is 0, the target
* continues holding the SDA line, which prevents the STOP event generation, and most
* likely a timeout error will be generated after 25 ms.
*
* As a result, you can use Quick Command and Received Byte as follows:
* - If only one protocol, either Quick Command or Received Byte, is used, no
* collision/errors occur on the bus.
* \note If the Quick Command is initiated by the controller and the Received Byte is disabled in the middleware,
* the middleware sends the 0xFF byte after the address part of the transfer and ignores
* the arbitration lost event. Sending 0xFF allows the controller to win arbitration.
* - No issue if only Quick Command with the Write direction and Received Byte
* are used.
* - If both Quick Command with Read direction and Received Byte are used, the first
* bit of the Received Byte must always be set to 1 to allow the controller to win arbitration
* in the case of Quick Command.
* \note For this case, the arbitration lost event is also ignored and the middleware does
* not report it to the application level.
*
*
* <b> Zone Read and Write Protocols: </b>
* The Middleware handles the Zone Read and Zone Write protocols, assigning
* new zones to the pages and tracking the new active zones on the bus automatically.
* Also, the Middleware cares about Command Control code and determines the type of
* actions (Status or Command), the byte ordering, bit swapping and respond mode.
* So, the commands handling is very similar to standard protocols.
* Also, to read/write command data, the Zone Read protocol can be used to request the
* status. The Middleware reports the requested status bits through in callback to
* application, see \ref mtb_pmbus_zone_events_t.
*
* To enable Zone protocols execute the next steps:
* - Enable Zone protocols in Compile Time Options by setting \ref MTB_PMBUS_SUPPORT_ZONE to 1U
* - Enable Zone in config structure by setting \ref mtb_pmbus_stc_config_t::enable_zone to true
* - Enable Zone Config and Zone Active commands in Compile Time Options using
* \ref MTB_PMBUS_IMPL_CMD_ZONE_CONFIG and \ref MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE
* - Enable Zone Config and Zone Active commands in config structure using
* \ref MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_EN and \ref MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_EN masks in
* \ref mtb_pmbus_stc_config_t::impl_cmd_mask field
* - Add Zone specific callback \ref mtb_pmbus_stc_config_t::zone_callback in case if status should
* be handled
*
* Zone Write and Zone Read command handling
* From User point of view, the commands handling during the Zone protocols is almost
* the same as for standard protocols. The only difference is that during Zone Read
* protocol, the lost arbitration event may occur when the Middleware loses arbitration
* to another target. To ensure that Controller completes the read of the command, always check if
* \ref MTB_PMBUS_CMD_DONE event has occurred.
*
* By default, after initialization of PMBus instance, the 0xFE zone is assigned for all pages.
* It is expected that the proper zone values are assigned by Controller or manually use the
* \ref mtb_pmbus_set_default_zones function.
*
* Zone protocols rely on the pages, but if pages are disabled for PMBus
* instance, the Zone Write and Zone Read are assigned globally for the whole instance.
*
********************************************************************************
* \subsection section_pmbus_timeout Timeout Handling
********************************************************************************
*
* The SMBus specification defines a few timeouts:
* - Detect clock low timeout (tTIMEOUT)
* - Cumulative clock low extend time (target device) (tLOW:TEXT)
* - Cumulative clock low extend time (controller device) (tLOW:CEXT)
*
* This middleware only detects the tTIMEOUT condition.
* The Timeout handling is a compile-time option.
*
* \note Under standard configuration, the middleware does not violate tLOW:TEXT.
* However, enabling logging can significantly impact the response time to received
* addresses and bytes, making it easy to exceed the cumulative clock low extend
* time. 
*
********************************************************************************
* \subsection section_pmbus_callback Callback Handling
********************************************************************************
*
* The PMBus Middleware provides multiple callbacks. Almost all callbacks are
* called inside the ISR handler, so these callback functions must be optimized
* for usage in ISR.
*
********************************************************************************
* \subsection section_pmbus_callback_cmd Command callback
********************************************************************************
*
* Each command may have its own callback. This callback is registered in the command
* config structure, see \ref mtb_pmbus_stc_config_cmd_t::callback. The list
* of events is available in \ref mtb_pmbus_cmd_events_t. The callback is called
* only with one event at a specific time moment.
*
* Besides the event, the PMBus Middleware also informs about the active page/phase
* and the received byte. The received byte is actual only for the
* \ref MTB_PMBUS_CMD_WRITE_BYTE event. If the command is not paged and/or
* phased, the page/phase parameters must be ignored.
*
* The callback can return the bool value, this value is ignored for all events
* except \ref MTB_PMBUS_CMD_WRITE_BYTE. For the \ref MTB_PMBUS_CMD_WRITE_BYTE
* event, the return value will indicate which response (true - ACK, false - NACK)
* to send after receiving the data byte. The \ref MTB_PMBUS_CMD_WRITE_BYTE event
* is triggered only for data bytes, for example, byte-count byte for Block
* Write/Read is completely handled inside the middleware.
*
* Events visualization for write transfer:
* \image html pmbus_cmd_callback_write.png width=50%
*
* Events visualization for read transfer:
* \image html pmbus_cmd_callback_read.png width=50%
*
* Events visualization for process call transfer:
* \image html pmbus_cmd_callback_process_call.png width=50%
*
* Events visualization for Zone Write transfer:
* \image html pmbus_cmd_callback_zone_wr.png width=50%
*
* Events visualization for Group Command transfer. Callback triggering is analyzed for Device 2:
* \image html pmbus_cmd_callback_group_command.png width=50%
* 
********************************************************************************
* \subsection section_pmbus_gen_cmd General callback
********************************************************************************
*
* The PMBus Middleware provides two common callbacks: one for general events (see
* \ref mtb_pmbus_handle_cmd_events_t) and one for Zone features (see
* \ref mtb_pmbus_handle_zone_events_t). The general events callback is required
* only if any of these events are used by the application. The middleware always
* responds to Quick Command with both RD and WR directions as required by the
* SMBus/PMBus specification, always sending an ACK after receiving the target address. Additionally,
* the middleware sends a byte in the Received Byte callback with the default value is 0xFF.
* \note When PMBus mode is enabled, a target address with the RD direction is considered
* a data content fault. The middleware still sends an ACK after receiving the address but generates
* the corresponding error in the \ref mtb_pmbus_handle_error_events_t callback.
*
* The \ref mtb_pmbus_handle_error_events_t callback is also used to report
* communication errors. This callback is typically invoked when a STOP condition
* occurs on the bus or when an extraordinary sequence of events happens,
* such as a bus error or timeout.
* It is important for the application to use this callback to provide the correct
* response to fault events. The application can ignore handling certain error
* events if they are not relevant to the features in use. For example, there is no
* need to handle PMBus-specific events when only SMBus mode is enabled.
*
********************************************************************************
* \subsection section_pmbus_cmd Command Organization
********************************************************************************
*
* The PMBus Middleware supports configuration of multiple pages and phases for commands.
*
* Page/Phase configuration for commands.
* The command supports two types of configuration:
* - The command is not paged/phased. In this case, only one of command copies is
* available for all pages/phases.
* - The command is paged/phased. In this case, the command has a number of copies
* configured in mtb_pmbus_stc_config_t::num_pages or mtb_pmbus_stc_config_t::num_phases.
*
* The same command can be paged and phased or only paged or only phased or neither.
*
* - If a command is paged/phased, implement a two-dimensional array for the data
* buffer:
* \code cmd_data[NUM_PAGES][DATA_SIZE] \endcode
* \code cmd_data[NUM_PHASES][DATA_SIZE] \endcode
* - If a command is paged and phased at the same time, a three-dimensional array is
* required for the data buffer:
* \code cmd_data[NUM_PAGES][NUM_PHASES][DATA_SIZE] \endcode
*
* \note For the block write/read protocols, increment the DATA_SIZE by one byte.
*       This additional byte is necessary to store the byte number within the package.
********************************************************************************
* \subsubsection section_pmbus_cmd_cap Command Capabilities
********************************************************************************
*
* The commands have multiple capabilities, which define the command behavior.
* Refer to \ref group_pmbus_macro_cmd_cap for more information.
*
********************************************************************************
* \subsubsection section_pmbus_impl_cmd Implemented Commands
********************************************************************************
*
* The PMBus Middleware has implemented several commands from PMBus specification.
* Usage of implemented commands is optional and command's own implementation can be
* provided through the general command table.
*
* Perform the next steps to use implemented commands:
* - Enable implemented command by a corresponding compile time macro in the mtb_pmbus_conf.h
* file by setting the macro to 1 (this setting is applicable for all instances).
* - Add the corresponding run time macro to \ref mtb_pmbus_stc_config_t::impl_cmd_mask
* (this setting is applicable only for specific instances)
* - Ensure that a command with the same code is not defined in the main command table:
* \ref mtb_pmbus_stc_config_t::cmd_table
*
* List of implemented commands:
* <table class="doxtable">
*   <tr><th>Name</th><th>Code</th><th>Implementation details</th><th>Compile time macro</th><th>Run-time macro</th></tr>
*   <tr>
*     <td>PAGE</td>
*     <td>0x00</td>
*     <td>The command stores the active page number and middleware uses it
*         to update the corresponding data buffer. Also, the controller can
*         read the last assigned page number.
*     </td>
*     <td> \ref MTB_PMBUS_IMPL_CMD_PAGE</td>
*     <td> \ref MTB_PMBUS_IMPL_CMD_PAGE_EN</td>
*   </tr>
*   <tr>
*     <td>PHASE</td>
*     <td>0x04</td>
*     <td>The command stores the active phase number and middleware uses it
*         to update the corresponding data buffer. Also, the controller can
*         read the last assigned phase number.
*     </td>
*     <td> \ref MTB_PMBUS_IMPL_CMD_PHASE</td>
*     <td> \ref MTB_PMBUS_IMPL_CMD_PHASE_EN</td>
*   </tr>
*   <tr>
*     <td>ZONE_CONFIG</td>
*     <td>0x07</td>
*     <td>The command stores the configured Read and Write Zones. The middleware uses it
*         during Zone Read and Write Protocols.
*     </td>
*     <td> \ref MTB_PMBUS_IMPL_CMD_ZONE_CONFIG</td>
*     <td> \ref MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_EN</td>
*   </tr>
*   <tr>
*     <td>ZONE_ACTIVE</td>
*     <td>0x08</td>
*     <td>The command sets Read and Write Zones. The middleware uses it
*         during Zone Read and Write Protocols.
*     </td>
*     <td> \ref MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE</td>
*     <td> \ref MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_EN</td>
*   </tr>
*   <tr>
*     <td>CAPABILITY</td>
*     <td>0x19</td>
*     <td>The command returns some key capabilities of the PMBus Device:
*         - If PEC is enabled
*         - The Maximum supported Bus Speed
*         - If SMBALERT pin is present
*         - The supported Number Format
*         - AVSBus support is always disabled for this implementation</td>
*     <td> \ref MTB_PMBUS_IMPL_CMD_CAPABILITY</td>
*     <td> \ref MTB_PMBUS_IMPL_CMD_CAPABILITY_EN</td>
*   </tr>
*   <tr>
*     <td>QUERY</td>
*     <td>0x1A</td>
*     <td>The commands returns info about the requested command
*         - If the command is supported (Defined in command table)
*         - The supported directions (WR OR/AND RD)
*         - The supported numeric format.
*
*         This information is configured in
*         \ref mtb_pmbus_stc_config_cmd_t::cmd_cap.
*         The controller can request information only for the command from
*         \ref mtb_pmbus_stc_config_t::cmd_table
*     </td>
*     <td> \ref MTB_PMBUS_IMPL_CMD_QUERY</td>
*     <td> \ref MTB_PMBUS_IMPL_CMD_QUERY_EN</td>
*   </tr>
*   <tr>
*     <td>PMBUS_REVISION</td>
*     <td>0x98</td>
*     <td>The command returns the PMBus revision configured in
*         \ref mtb_pmbus_stc_config_t::revision
*    </td>
*     <td> \ref MTB_PMBUS_IMPL_CMD_REVISION</td>
*     <td> \ref MTB_PMBUS_IMPL_CMD_REVISION_EN</td>
*   </tr>
* </table>
*
********************************************************************************
* \subsection section_pmbus_data_conv Data Format conversion functions
********************************************************************************
*
* The PMBus Middleware provides a set of functions to convert data between
* PMBus data formats and Float32 format. The conversion functions are:
* - From PMBus LINEAR11 to Float32. See \ref mtb_pmbus_lin11_to_float.
* - From Float32 to PMBus LINEAR11. See \ref mtb_pmbus_float_to_lin11.
* - From PMBus LINEAR16 to Float32. See \ref mtb_pmbus_lin16_to_float.
* - From Float32 to PMBus LINEAR16. See \ref mtb_pmbus_float_to_lin16.
*
* \note LINEAR11 and LINEAR16 are less precise than 
*       Float32, so conversion from Float32 to 
*       PMBus LINEAR11/16 may lead to loss of precision.
*
* \note Mantissa of PMBus LINEAR11 and LINEAR16 are not normalized.
*       So different combinations of mantissa and exponent may lead to
*       the same value when converted to Float32:
* \code
*       0x0004(LIN11 HEX) ->  0 exponent, 4 mantissa   -> 4.0f;
*       0x1001(LIN11 HEX) ->  2 exponent, 1 mantissa   -> 4.0f;
*       0xCA00(LIN11 HEX) -> -7 exponent, 512 mantissa -> 4.0f;
* \endcode
*
********************************************************************************
* \subsection group_pmbus_signal Optional Signals
********************************************************************************
*
* The SMBus and PMBus specifications specify a set of optional signals. The
* middleware supports some of them.
*
* <table class="doxtable">
*   <tr><th>Signal Name</th><th>Specification</th><th>Support</th><th>Implementation details</th></tr>
*   <tr>
*     <td>SMBSUS#</td>
*     <td>Not supported</td>
*     <td>SMBus</td>
*     <td>This signal must be handled by the Application.
*         When the SMBSUS# signal goes low, the Application can call \ref mtb_pmbus_disable
*         function to disable the middleware. After the SMBSUS# goes high, the
*         Application can call \ref mtb_pmbus_enable to restore the communication.
*     </td>
*   </tr>
*   <tr>
*     <td>SMBALERT#</td>
*     <td>Supported</td>
*     <td>SMBus</td>
*     <td>The SMBALERT# signal has the in-build support in the middleware.
*         See: \ref mtb_pmbus_smbalert_config_mode, \ref mtb_pmbus_smbalert_set_signal
*         and \ref mtb_pmbus_smbalert_clear_signal.
*     </td>
*   </tr>
*   <tr>
*     <td>Control Signal (CONTROL)</td>
*     <td>Not supported</td>
*     <td>PMBus</td>
*     <td>This signal must be handled by the Application.
*    </td>
*   </tr>
*   <tr>
*     <td>Write Protect (WP)</td>
*     <td>Not supported</td>
*     <td>PMBus</td>
*     <td>This signal must be handled by the Application.
*         Use the \ref mtb_pmbus_cmd_wr_protect and
*         \ref mtb_pmbus_cmd_all_wr_protect to protect commands against write.
*    </td>
*   </tr>
* </table>
*
********************************************************************************
* \subsection group_pmbus_logging Logging
********************************************************************************
* The PMBus Middleware provides the possibility to enable the logging feature.
* The logging can be enabled by defining MTB_PMBUS_LOG_LEVEL with the selected log level
* in mtb_pmbus_conf.h file.
* See the available log levels - \ref group_pmbus_macro_log.
*
* By default, the logs are printed by the retarget-io middleware. So, initialize
* this middleware at the application level. If another output method is required,
* redirect the PMBus logging by defining MTB_PMBUS_CUSTOM_LOG in the mtb_pmbus_conf.h
* file and provide custom implementation of the mtb_pmbus_log function. Also,
* you can redefine the buffer size by MTB_PMBUS_LOG_BUF.
*
* \note If you select MTB_PMBUS_LOG_LEVEL_INFO or MTB_PMBUS_LOG_LEVEL_DEBUG as the log levels,
* too many log messages can be printed, which leads to a timeout error on the bus.
* Recommended:
* - Increase the data speed of the logging method
* - Redirect the logging data to the buffer and print messages after transfers
*
********************************************************************************
* \subsection group_pmbus_lpm Low Power Support
********************************************************************************
*
* The Middleware can operate only in power modes supported by the corresponding
* I2C/Timer hardware. Typically, only Active and Sleep modes are supported.
*
********************************************************************************
* \subsection group_pmbus_compile_time Compile Time Options
********************************************************************************
* The PMBus Middleware provides the opportunity to disable unused features to
* reduce the memory footprint and improve the speed of code execution. The Compile Time
* Options are applicable for all instances, so if Instance 0 uses PEC and Instance
* 1 does not use it, the \ref MTB_PMBUS_SUPPORT_PEC compile time option must be
* enabled.
*
* Also, if you disable some of the compile time options, not all APIs will be
* available. For example: \ref mtb_pmbus_stc_config_t::enable_pec is
* available only if \ref MTB_PMBUS_SUPPORT_PEC is set to 1U.
*
* Use the mtb_pmbus_conf.h file to reflect PMBus Middleware configuration in your
* application. This file will be automatically copied into your project once
* the middleware is added by the Library Manager.
*
* The list of Compile Time Option and their default values: \ref group_pmbus_macro_conf
*
********************************************************************************
* \subsection section_pmbus_hw_dependent_layer Hardware-dependent layer
********************************************************************************
*
********************************************************************************
* \subsubsection section_pmbus_hw_zone_limitations Limitations and exeptions of the Zone feature
********************************************************************************
*
* According to the PMBus specification, during a ZONE_READ transaction, the controller generates an
* Acknowledge (ACK) bit for the last byte read before issuing a repeated start condition. This behavior
* contradicts the I2C specification, which states that a read transaction must end with a Not Acknowledge
* (NACK) bit to indicate that the last byte has been read. The PMBus middleware is built on top of the I2C
* communication block, so it cannot reliably detect a restart or stop event during a read transaction if the
* last byte read by the controller was acknowledged with an ACK bit.
* 
* Note the ZONE_READ with PEC case: a sequence that also cannot be correctly recognized by the middleware
* due to hardware limitations, occurs before the stop event when receiving data from the last device in the zone:
* /Read last data byte/NACK/PEC/NACK/STOP.
*
********************************************************************************
* \subsubsection section_pmbus_timeout_timer Timeout Detection HW Implementations
********************************************************************************
*
* Employs an external timer triggered by a signal from the
* communication line, such as the SCL. This solution relies on the internal
* connection between the communication line and a timer to generate a
* trigger signal, thereby facilitating the timeout handling.
*
********************************************************************************
* \subsubsection section_pmbus_timeout_timer_cfg Timeout Detection Configuration
********************************************************************************
*
* To enable Timeout Detection, use macro \ref MTB_PMBUS_ENABLE_TIMEOUT.
*
* To initialize the TCPWM timeout handling, follow these steps:
*
* 1. Open the Device Configurator: Navigate to the Peripherals tab and enable the TCPWM as the Timer-Counter.
* 2. Configure TCPWM; Assign the peripheral clock to the Timer-Counter and set Run mode to One shot in the General section.
* 3. Set Period: Set the Period in the General section calculated as TIMER_FREQUENCY_CLOCK * 0.025.
* 4. Configure Compare: Select the Compare option as Compare and set the Compare value to the same value as the Period.
* 5. Configure Interrupt Source: Go to the Interrupt Source section and enable the Compare and Capture checkbox.
* 6. Configure Inputs: Move to the Inputs section, select the Stop Input as Rising Edge, and assign the scl_filtered signal of SCB to the Stop Signal.
* 7. Configure Reload Input: Select the Reload Input as Falling Edge and assign the scl_filtered signal of SCB to the Reload Signal.
* 8. Add Initialization Code: Add the following code to the HW initialization:
*     - Create the Timer IRQ handler:
*       \snippet code_snippets/hw_cfg/pmbus_hw_cfg.c PMBUS_TIMEOUT_TIMER_ISR_CFG
*     - Initialize the Timer:
*       \snippet code_snippets/hw_cfg/pmbus_hw_cfg.c PMBUS_TIMEOUT_TIMER_CFG
*     - Add TCPWM HW pointer and counter number to the mtb_pmbus_stc_config_hal_t:
*       \snippet code_snippets/hw_cfg/pmbus_hw_cfg.c PMBUS_TIMEOUT_TIMER_HAL_OBJ
*     - Provide/update \ref mtb_pmbus_hw_resources_ctrl_t for TCPWM as HW resource.
*       \snippet code_snippets/hw_cfg/pmbus_hw_cfg.c PMBUS_TIMEOUT_TIMER_EN_CALLBACK
*     - Provide/update \ref mtb_pmbus_hw_isr_ctrl_t to enable/disable TCPWM interrupt.
*       \snippet code_snippets/hw_cfg/pmbus_hw_cfg.c PMBUS_TIMEOUT_TIMER_INT_CTRL
*
********************************************************************************
* \subsubsection section_pmbus_timeout_timer_frequency_selection Frequency Selection for Timeout Handling
********************************************************************************
*
* The assigned clock is crucial for the timeout handling because it samples the SCL
* line with a frequency twice as high as the frequency of the communication
* signal. For example, for the 400 kHz I2C speed, the minimal value of the HIGH pulse
* of the SCL is 0.6 us (period 0.12 us). In this case, the sample frequency is expected to
* be (1 / 0.12 us) * 2 = 1.666666 MHz. Considering the clock accuracy, the sampled frequency
* is expected to be slightly higher, for example, 1.8 MHz. The Reload value must be configured
* to 45000 to measure 25 ms with this input frequency.
* 
* \note For some I2C speeds, the reload value may be higher than 65535, which
* imposes limitations on the use of the 16-bit timer for high I2C speeds.
*
********************************************************************************
* \subsubsection section_pmbus_hw_smbus_signal_configuration Hardware configuration of the SMBALERT signal
********************************************************************************
*
* According to the SMBUS spec, the SMBALERT# signal is a wired-AND signal. This
* means that the output of the combined signal is only true (or high) if all the
* individual input signals are true (or high). In the Wired-AND connection, multiple
* signals are connected together through a common wire or bus. Each signal is
* typically an open-collector or open-drain output, which means that they can only
* pull the signal low (to ground) but not drive it high. When all the signals are
* inactive (high impedance), the combined signal is pulled high by an external
* pull-up resistor. Use the Open-Drain-Drives-Low mode for the SMBALERT# signal
* to meet the requirements of the WIRED-AND connection. To determine the specific 
* capabilities of this mode, consult the Technical Reference Manual (TRM) documentation.
*
********************************************************************************
* \section group_pmbus_MISRA MISRA-C:2012 Compliance
********************************************************************************
*
* This section describes MISRA-C:2012 compliance and deviations for the PMBus.
*
* MISRA stands for Motor Industry Software Reliability Association. The MISRA
* specification covers a set of 10 mandatory rules, 110 required rules and
* 39 advisory rules that apply to the firmware design and has been put together
* by the Automotive Industry to enhance the quality and robustness of
* the firmware code embedded in automotive devices.
*
* The MISRA specification defines two categories of deviations (see section 5.4
* of the MISRA-C:2012 specification):
* * Project Deviations - deviations applicable for a particular class of
*   circumstances.
* * Specific Deviations - deviations applicable for a single instance
*   in a single file.
*
* Project Deviations are documented in the current section below.
*
* Specific deviations are documented in the source code, close to the
* deviation occurrence. For each deviation, a special macro identifies the
* relevant rule or directive number, and reason.
*
* <h2>Verification Environment</h2>
* This section provides a MISRA compliance analysis environment description.
*
* <table class="doxtable">
*   <tr>
*     <th>Component</th>
*     <th>Name</th>
*     <th>Version</th>
*   </tr>
*   <tr>
*     <td>Test Specification</td>
*     <td>MISRA-C:2012 Guidelines for the use of the C language in critical systems</td>
*     <td>March 2013</td>
*   </tr>
*   <tr>
*     <td rowspan="2">MISRA Checking Tool</td>
*     <td>Coverity Static Analysis Tool</td>
*     <td>2022.12.0</td>
*   </tr>
* </table>
*
* <h2>Project Deviation</h2>
* The list of deviated required rules is provided in the table below. Advisory
* rules deviation is not documented, as not required per MISRA specification.
*
* <table class="doxtable">
*   <tr>
*     <th>Rule ID</th>
*     <th>Rule Description</th>
*     <th>Description of Deviation(s)</th>
*   </tr>
*   <tr>
*     <td>Rule 3.1</td>
*     <td>SThe character sequences / * and / / shall not be used within a comment.</td>
*     <td>Required. Using of the special comment symbols is needed for Doxygen comment 
*         support; it does not have any impact.
*     </td>
*   </tr>
*   <tr>
*     <td>Rule 5.1</td>
*     <td>External identifiers shall be distinct.</td>
*     <td>Required. Toolchains from Supported Software and Tools documentation section are 
*         verified to work with functions whose names have similar first 31.
*         symbols.
*     </td>
*   </tr>
*   <tr>
*     <td>Rule 5.5</td>
*     <td>Identifiers shall be distinct from macro names.</td>
*     <td>Required. This rule applies to ISO:C90 standard. The middleware conforms to ISO:C99, 
*         which does not require this limitation.
*     </td>
*   </tr>
*   <tr>
*     <td>Rule 5.8</td>
*     <td>Identifiers that define objects or functions with external linkage shall be unique.</td>
*     <td>Required. During the code analysis, the same source files are compiled multiple 
*         times with device-specific options. All object and function identifiers 
*         are unique for each specific run.
*     </td>
*   </tr>
*   <tr>
*     <td>Rule 5.9</td>
*     <td>Identifiers for objects with internal linkage shall be unique.</td>
*     <td>Required. During the code analysis, the same source files are compiled 
*         multiple times with device-specific options. All object and function 
*         identifiers are actually unique for each specific run.
*     </td>
*   </tr>
*   <tr>
*     <td>Rule 8.6</td>
*     <td>An identifier with external linkage shall have exactly one external definition.</td>
*     <td>Required. During the code analysis, the same source files are compiled 
*         multiple times with device-specific options. All object and function 
*         identifiers are unique for each specific run.
*     </td>
*   </tr>
*   <tr>
*     <td>Rule 11.8</td>
*     <td>A cast should not remove any const or volatile qualifications from the type
*         pointed to by a pointer</td>
*     <td>Required. Casting a volatile pointer to non-volatile is required in an interrupt
*         context to manipulate a user-provided mtb_pmbus_stc_t object. Volatile is preserved
*         during the variables lifecycle to prevent compiler optimization.
*     </td>
*   </tr>
*   <tr>
*     <td>Rule 21.6</td>
*     <td>The Standard Library input/output functions shall not be used.</td>
*     <td>Required. Deviated since usage of printf is required for logging.</td>
*   </tr>
* </table>
*
********************************************************************************
* \section group_pmbus_changelog Changelog
********************************************************************************
*
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td rowspan="1">0.5.0</td>
*     <td>Initial version</td>
*     <td></td>
*   </tr>
* </table>
*
* \defgroup group_pmbus_macro         Macros
* \defgroup group_pmbus_functions     Functions
* \defgroup group_pmbus_data_structs  Data Structures
* \defgroup group_pmbus_enums         Enumerated Types
* \defgroup group_pmbus_hal_apis      Hardware Specific APIs
*/

#ifndef MTB_PMBUS_H
#define MTB_PMBUS_H

#include "mtb_pmbus_conf_def.h"
#include "mtb_pmbus_log.h"
#include "mtb_pmbus_hal.h"

#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*                          Middleware Version
*******************************************************************************/

/** PMBus middleware major version */
#define MTB_PMBUS_MW_VERSION_MAJOR                (1U)

/** PMBus middleware minor version */
#define MTB_PMBUS_MW_VERSION_MINOR                (0U)

/** PMBus middleware version */
#define MTB_PMBUS_MW_VERSION                      (10U)

/* Internal */
#define MTB_PMBUS_CMD_MAX_NUM                   (256U)
#define MTB_PMBUS_CMD_CAP_FORMAT_POS            (6U)

/**
* \addtogroup group_pmbus_macro
* \{
*/
/** Use this macro for \ref mtb_pmbus_cmd_update_data_ext, \ref mtb_pmbus_cmd_read_data_ext,
 * \ref mtb_pmbus_cmd_get_transfer_size_ext as input parameters for page or phase fields in case
 * if command is not paged or/and not phased.
 */
#define MTB_PMBUS_NO_PAGE_PHASE                 (-1)

/** The maximum command size in bytes */
#define MTB_PMBUS_CMD_MAX_SIZE                  (255U)

/**
* \defgroup group_pmbus_macro_cmd_cap Command capabilities macro
 * Command Capabilities setting rules:
 * - The \ref MTB_PMBUS_CMD_CAP_DIR_WR, \ref MTB_PMBUS_CMD_CAP_DIR_RD,
 * \ref MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL, and \ref MTB_PMBUS_CMD_CAP_BLOCK
 * are bitwise macros, and a few of them can be applied to one command.
 * However, it is forbidden to combine \ref MTB_PMBUS_CMD_CAP_DIR_WR,
 * \ref MTB_PMBUS_CMD_CAP_DIR_RD, and \ref MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL
 * together. Only two of them can be selected for one command.
 * - Additionally, each command must have one of the formats selected.
 * The formats cannot be combined. The selected format only impacts the
 * response of implemented QUERY command if the QUERY command is enabled.
 * 
 * Valid combinations of capabilities:
 * - MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_FORMAT_16_BIT
 * - MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_DIR_MODE
 * - MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL | MTB_PMBUS_CMD_CAP_FORMAT_8_BIT
 *
 * Not valid combination of capabilities:
 * - MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL |
 * MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM: \ref MTB_PMBUS_CMD_CAP_DIR_WR, \ref MTB_PMBUS_CMD_CAP_DIR_RD and
 * \ref MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL can not be selected for one command
 * - MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD: At least one numeric format must be selected
 *
 * \{
 */

/** The command supports write direction */
#define MTB_PMBUS_CMD_CAP_DIR_WR                (0x1U)
/** The command supports read direction */
#define MTB_PMBUS_CMD_CAP_DIR_RD                (0x2U)
/** The command supports Process Call */
#define MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL      (0x4U)
/** The command supports Block Write/Read protocols */
#define MTB_PMBUS_CMD_CAP_BLOCK                 (0x8U)
/** The command is paged */
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
#define MTB_PMBUS_CMD_CAP_PAGE                  (0x10U)
#else
#define MTB_PMBUS_CMD_CAP_PAGE                  (0U)
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
/** The command is phased */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U))
#define MTB_PMBUS_CMD_CAP_PHASE                 (0x20U)
#else
#define MTB_PMBUS_CMD_CAP_PHASE                 (0U)
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) */
/** The command uses LINEAR11 or ULINEAR16 numeric format  */
#define MTB_PMBUS_CMD_CAP_FORMAT_LIN_11_16      (0U << MTB_PMBUS_CMD_CAP_FORMAT_POS)
/** The command uses 16 bit signed number */
#define MTB_PMBUS_CMD_CAP_FORMAT_16_BIT         (1U << MTB_PMBUS_CMD_CAP_FORMAT_POS)
/** The command uses IEEE Half Precision Floating Point Format */
#define MTB_PMBUS_CMD_CAP_FORMAT_IEEE           (2U << MTB_PMBUS_CMD_CAP_FORMAT_POS)
/** The command uses Direct Mode Format */
#define MTB_PMBUS_CMD_CAP_FORMAT_DIR_MODE       (3U << MTB_PMBUS_CMD_CAP_FORMAT_POS)
/** The command uses 8 bit unsigned */
#define MTB_PMBUS_CMD_CAP_FORMAT_8_BIT          (4U << MTB_PMBUS_CMD_CAP_FORMAT_POS)
/** The command uses VID Mode Format */
#define MTB_PMBUS_CMD_CAP_FORMAT_VID_MODE       (5U << MTB_PMBUS_CMD_CAP_FORMAT_POS)
/** The command uses Manufacturer specific format */
#define MTB_PMBUS_CMD_CAP_FORMAT_MFC_SPEC       (6U << MTB_PMBUS_CMD_CAP_FORMAT_POS)
/** The command does not use any numeric data  */
#define MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM         (7U << MTB_PMBUS_CMD_CAP_FORMAT_POS)

/** \} group_pmbus_macro_cmd_cap */

/**
* \defgroup group_pmbus_macro_error_evt Error events
* \{
*/

/** Controller attempts to read too many bytes */
#define MTB_PMBUS_ERR_RD_TOO_MANY_BYTES         (0x01U)
/** Controller attempts to write too many bytes */
#define MTB_PMBUS_ERR_WR_TOO_MANY_BYTES         (0x02U)
/** Received command is unsupported */
#define MTB_PMBUS_ERR_UNSUPPORTED_CMD           (0x04U)
/** Controller writes less byte than expected */
#define MTB_PMBUS_ERR_WR_LESS_BYTES             (0x08U)
/** Controller read less byte than expected */
#define MTB_PMBUS_ERR_RD_LESS_BYTES             (0x10U)
/** Bus reset timeout occurred */
#define MTB_PMBUS_ERR_TIMEOUT                   (0x20U)
/** Received PEC does not match */
#define MTB_PMBUS_ERR_CORRUPTED_DATA            (0x40U)
/** Controller attempts to write data after the second address match */
#define MTB_PMBUS_ERR_WR_AFTER_SECOND_ADDR      (0x80U)
/** The received count byte exceeds the maximum command size.
 * Applicable only Block Protocols
 */
#define MTB_PMBUS_ERR_BYTE_COUNT_TOO_BIG        (0x100U)
/** The controller attempts to write data for the read only command */
#define MTB_PMBUS_ERR_REQ_WR_FOR_RD_ONLY        (0x200U)
/** The Middleware loses Arbitration. This error is not reported
 * in the following cases:
 * - Arbitration is lost during Target response on ARA
 * (Address Response Alert)
 * - Arbitration is lost during Zone Read Protocol
 * 
 * In these cases, the lost Arbitration is not an error.
*/
#define MTB_PMBUS_ERR_ARB_LOST                  (0x400U)
/** The controller sets an unsupported page for the current target.
 * These errors can be triggered only if the PAGE command implemented
 * is enabled in \ref mtb_pmbus_stc_config_t::impl_cmd_mask.
 */
#define MTB_PMBUS_ERR_INVALID_PAGE              (0x800U)
/** The controller sets am unsupported phase for the current target.
 * These errors can be triggered only if the implemented PHASE command
 * is enabled in \ref mtb_pmbus_stc_config_t::impl_cmd_mask.
 */
#define MTB_PMBUS_ERR_INVALID_PHASE             (0x1000U)
/** Command callback from the application sends NACK after receiving the byte */
#define MTB_PMBUS_ERR_APP_NACK_BYTE             (0x2000U)
/** The error code indicating a write operation to a protected PMBus command */
#define MTB_PMBUS_ERR_WR_TO_PROT_CMD            (0x4000U)
/** An error is detected on the bus. Typically, it is an
 * unexpected Start or Stop condition.
 */
#define MTB_PMBUS_ERR_BUS_ERROR                 (0x8000U)
/** The protocol is started with Target Address with Read Direction.
 *
 * The error is only applicable for PMBus mode.
 */
#define MTB_PMBUS_ERR_ADDR_WITH_RD_BIT          (0x10000U)
/** An error is detected when a read request occurs for a command
 *  specified as write and the process call.
 */
#define MTB_PMBUS_ERR_REQ_RD_FOR_PC_ONLY        (0x20000U)
/** An error is detected when a write request occurs for a command
 *  specified as read and the process call.
 */
#define MTB_PMBUS_ERR_REQ_WR_FOR_PC_ONLY        (0x40000U)
/** The zone value assigned by the controller is out of range */
#define MTB_PMBUS_ERR_ZONE_VALUE_OUT_OF_RANGE   (0x80000U)
/** Controller request read transfer for the paged/phased command when
 * an active page/phase is set to 0xFF.
 */
#define MTB_PMBUS_ERR_READ_REQ_FOR_FF           (0x100000U)

/** \} group_pmbus_macro_error_evt */

/**
* \defgroup group_pmbus_macro_impl_cmd Implemented Commands macro
* \{
*/

/** Enable the PAGE (0x00) command */
#define MTB_PMBUS_IMPL_CMD_PAGE_EN              (1U << 0U)
/** Enable the PHASE (0x04) command */
#define MTB_PMBUS_IMPL_CMD_PHASE_EN             (1U << 1U)
/** Enable the CAPABILITY (0x19) command */
#define MTB_PMBUS_IMPL_CMD_CAPABILITY_EN        (1U << 2U)
/** Enable the QUERY (0x1A) command */
#define MTB_PMBUS_IMPL_CMD_QUERY_EN             (1U << 3U)
/** Enable the PMBUS_REVISION (0x98) command */
#define MTB_PMBUS_IMPL_CMD_REVISION_EN          (1U << 4U)
/** Enable the ZONE_CONFIG (0x07) command */
#define MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_EN       (1U << 5U)
/** Enable the ZONE_ACTIVE (0x08) command */
#define MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_EN       (1U << 6U)

/** \} group_pmbus_macro_impl_cmd */

/** \} group_pmbus_macro */

/**
* \addtogroup group_pmbus_enums
* \{
*/
/** Used to return the statuses of most PMBus APIs */
typedef enum
{
    /** Correct status. No error. */
    MTB_PMBUS_STATUS_SUCCESS = MTB_PMBUS_HAL_STATUS_SUCCESS,
    /** The provided command code is incorrect */
    MTB_PMBUS_STATUS_INVALID_CMD_CODE = MTB_PMBUS_HAL_STATUS_INVALID_CMD_CODE,
    /** The provided page is incorrect */
    MTB_PMBUS_STATUS_INVALID_PAGE = MTB_PMBUS_HAL_STATUS_INVALID_PAGE,
    /** The provided phase is incorrect */
    MTB_PMBUS_STATUS_INVALID_PHASE = MTB_PMBUS_HAL_STATUS_INVALID_PHASE,
    /** One or more of input parameters are invalid */
    MTB_PMBUS_STATUS_BAD_PARAM = MTB_PMBUS_HAL_STATUS_BAD_PARAM,
    /** The requested size is bigger than command size */
    MTB_PMBUS_STATUS_TOO_MANY_BYTES = MTB_PMBUS_HAL_STATUS_TOO_MANY_BYTES,
    /** The requested action is aborted for the command as this command takes part in communication */
    MTB_PMBUS_STATUS_CMD_IS_ACTIVE = MTB_PMBUS_HAL_STATUS_CMD_IS_ACTIVE,
    /** The requested action is aborted as the PMBus instance takes part in communication */
    MTB_PMBUS_STATUS_IS_BUSY = MTB_PMBUS_HAL_STATUS_IS_BUSY,
} mtb_pmbus_status_t;

#if defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)
/** The modes for SMBALERT pin 
 * \note This enum is only available when \ref MTB_PMBUS_SUPPORT_SMBALERT is enabled at compile time options.
 */
typedef enum
{
    /** The user must manually clear SMBALERT signal.
     * Typically, in the Alert Response Address callback function.
     */
    MTB_PMBUS_SMBALERT_MODE_MANUAL = 0U,
    /** The middleware automatically clears SMBALERT signal
     * after receiving Alert Response Address.
     */
    MTB_PMBUS_SMBALERT_MODE_AUTO   = 1U
} mtb_pmbus_smbalert_mode_t;
#endif /* defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U) */
/** General events for PMBus Middleware */
typedef enum
{
    /** Quick command event with Read bit */
    MTB_PMBUS_QUICK_CMD_RD_EVENT       = 0U,

    /** Quick command event with Write bit */
    MTB_PMBUS_QUICK_CMD_WR_EVENT       = 1U,

    /** Receive byte is requested by the controller.
     * Use this event to update data byte before sending.
     * This event is triggered in two cases: when the controller
     * requests the Receive byte or when the controller sends Quick
     * command with Read bit. Use \ref MTB_PMBUS_RECEIVE_BYTE_CMPLT_EVENT
     * to determine if the controller requests Receive byte or
     * Quick command is executed.
     */
    MTB_PMBUS_RECEIVE_BYTE_EVENT       = 2U,

    /** Receive byte protocol is executed */
    MTB_PMBUS_RECEIVE_BYTE_CMPLT_EVENT = 3U,

    /** The target successfully sent its own address to Host.
     * This event occurs only when SMBALERT signal is pulled down
     * by this Target Device. If SMBALERT mode is manual ( \ref MTB_PMBUS_SMBALERT_MODE_MANUAL),
     * use this event to stop pulling down of SMBALERT signal.
     */
    MTB_PMBUS_ALERT_RESPONSE_ADDR_EVENT = 4U,
} mtb_pmbus_events_t;

/** Command specific events */
typedef enum
{
    /** Command match event. Inside this event, the application can
     * protect or disable the command. In this case, the middleware sends
     * NACK after receiving the command code.
     */
    MTB_PMBUS_CMD_MATCH      = 0U,
    /** The controller sends a data byte. Using this event,
     * the application may decide whether ACK or NACK will be sent
     * after receiving the byte.
     */
    MTB_PMBUS_CMD_WRITE_BYTE = 1U,
    /** Write is completed. The application can read the received data 
     * inside the callback by \ref mtb_pmbus_cmd_read_data_isr
     * or \ref mtb_pmbus_cmd_read_data_ext_isr. For complex
     * protocols, like Group Protocol, it is recommended to apply the
     * received value only when the \ref MTB_PMBUS_CMD_DONE event occurs on
     * the bus.
     */
    MTB_PMBUS_CMD_WRITE_DONE = 2U,
    /** Read data is requested. The application can update the data
     * before sending it to the controller inside this callback by
     * \ref mtb_pmbus_cmd_update_data_isr or \ref mtb_pmbus_cmd_update_data_ext_isr
     */
    MTB_PMBUS_CMD_READ_REQ   = 3U,
    /** Read is completed. This event does not mean that transfer is completed.
     * When complex protocols are initiated on the bus, like Zone Read, the completion
     * of read request does not mean that the controller completes the transfer, so
     * it is recommended to consider the data receiving event on the controller side
     * only when \ref MTB_PMBUS_CMD_DONE is occurred.
     */
    MTB_PMBUS_CMD_READ_DONE  = 4U,
    /** The transfer for command is completed, STOP event occurs on the Bus.
     * The application can perform any actions over a command including enabling or
     * disabling all commands. Before this event, the middleware clears the active transfer
     * status.
     */
    MTB_PMBUS_CMD_DONE       = 5U,
    /** The command arbitration is lost. This event occurs when the target
     * loses arbitration during a command transfer.
     */
    MTB_PMBUS_CMD_ARB_LOST   = 6U
} mtb_pmbus_cmd_events_t;

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) || defined (MTB_PMBUS_DOXYGEN)
/** Zone events 
 * \note This enum is only available when \ref MTB_PMBUS_SUPPORT_ZONE is enabled at compile time options.
 */
typedef enum
{
    /** Zone Read protocol is completed */
    MTB_PMBUS_ZONE_READ_STATUS_CMPLT    = 0U,
    /** The Status byte is requested */
    MTB_PMBUS_ZONE_READ_STATUS_BYTE_REQ = 1U,
    /** The Word byte is requested */
    MTB_PMBUS_ZONE_READ_STATUS_WORD_REQ = 2U,
#if (defined(MTB_PMBUS_ZONE_SEL_ADV) && (MTB_PMBUS_ZONE_SEL_ADV != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** The controller assigns the new read zone for the whole instance or specific page.
     * The requested zone value can be accepted or rejected by setting the return value
     * for the callback: 0U - NACK is sent after receiving the requested value, 1U - ACK is sent after receiving the requested
     * value.
     * \note This enum value is only available when \ref MTB_PMBUS_ZONE_SEL_ADV is enabled at compile time options.
     */
    MTB_PMBUS_ZONE_CONFIG_RD = 3U,
    /** The controller assigns a new write zone for the whole instance or specific page.
     * The requested zone value can be accepted or rejected by setting the return value
     * for callback: 0U - NACK is sent after receiving the the requested value, 1U - ACK is sent after receiving the requested
     * value.
     * \note This enum value is only available when \ref MTB_PMBUS_ZONE_SEL_ADV is enabled at compile time options.
     */
    MTB_PMBUS_ZONE_CONFIG_WR = 4U,
    /** The controller selects the new active read zone.
     * The requested zone value can be accepted or rejected by setting the return value
     * for callback: 0U - the NACK is sent after receiving the requested value, 1U - the ACK is sent after receiving the requested
     * value.
     * \note This enum value is only available when \ref MTB_PMBUS_ZONE_SEL_ADV is enabled at compile time options.
     */
    MTB_PMBUS_ZONE_ACTIVE_RD = 5U,
    /** The controller selects the new active write zone.
     * The requested zone value can be accepted or rejected by setting the return value
     * for callback: 0U - NACK is sent after receiving the requested value, 1U - ACK is sent after receiving the requested
     * value.
     * \note This enum value is only available when \ref MTB_PMBUS_ZONE_SEL_ADV is enabled at compile time options.
     */
    MTB_PMBUS_ZONE_ACTIVE_WR = 6U,
#endif /* #if (defined(MTB_PMBUS_ZONE_SEL_ADV) && (MTB_PMBUS_ZONE_SEL_ADV != 0U)) || defined (MTB_PMBUS_DOXYGEN) */
} mtb_pmbus_zone_events_t;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) || defined (MTB_PMBUS_DOXYGEN) */

/** I2C HW actions */
typedef enum
{
    /** Enable the I2C HW */
    MTB_PMBUS_HW_RESOURCES_ENABLE  = 0U,
    /** Disable the I2C HW */
    MTB_PMBUS_HW_RESOURCES_DISABLE = 1U,
} mtb_pmbus_hw_resources_ctrl_action_t;

#if (defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U)) || defined (MTB_PMBUS_DOXYGEN)
/** Revision of the PMBus specification 
 * \note This enum is only available when \ref MTB_PMBUS_IMPL_CMD_REVISION is enabled at compile time options.
 */
typedef enum
{
    /** PMBus revision 1.4 */
    MTB_PMBUS_REVISION_1_4 = 0U,
} mtb_pmbus_revision_t;
#endif /* #if (defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U)) || defined (MTB_PMBUS_DOXYGEN) */

#if (defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U)) || defined (MTB_PMBUS_DOXYGEN)
/** The Bus Speed 
 * \note This enum is only available when \ref MTB_PMBUS_IMPL_CMD_CAPABILITY is enabled at compile time options.
 */
typedef enum
{
    /** The maximum supported bus speed is 100 kHz */
    MTB_PMBUS_SPEED_100  = 0U,
    /** The maximum supported bus speed is 400 kHz */
    MTB_PMBUS_SPEED_400  = 1U,
    /** The maximum supported bus speed is 1 MHz */
    MTB_PMBUS_SPEED_1000 = 2U,
} mtb_pmbus_speed_t;
#endif /* #if (defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U)) || defined (MTB_PMBUS_DOXYGEN) */
/** \} group_pmbus_enums */

/**
* \addtogroup group_pmbus_data_structs
* \{
*/

/** Provides the typedef for the callback function called in ISR to
 * notify the user about occurrences of \ref mtb_pmbus_events_t
 */
typedef void (* mtb_pmbus_handle_events_t)(mtb_pmbus_events_t event);

/** Provides the typedef for the callback function called in ISR to
 * notify the user about the occurrences of error events
 */
typedef void (* mtb_pmbus_handle_error_events_t)(uint32_t events, uint8_t cmd_code, bool cmd_is_ext);

/** Provides the typedef for the callback function called in ISR to
 * notify the user about occurrences of \ref mtb_pmbus_events_t
 */
typedef bool (* mtb_pmbus_handle_cmd_events_t)(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) || defined (MTB_PMBUS_DOXYGEN)
/** Provides the typedef for the callback function called in ISR to
 * notify the user about occurrences of \ref mtb_pmbus_zone_events_t
 */
typedef uint8_t (* mtb_pmbus_handle_zone_events_t)(mtb_pmbus_zone_events_t event, uint8_t data_byte, int32_t page);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) || defined (MTB_PMBUS_DOXYGEN) */

/** Provides the typedef for the callback function called in
 * \ref mtb_pmbus_enable and \ref mtb_pmbus_disable to perform
 * enabling/disabling HW resources.
 */
typedef void (* mtb_pmbus_hw_resources_ctrl_t)(mtb_pmbus_hw_resources_ctrl_action_t action);

/** Provides the typedef for the callback function called inside
 * the middleware to enable/disable PMBus related interrupts.
 */
typedef void (* mtb_pmbus_hw_isr_ctrl_t)(void);

/** Command configuration structure */
typedef struct
{
    /** Command code */
    uint8_t cmd_code;
    /** Command capabilities, see \ref group_pmbus_macro_cmd_cap */
    uint16_t cmd_cap;
    /** The pointer to command data buffer */
    void * data_buf;
    /** The command data size */
    uint8_t data_size;
    /** Command callback */
    mtb_pmbus_handle_cmd_events_t callback;
} mtb_pmbus_stc_config_cmd_t;

/** Hardware configuration structure */
typedef struct
{
    /** The pointer to HAL configuration structure, see \ref mtb_pmbus_stc_config_hal_t */
    mtb_pmbus_stc_config_hal_t * hal_config;
    /** The pointer to the user callback for enabling/disabling the I2C Hardware */
    mtb_pmbus_hw_resources_ctrl_t hw_resource_ctrl_callback;
    /** The pointer to the user callback for enabling I2C HW interrupts */
    mtb_pmbus_hw_isr_ctrl_t enable_hw_irq_callback;
    /** The pointer to the user callback for disabling I2C HW interrupts */
    mtb_pmbus_hw_isr_ctrl_t disable_hw_irq_callback;
} mtb_pmbus_stc_config_hw_t;

/** Configuration structure */
typedef struct
{
    /** Pointer to hardware configuration structure */
    mtb_pmbus_stc_config_hw_t * hw_config;

    /** Target address */
    uint8_t address;

#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable PEC support.
     * - With PEC enabled, the middleware can handle the protocols with or
     * without PEC.
     * \note If the received and calculated PEC bytes do not match, the
     * middleware sends NACK to PEC byte
     * - With PEC disabled, the middleware supports only protocols without PEC.
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_PEC is enabled at compile time options.
     */
    bool enable_pec;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) || defined (MTB_PMBUS_DOXYGEN) */
#if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable SMBALERT signal 
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_SMBALERT is enabled at compile time options.
     */
    bool enable_smbalert;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) || defined (MTB_PMBUS_DOXYGEN) */
#if (defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable General Call address handling 
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_GEN_CALL_ADDR is enabled at compile time options.
     */
    bool enable_gen_call_addr;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_GEN_CALL_ADDR) && (MTB_PMBUS_SUPPORT_GEN_CALL_ADDR != 0U)) || defined (MTB_PMBUS_DOXYGEN) */
#if (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U))
    /** Enable PMBus support. If this option is disabled, only SMBUS features are supported 
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_PMBUS is enabled at compile time options.
     */
    bool enable_pmbus;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PMBUS) && (MTB_PMBUS_SUPPORT_PMBUS != 0U)) */
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** The total number of pages. If set to 0, the pages are not supported 
     * \note This field is only available when \ref MTB_PMBUS_PAGES_NUM is greater than 0U at compile time options.
     */
    uint8_t num_pages;
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) || defined (MTB_PMBUS_DOXYGEN) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) || defined (MTB_PMBUS_DOXYGEN) 
    /** The total number of phases. If set to 0, the phases are not supported 
     * \note This field is only available when \ref MTB_PMBUS_PHASES_NUM is greater than 0U at compile time options.
     */
    uint8_t num_phases;
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM != 0U)) || defined (MTB_PMBUS_DOXYGEN) */
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** Enable Zone Write and Zone Read protocols 
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_ZONE is enabled at compile time options.
     */
    bool enable_zone;

    /** The pointer to the user callback for zone protocol 
     * \note This field is only available when \ref MTB_PMBUS_SUPPORT_ZONE is enabled at compile time options.
     */
    mtb_pmbus_handle_zone_events_t zone_callback;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) || defined (MTB_PMBUS_DOXYGEN) */

#if (MTB_PMBUS_IMPL_CMD_NUM != 0U) || defined (MTB_PMBUS_DOXYGEN)
    /** List of implemented commands
     *
     * Use this field to define the list of the implemented commands
     * \ref group_pmbus_macro_impl_cmd
     * \note This field is only available when at least one preimplemented command is enabled at compile time options.
     */
    uint32_t impl_cmd_mask;
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM != 0U) || defined (MTB_PMBUS_DOXYGEN) */
    /** The pointer to commands table */
    mtb_pmbus_stc_config_cmd_t * cmd_table;
    /** Number of defined commands in commands table  */
    uint8_t cmd_num;

    /** The pointer to the user callback for general events */
    mtb_pmbus_handle_events_t gen_callback;

    /** The pointer to the user callback for error events */
    mtb_pmbus_handle_error_events_t errors_callback;

#if (defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** Revision of PMBus specification 
     * \note This field is only available when \ref MTB_PMBUS_IMPL_CMD_REVISION is enabled at compile time options.
     */
    mtb_pmbus_revision_t revision;
#endif /* #if (defined(MTB_PMBUS_IMPL_CMD_REVISION) && (MTB_PMBUS_IMPL_CMD_REVISION != 0U)) || defined (MTB_PMBUS_DOXYGEN) */
#if (defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U)) || defined (MTB_PMBUS_DOXYGEN)
    /** The maximum supported speed 
     * \note This field is only available when \ref MTB_PMBUS_IMPL_CMD_CAPABILITY is enabled at compile time options.
     */
    mtb_pmbus_speed_t speed;
    /** If enabled the Numeric data is in IEEE Half Precision
     * Floating Point Format, otherwise in LINEAR11, ULINEAR16 or
     * DIRECT formats
     * \note This field is only available when \ref MTB_PMBUS_IMPL_CMD_CAPABILITY is enabled at compile time options.
     */
    bool enable_ieee_format;
#endif /* #if (defined(MTB_PMBUS_IMPL_CMD_CAPABILITY) && (MTB_PMBUS_IMPL_CMD_CAPABILITY != 0U)) || defined (MTB_PMBUS_DOXYGEN) */
} mtb_pmbus_stc_config_t;

/** \cond INTERNAL */
typedef struct
{
    /** Command position in command table */
    uint8_t cmd_pos;
    /** Command flags */
    uint8_t flags;
} mtb_pmbus_cmd_lookup_tbl_t;
/** \endcond*/

/** Instance structure */
typedef struct
{
    /** \cond INTERNAL */
    mtb_pmbus_stc_config_t const * cfg;
    mtb_pmbus_cmd_lookup_tbl_t cmd_lookup_tbl[MTB_PMBUS_CMD_MAX_NUM];
    /*  Command table and look up table for implemented commands */
#if (MTB_PMBUS_IMPL_CMD_NUM > 0U)
    mtb_pmbus_stc_config_cmd_t pre_impl_cmd_table[MTB_PMBUS_IMPL_CMD_NUM];
    mtb_pmbus_cmd_lookup_tbl_t pre_impl_cmd_lookup_tbl[MTB_PMBUS_IMPL_CMD_NUM];
#endif /* #if (MTB_PMBUS_IMPL_CMD_NUM > 0U) */
    uint32_t state;
    uint32_t errors;
    uint16_t byte_received;
    uint16_t byte_requested; /* Applicable only for Block commands */
    uint16_t byte_sent;
    uint16_t byte_to_send;
    uint8_t cmd_pos;
    uint8_t cmd_code;
    uint16_t cmd_code_full;
#if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U))
    uint8_t crc;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_PEC) && (MTB_PMBUS_SUPPORT_PEC != 0U)) */
    uint8_t int_buff[MTB_PMBUS_CMD_MAX_SIZE + 1U]; /* Add additional byte for Block commands */
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM > 0U))
    uint8_t act_page;
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM > 0U)) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM > 0U))
    uint8_t act_phase;
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM > 0U)) */
#if ((defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)))
    uint8_t stored_page;
#endif /* #if ((defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))) */
#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U))
    uint8_t read_zones[MTB_PMBUS_PAGES_NUM]; 
    uint8_t write_zones[MTB_PMBUS_PAGES_NUM];
#else /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
    uint8_t read_zones[1U];
    uint8_t write_zones[1U];
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM != 0U)) */
    uint8_t act_read_zone;
    uint8_t act_write_zone;
    uint8_t cmd_ctrl_byte;
    uint8_t zone_sts_mask;
    uint8_t zone_pages;
    uint8_t num_available_zone;
    bool zone_is_read;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */
    bool    is_cmd_block;
    mtb_pmbus_stc_config_cmd_t * active_cmd_tbl;
    mtb_pmbus_cmd_lookup_tbl_t * active_lookup_tbl;
    /* Byte for Received byte protocol */
    uint8_t received_byte;
#if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U))
    mtb_pmbus_smbalert_mode_t smbalert_mode;
    bool smbalert_is_trig;
#endif /* #if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) */
    /** \endcond */
} mtb_pmbus_stc_t;

/** \} group_pmbus_data_structs */

/**
* \addtogroup group_pmbus_functions
* \{
*/

/**
* \defgroup group_pmbus_init_func Initialization Functions
* \{
*/

/**
 * @brief Initialize the PMBus Middleware
 * 
 * @param inst  The pointer to the PMBus instance structure
 * @param config The pointer to PMBus configuration structure
 * @return \ref mtb_pmbus_status_t 
 */
mtb_pmbus_status_t mtb_pmbus_init(mtb_pmbus_stc_t * inst, mtb_pmbus_stc_config_t const * config);

/**
 * @brief Enable PMBus Middleware
 *
 * After calling of this function Middleware starts to response on Controller transfers
 *
 * @param inst The pointer to the PMBus instance structure
 * @return \ref mtb_pmbus_status_t 
 */
mtb_pmbus_status_t mtb_pmbus_enable(mtb_pmbus_stc_t * inst);

/**
 * @brief Disable PMBus Middleware
 *
 * After calling of this function Middleware stops to response on Controller transfers
 *
 * @param inst The pointer to the PMBus instance structure
 * @return \ref mtb_pmbus_status_t 
 */
mtb_pmbus_status_t mtb_pmbus_disable(mtb_pmbus_stc_t * inst);

/** \} group_pmbus_init_func */

/**
* \defgroup group_pmbus_cmd_func Command Handler Functions
* A variety of functions are provided to work with commands:
* - Update or read the command data.
* - Retrieve the last transfer size (applicable for Block protocols).
* - Enable or disable commands.
* - Protect commands against write operations.
*
* In this section, functions with the `isr` suffix are optimized for use during ISRs,
* typically in command callback functions. APIs without the `isr` suffix return the
* \ref MTB_PMBUS_STATUS_CMD_IS_ACTIVE status if the selected command is participating
* in an active transfer.
* To access data for specific pages or phases of commands, use APIs with the `ext` suffix.
* \{
*/

/**
 * @brief Update the data buffer for the selected command with new data.
 *
 * For the Block Read and Block Write-Block Read Process Call protocols:
 * If the updated data size is smaller than the command size in the command
 * table, only updated bytes will be transferred.
 *
 * This function always updates data from the 0 position.
 *
 * The function validates the input parameters and returns the appropriate error status.
 *
 * The function prevents updating the data content if the command is active, to avoid data
 * corruption. Therefore, using this function within a PMBus callback (executed inside an ISR)
 * will have no effect.
 *
 * Use this function only for not paged/phased commands.
 *
 * \warning Updating the command buffer is not an atomic operation and does not occur
 * within a critical section. Therefore, in projects where the command buffer is updated
 * from multiple execution contexts, it is the user's responsibility to ensure the proper
 * synchronization and update order.
 * 
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param data The pointer to the data array
 * @param data_size The size of the data array
 *
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_update_data(mtb_pmbus_stc_t * inst, uint32_t code, uint8_t * data, uint32_t data_size);

/**
 * @brief Update the data buffer for the selected command with new data.
 *
 * This function is optimized for use during ISR and is recommended to be used in a command callback.
 * The function skips validation of input parameters and does not check if the command is currently
 * being updated or read by the controller.
 *
 * For the Block Read and Block Write-Block Read Process Call protocols:
 * If the updated data size is smaller than the command size in the command
 * table, only the updated bytes will be transferred.
 *
 * This function always updates data from position 0.
 *
 * Use this function only for non-paged/non-phased commands.
 *
 * \warning The updating data for active command can lead to data corruption.
 * \warning Updating the command buffer is not an atomic operation and does not occur
 * within a critical section. Therefore, in projects where the command buffer is updated
 * from multiple execution contexts, it is the user's responsibility to ensure the proper
 * synchronization and update order.
 *
 * @param inst      Pointer to the PMBus instance structure
 * @param code      Command code
 * @param data      Pointer to the data array
 * @param data_size Size of the data array
 */
void mtb_pmbus_cmd_update_data_isr(mtb_pmbus_stc_t * inst, uint32_t code, uint8_t * data, uint32_t data_size);

/**
 * @brief Update the data buffer for the selected command, page and phase with the new data.
 *
 * For the Block Read and Block Write-Block Read Process Call protocols:
 * If the updated data size is smaller than the command size in the command
 * table, only updated bytes will be transferred.
 *
 * This function always updates data from the 0 position.
 *
* \warning Updating the command buffer is not an atomic operation and does not occur
 * within a critical section. Therefore, in projects where the command buffer is updated
 * from multiple execution contexts, it is the user's responsibility to ensure the proper
 * synchronization and update order.
 * 
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param page  The page number to update. If the command is not paged, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param phase  The phase number to update. If the command is not phased, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param data The pointer to the data array
 * @param data_size The size of the data array
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_update_data_ext(mtb_pmbus_stc_t * inst, uint32_t code, int32_t page, int32_t phase, uint8_t * data, uint32_t data_size);

/**
 * @brief Update the data buffer for the selected command, page and phase with the new data.
 *
 * This function is optimized for use during ISR and is recommended to be used in a command callback.
 * The function skips validation of input parameters and does not check if the command is currently
 * being updated or read by the controller.
 *
 * For the Block Read and Block Write-Block Read Process Call protocols:
 * If the updated data size is smaller than the command size in the command
 * table, only updated bytes will be transferred.
 *
 * This function always update data from the 0 position.
 *
 * The function validates the input parameters and returns the appropriate error status.
 *
 * The function prevents updating the data content if the command is active, to avoid data
 * corruption. Therefore, using this function within a PMBus callback (executed inside an ISR)
 * will have no effect.
 *
 * \warning The updating data for active command can lead to data corruption.
 * \warning Updating the command buffer is not an atomic operation and does not occur
 * within a critical section. Therefore, in projects where the command buffer is updated
 * from multiple execution contexts, it is the user's responsibility to ensure the proper
 * synchronization and update order.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param page  The page number to update. If the command is not paged, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param phase  The phase number to update. If the command is not phased, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param data The pointer to the data array
 * @param data_size The size of the data array
 */
void mtb_pmbus_cmd_update_data_ext_isr(mtb_pmbus_stc_t * inst, uint32_t code, int32_t page, int32_t phase, uint8_t * data, uint32_t data_size);

/**
 * @brief Read the data from the internal buffer for selected command
 *
 * The function validates the input parameters and returns the appropriate error status.
 *
 * The function prevents reading the data content if the command is active, to avoid data
 * corruption. Therefore, using this function within a PMBus callback (executed inside an ISR)
 * will have no effect.
 *
 * \warning Reading the command buffer is not an atomic operation and does not occur
 * within a critical section. Therefore, in projects where the command buffer is accessed
 * from multiple execution contexts, it is the user's responsibility to ensure the proper
 * synchronization and access order.
 * 
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param data The pointer to the data array
 * @param data_size The size of the data array
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_read_data(mtb_pmbus_stc_t * inst, uint32_t code, uint8_t * data, uint32_t data_size);

/**
 * @brief Read the data from the internal buffer for selected command
 * 
 * This function is optimized for use during ISR and is recommended to be used in a command callback.
 * The function skips validation of input parameters and does not check if the command is currently
 * being updated or read by the controller.
 *
 * \warning The returned data for command which take part in active transfer can be
 * invalid.
 * \warning Reading the command buffer is not an atomic operation and does not occur
 * within a critical section. Therefore, in projects where the command buffer is accessed
 * from multiple execution contexts, it is the user's responsibility to ensure the proper
 * synchronization and access order.
 * 
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param data The pointer to the data array
 * @param data_size The size of the data array
 */
void mtb_pmbus_cmd_read_data_isr(mtb_pmbus_stc_t * inst, uint32_t code, uint8_t * data, uint32_t data_size);

/**
 * @brief Read the data from the buffer for the selected command, page and phase.
 *
 * The function validates the input parameters and returns the appropriate error status.
 *
 * The function prevents reading the data content if the command is active, to avoid data
 * corruption. Therefore, using this function within a PMBus callback (executed inside an ISR)
 * will have no effect.
 * 
 * \warning Reading the command buffer is not an atomic operation and does not occur
 * within a critical section. Therefore, in projects where the command buffer is accessed
 * from multiple execution contexts, it is the user's responsibility to ensure the proper
 * synchronization and access order.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param page The page number to update. If the command is not paged, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param phase  The phase number to update. If the command is not phased, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param data The pointer to the data array
 * @param data_size The size of the data array
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_read_data_ext(mtb_pmbus_stc_t * inst, uint32_t code, int32_t page, int32_t phase, uint8_t * data, uint32_t data_size);

/**
 * @brief Read the data from the buffer for the selected command, page and phase.
 *
 * This function is optimized for use during ISR and is recommended to be used in a command callback.
 * The function skips validation of input parameters and does not check if the command is currently
 * being updated or read by the controller.
 *
 * \warning The returned data for command which take part in active transfer can be
 * invalid.
 * \warning Reading the command buffer is not an atomic operation and does not occur
 * within a critical section. Therefore, in projects where the command buffer is accessed
 * from multiple execution contexts, it is the user's responsibility to ensure the proper
 * synchronization and access order.
 * 
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param page  The page number to update. If the command is not paged, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param phase  The phase number to update. If the command is not phased, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param data The pointer to the data array
 * @param data_size The size of the data array
 */
void mtb_pmbus_cmd_read_data_ext_isr(mtb_pmbus_stc_t * inst, uint32_t code, int32_t page, int32_t phase, uint8_t * data, uint32_t data_size);

/**
 * @brief Returns the number of bytes which is transferred during last transaction.
 *
 * For majority of protocol the size of read/write data equal to command size,
 * but for the Block Write-Block Read Process Call, Block Write, Block Read protocols,
 * the size of transfer can be smaller than the maximum size of command in command table.
 *
 * The function validates the input parameters and returns the appropriate error status.
 *
 * The function prevents reading the transfer size if the command is active, to avoid data
 * corruption. Therefore, using this function within a PMBus callback (executed inside an ISR)
 * will have no effect.
 * 
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param size The size of the last transaction
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_get_transfer_size(mtb_pmbus_stc_t * inst, uint32_t code, uint8_t * size);

/**
 * @brief Returns the number of bytes which is transferred during last transaction.
 * 
 * This function is optimized for use during ISR and is recommended to be used in a command callback.
 * The function skips validation of input parameters and does not check if the command is currently
 * being updated or read by the controller.
 *
 * For the majority of protocols, the size of read/write data is equal to the command size,
 * but for the Block Write-Block Read Process Call, Block Write, Block Read protocols,
 * the size of transfer can be smaller than the maximum size of a command in the command table.
 *
 * \warning The returned transfer size for command which takes part in active transfer can be
 * invalid.
 * 
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param size The size of the last transaction
 */
void mtb_pmbus_cmd_get_transfer_size_isr(mtb_pmbus_stc_t * inst, uint32_t code, uint8_t * size);

/**
 * @brief Returns the number of bytes which is transferred during last transaction.
 * 
 * For the majority of protocols, the size of read/write data is equal to the command size,
 * but for the Block Write-Block Read Process Call, Block Write, Block Read protocols,
 * the size of transfer can be smaller than the maximum size of command in command table.
 *
 * The function validates the input parameters and returns the appropriate error status.
 *
 * The function prevents reading the transfer size if the command is active, to avoid data
 * corruption. Therefore, using this function within a PMBus callback (executed inside an ISR)
 * will have no effect.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param page  The page number to update. If the command is not paged, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param phase  The phase number to update. If the command is not phased, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param size The pointer to transfer size
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_get_transfer_size_ext(mtb_pmbus_stc_t * inst, uint32_t code, int32_t page, int32_t phase, uint8_t * size);

/**
 * @brief Returns the number of bytes which is transferred during last transaction.
 *
 * This function is optimized for use during ISR and is recommended to be used in a command callback.
 * The function skips validation of input parameters and does not check if the command is currently
 * being updated or read by the controller.
 * 
 * For the majority of protocols, the size of read/write data is equal to the command size,
 * but for the Block Write-Block Read Process Call, Block Write, Block Read protocols,
 * the size of transfer can be smaller than the maximum size of a command in the command table.
 *
 * \warning The returned transfer size for command which takes part in active transfer can be
 * invalid.
 * 
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param page The page number to update. If the command is not paged, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param phase  The phase number to update. If the command is not phased, then use
 * \ref MTB_PMBUS_NO_PAGE_PHASE as input parameters
 * @param size The pointer to transfer size
 */
void mtb_pmbus_cmd_get_transfer_size_ext_isr(mtb_pmbus_stc_t * inst, uint32_t code, int32_t page, int32_t phase, uint8_t * size);

/**
 * @brief Returns true if command participate in current transfer, otherwise returns false
 *
 * This function is optimized for use during ISR and is recommended to be used in a command callback.
 * The function skips validation of input parameters and does not check if the command is currently
 * being updated or read by the controller.
 * 
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param status True if command is active, otherwise false
 */
void mtb_pmbus_cmd_is_active_isr(mtb_pmbus_stc_t * inst, uint32_t code, bool * status);

/**
 * @brief Returns true if command participate in current transfer, otherwise returns false
 *
 * The function validates the input parameters and returns the appropriate error status.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param status True if command is active, otherwise false
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_is_active(mtb_pmbus_stc_t * inst, uint32_t code, bool * status);

/**
 * @brief Enable/Disable command. If the command is enabled, the PMBus Middleware sends
 * ACK bit after receiving the command code and transmits or receives data, otherwise the PMBus sends
 * NACK after receiving the command code.
 *
 * The function can be used to enable/disable commands from command table -
 * main ( \ref mtb_pmbus_stc_config_t::cmd_table).
 *
 * This function is optimized for use during ISR and is recommended to be used in a command callback.
 * The function skips validation of input parameters and does not check if the command is currently
 * being updated or read by the controller.
 *
 * \warning Enabling/Disabling the command which take part in active transfer can lead
 * to unexpected behavior.
 * 
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param status Set to true to enable command, set to false to disable command
 */
void mtb_pmbus_cmd_enable_disable_isr(mtb_pmbus_stc_t * inst, uint32_t code, bool status);

/**
 * @brief Enable/Disable command. If the command is enabled, the PMBus Middleware sends
 * ACK bit after receiving the command code and transmits or receives data, otherwise the PMBus sends
 * NACK after receiving the command code.
 *
 * The function can be used to enable/disable commands from command table -
 * main ( \ref mtb_pmbus_stc_config_t::cmd_table).
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param status Set to true to enable command, set to false to disable command
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_enable_disable(mtb_pmbus_stc_t * inst, uint32_t code, bool status);

/**
 * @brief Enable/Disable all commands from command table - main ( \ref mtb_pmbus_stc_config_t::cmd_table).
 * If the command is enabled, the PMBus middleware
 * sends ACK bit after receiving the command code and transmits or receives data, otherwise the PMBus sends
 * NACK after receiving the command code.
 *
 * The function prevents enabling/disabling the commands if PMBus instance takes
 * part in communication.
 * 
 * @param inst The pointer to the PMBus instance structure
 * @param status Set to true to enable all the commands, set to false to disable all the commands
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_all_enable_disable(mtb_pmbus_stc_t * inst, bool status);

/**
 * @brief Returns true if a command is executed at least one time.
 *
 * Returns true if a command is executed at least one time (The controller executes transaction
 * for this command and transaction is completed). Returns false if a command never executed after
 * enabling of middleware or after the last calling of \ref mtb_pmbus_cmd_get_status() for this command.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code
 * @param status Status of command execution
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_get_status(mtb_pmbus_stc_t * inst, uint32_t code, bool * status);

/**
 * @brief Enable or disable write protection for a specific command.
 *
 * When write protection is enabled (status = true), the PMBus middleware
 * will NACK any write attempts to the specified command code.
 * When disabled (status = false), normal write access is allowed.
 * 
 * \warning Enabling/Disabling write protection for the command which take part in active transfer can lead
 * to unexpected behavior.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code for target command
 * @param status Set to true to enable write protection, false to disable
 */
void mtb_pmbus_cmd_wr_protect_isr(mtb_pmbus_stc_t * inst, uint32_t code, bool status);

/**
 * @brief Enable or disable write protection for a specific command.
 *
 * When write protection is enabled (status = true), the PMBus middleware
 * will NACK any write attempts to the specified command code.
 * When disabled (status = false), normal write access is allowed.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param code The command code for target command
 * @param status Set to true to enable write protection, false to disable
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_wr_protect(mtb_pmbus_stc_t * inst, uint32_t code, bool status);

/**
 * @brief Enables or disables write protection for all PMBus commands.
 *
 * This function sets the write protection status for all commands in the PMBus instance.
 *
 * The function prevents enabling/disabling the commands protection if PMBus instance takes
 * part in communication.
 *
 * @param inst   The pointer to the PMBus instance structure.
 * @param status Set to true to enable write protection, false to disable.
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_cmd_all_wr_protect(mtb_pmbus_stc_t * inst, bool status);

/** \} group_pmbus_cmd_func */

/**
* \defgroup group_pmbus_zone_func Zone Functions
* \{
*/

#if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U))
/**
 * @brief Returns assigned Read Zone to this target device
 *
 * If the instance does not support pages, then the @p page parameter is ignored
 * and the function returns the global zones assigned to this instance.
 *
 * \note This function is only available when \ref MTB_PMBUS_SUPPORT_ZONE is enabled at compile time options.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param zone The pointer to variable for storing the Read Zone
 * @param page The page number for zone assignment
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_get_read_zone(mtb_pmbus_stc_t * inst, uint32_t * zone, uint32_t page);

/**
 * @brief Returns assigned Write Zone to this target device
 *
 * If the instance does not support pages, then the @p page parameter is ignored
 * and the function returns the global zones assigned to this instance.
 *
 * \note This function is only available when \ref MTB_PMBUS_SUPPORT_ZONE is enabled at compile time options.
 *
 * @param inst  The pointer to the PMBus instance structure
 * @param zone The pointer to variable for storing the Write Zone
 * @param page The page number for zone assignment
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_get_write_zone(mtb_pmbus_stc_t * inst, uint32_t * zone, uint32_t page);

/**
 * @brief Returns the active zones
 * 
 * \note This function is only available when \ref MTB_PMBUS_SUPPORT_ZONE is enabled at compile time options.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param zone_read The pointer to variable for storing the active Read Zone
 * @param zone_write The pointer to variable for storing the active Write Zone
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_get_active_zones(mtb_pmbus_stc_t * inst, uint32_t * zone_read, uint32_t * zone_write);

/**
 * @brief Sets the default zones for the device. Use this function to change the default zones
 * during the initialization of the device: after \ref mtb_pmbus_init() and before \ref mtb_pmbus_enable().
 * 
 * By default the 0xFE is assigned to Read and Write Zones during initialization stage.
 *
 * If instance does not support pages, then the @p page parameter value is ignored
 * and zones are assigned globally for instance.
 *
 * \note This function is only available when \ref MTB_PMBUS_SUPPORT_ZONE is enabled at compile time options.
 *
 * \warning This function does not check for invalid zone values. This is done for flexibility purposes,
 * as there are several ranges of zone values.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param zone_read The Read Zone to assign to this device
 * @param zone_write The Write Zone to assign to this device
 * @param page The page number for zone assignment
 * @return \ref mtb_pmbus_status_t
 */
mtb_pmbus_status_t mtb_pmbus_set_default_zones(mtb_pmbus_stc_t * inst, uint32_t zone_read, uint32_t zone_write, uint32_t page);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_ZONE) && (MTB_PMBUS_SUPPORT_ZONE != 0U)) */

/** \} group_pmbus_zone_func */

/**
* \defgroup group_pmbus_page_phase_func Page/Phase Functions
* \{
*/
#if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM > 0U)) || defined (MTB_PMBUS_DOXYGEN)
/**
 * @brief Returns the active page number
 * 
 * \note This function is only available when \ref MTB_PMBUS_PAGES_NUM is greater than 0U at compile time options.
 *
 * @param inst The pointer to the PMBus instance structure
 * @return uint8_t The active page number
 */
uint8_t mtb_pmbus_get_active_page(mtb_pmbus_stc_t * inst);
#endif /* #if (defined(MTB_PMBUS_PAGES_NUM) && (MTB_PMBUS_PAGES_NUM > 0U)) || defined (MTB_PMBUS_DOXYGEN) */
#if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM > 0U)) || defined (MTB_PMBUS_DOXYGEN)
/**
 * @brief Returns the active phase number
 * 
 * \note This function is only available when \ref MTB_PMBUS_PHASES_NUM is greater than 0U at compile time options.
 *
 * @param inst The pointer to the PMBus instance structure
 * @return uint8_t The active phase number
 */
uint8_t mtb_pmbus_get_active_phase(mtb_pmbus_stc_t * inst);
#endif /* #if (defined(MTB_PMBUS_PHASES_NUM) && (MTB_PMBUS_PHASES_NUM > 0U)) || defined (MTB_PMBUS_DOXYGEN) */
/** \} group_pmbus_page_phase_func */

/**
* \defgroup group_pmbus_smbalert_func SMBALERT Functions
* \{
*/

#if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) || defined (MTB_PMBUS_DOXYGEN)
/**
 * @brief Sets the mode for SMBALERT pin
 * 
 * \note This function is only available when \ref MTB_PMBUS_SUPPORT_SMBALERT is enabled at compile time options.
 *
 * @param inst The pointer to the PMBus instance structure
 * @param mode The selected mode, \ref mtb_pmbus_smbalert_mode_t
 */
void mtb_pmbus_smbalert_config_mode(mtb_pmbus_stc_t * inst, mtb_pmbus_smbalert_mode_t mode);

/**
 * @brief Set low level for SMBALERT pin.
 *
 * Once the SMBALERT signal sets to 0, the Middleware starts to response on
 * Alert Response Address.
 *
 * This function does nothing if \ref mtb_pmbus_stc_config_t::enable_smbalert
 * is set to NULL.
 * 
 * \note This function is only available when \ref MTB_PMBUS_SUPPORT_SMBALERT is enabled at compile time options.
 *
 * @param inst The pointer to the PMBus instance structure
 */
void mtb_pmbus_smbalert_set_signal(mtb_pmbus_stc_t * inst);

/**
 * @brief Set hign level for SMBALERT pin
 *
 * This function does nothing if \ref mtb_pmbus_stc_config_t::enable_smbalert
 * is set to NULL.
 *
 * The middleware stops response on Alert Response Address once this function is
 * called. If the SMBALERT mode is \ref MTB_PMBUS_SMBALERT_MODE_AUTO, the middleware
 * automatic clears the SMBALERT signal, otherwise clear the signal
 * manually in \ref mtb_pmbus_handle_events_t callback when \ref MTB_PMBUS_ALERT_RESPONSE_ADDR_EVENT
 * is triggered.
 * 
 * \note This function is only available when \ref MTB_PMBUS_SUPPORT_SMBALERT is enabled at compile time options.
 *
 * @param inst The pointer to the PMBus instance structure
 */
void mtb_pmbus_smbalert_clear_signal(mtb_pmbus_stc_t * inst);
#endif /* #if (defined(MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) || defined (MTB_PMBUS_DOXYGEN) */
/** \} group_pmbus_smbalert_func */

/**
* \defgroup group_pmbus_miscellaneous_func Miscellaneous Functions
* \{
*/

/**
 * @brief Set the data byte for the Receive Byte protocol. The middleware will
 * automatically send this byte when it is requested by the controller.
 *
 * \note If the Quick Command with Read direction and Received Byte protocols
 * must be used in the same application, set the first bit of the data byte 
 * to 1. See \ref section_pmbus_protocols for more information.
 * 
 * @param inst The pointer to the PMBus instance structure
 * @param byte The byte to send
 */
void mtb_pmbus_set_received_byte(mtb_pmbus_stc_t * inst, uint8_t byte);

/**
 * @brief Checks if the PMBus instance is currently busy.
 *
 * This function determines whether the specified PMBus instance is engaged in communication.
 * The busy status is set once the PMBus instance receives any of the supported addresses (Own
 * Target Address, Alert Response Address, etc.) and cleared after a STOP condition. Use this
 * function together with \ref mtb_pmbus_disable to avoid disabling the PMBus instance during
 * active communication and to prevent bus errors.
 *
 * @param inst Pointer to the PMBus instance structure
 * @return true if the PMBus instance is busy; false otherwise.
 */
bool mtb_pmbus_is_busy(mtb_pmbus_stc_t * inst);

/** \} group_pmbus_miscellaneous_func */

/**
* \defgroup group_pmbus_helper_func Helper Functions
* \{
*/

/**
 * @brief Converts the argument "linear11" to floating point and returns it
 * 
 * @param linear_11 A number in LINEAR11 format
 * @return float32_t The linear11 parameter converted to floating point
 */
float32_t mtb_pmbus_lin11_to_float(uint16_t linear_11);

/**
 * @brief Takes the argument "floatvar" (a floating point number) and converts
 * it to a 16-bit LINEAR11 value (11-bit mantissa + 5-bit exponent), which it returns.
 * 
 * @param float_var  A floating point number
 * @return uint16_t float_var converted to LINEAR11
 *
 * @note Conversion to LINEAR11 may result in the loss of precision, because 
 * LINEAR11 uses less bits to represent the floating point number and mantissa is not normalized.
 */
uint16_t mtb_pmbus_float_to_lin11(float32_t float_var);

/**
 * @brief Converts the argument "linear16" to floating point and returns it
 * 
 * @param linear_16 The 16-bit mantissa of a LINEAR16 number
 * @param int_exp The 5-bit exponent of a LINEAR16 number. Packed in the
 * lower 5 bits. 2's Complement.
 * @return float32_t The parameters converted to floating point
 */
float32_t mtb_pmbus_lin16_to_float(uint16_t linear_16, int8_t int_exp);

/**
 * @brief Takes the argument "floatvar" (a floating point number) and converts
 * it to a 16-bit LINEAR16 value (16-bit mantissa), which it returns.
 * 
 * @param float_var A floating point number to be converted to LINEAR16
 * @param out_exp User provided 5-bit exponent to use in the conversion.
 * @return uint16_t The parameters converted to LINEAR16.
 *
 * @note Conversion to LINEAR16 may result in loss of precision, because 
 * LINEAR16 use less bits to represent the floating point number and mantissa is not normalized.
 * @note Providing mismatched exponent lead to incorrect conversion:
 * @code
 *      Incorrect conversion examples:
 *      float_var = 10.5f, out_exp = 0     -> 0x000B LIN16 mantissa -> 11.0f when converted back to float with the same exponent.
 *      float_var = 65535.0f, out_exp = -1 -> 0xFFFE LIN16 mantissa -> 32767.0f when converted back to float with the same exponent.
 *      Correct conversion examples:
 *      float_var = 10.5f, out_exp = -1    -> 0x0015 LIN16 mantissa -> 10.5f when converted back to float with the same exponent.
 *      float_var = 65535.0f, out_exp = 0  -> 0xFFFF LIN16 mantissa -> 65535.0f when converted back to float with the same exponent.
 * @endcode
 */
uint16_t mtb_pmbus_float_to_lin16(float32_t float_var, int8_t out_exp);

/** \} group_pmbus_helpers_func */

/**
* \defgroup group_pmbus_isr_func ISR Functions
* \{
*/

/**
 * @brief PMBus Target interrupt service routine
 *
 * This function should be called from the I2C interrupt handler
 *
 * @param inst The pointer to the PMBus instance structure
 */
void mtb_pmbus_i2c_isr(mtb_pmbus_stc_t * inst);

#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) || defined (MTB_PMBUS_DOXYGEN)
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) || defined (MTB_PMBUS_DOXYGEN)
/**
 * @brief PMBus Timer interrupt service routine
 *
 * This function should be called from the TCPWM interrupt handler
 *
 * \note This function is only available when \ref MTB_PMBUS_ENABLE_TIMEOUT
 * and \ref MTB_PMBUS_HAL_USE_TCPWM are enabled at compile time options.
 *
 * @param inst The pointer to the PMBus instance structure
 */
void mtb_pmbus_timer_isr(mtb_pmbus_stc_t * inst);
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) || defined (MTB_PMBUS_DOXYGEN) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) || defined (MTB_PMBUS_DOXYGEN) */

/** \} group_pmbus_isr_func */

/** \} group_pmbus_functions */


#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
void mtb_pmbus_handle_timeout(mtb_pmbus_stc_t * inst);
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */

#ifdef __cplusplus
}
#endif

#endif /* MTB_PMBUS_H */
