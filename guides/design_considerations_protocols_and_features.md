# Design Considerations: Protocols and Features

## Communication Protocols

The PMBus middleware supports multiple protocols defined in the SMBus and PMBus specifications. This section provides the specifics of the implementation for both Target and Controller modes.

### Target Mode

**Quick Command vs Received Byte Protocols:**

The Quick Command with the Read direction bit and the Received Byte protocol are identical during the Address stage of the transfer. As a result, the target device cannot distinguish between them in time, which may lead to an error condition on the bus.

> **Note:** After the last SCL clock for the ACK/NACK bit of the address part, the target starts to set the first bit of the Received Byte protocol. At the same time, if a Quick Command is initiated, the controller sets the SDA line to a low level to generate the STOP condition. Then, after setting the SCL line to a high level, the following cases can occur on the bus:
> - If the first bit of the Received Byte is 1, the controller wins arbitration and completes the transfer with a STOP condition. From the target's point of view, such a sequence of events on the bus is considered as arbitration lost.
> - If the first bit of the Received Byte is 0, the target continues holding the SDA line, which prevents the STOP event generation, and most likely a timeout error will be generated after 25 ms.

As a result, you can use Quick Command and Received Byte as follows:
- If only one protocol, either Quick Command or Received Byte, is used, no collision/errors occur on the bus.

> **Note:** If the Quick Command is initiated by the controller and the Received Byte is disabled in the middleware, the middleware sends the 0xFF byte after the address part of the transfer and ignores the arbitration lost event. Sending 0xFF allows the controller to win arbitration.

- No issue if only Quick Command with the Write direction and Received Byte are used.
- If both Quick Command with Read direction and Received Byte are used, the first bit of the Received Byte must always be set to 1 to allow the controller to win arbitration in the case of Quick Command.

> **Note:** For this case, the arbitration lost event is also ignored and the middleware does not report it to the application level.

**Zone Read and Write Protocols:**

The Middleware handles the Zone Read and Zone Write protocols, assigning new zones to the pages and tracking the new active zones on the bus automatically. Also, the Middleware cares about Command Control code and determines the type of actions (Status or Command), the byte ordering, bit swapping and respond mode. So, the commands handling is very similar to standard protocols. Also, to read/write command data, the Zone Read protocol can be used to request the status. The Middleware reports the requested status bits through in callback to application, see `mtb_pmbus_zone_events_t`.

To enable Zone protocols execute the next steps:
- Enable Zone protocols in Compile Time Options by setting `MTB_PMBUS_SUPPORT_ZONE` to 1U
- Enable Zone in config structure by setting `mtb_pmbus_stc_config_t::enable_zone` to true
- Enable Zone Config and Zone Active commands in Compile Time Options using `MTB_PMBUS_IMPL_CMD_ZONE_CONFIG` and `MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE`
- Enable Zone Config and Zone Active commands in config structure using `MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_EN` and `MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_EN` masks in `mtb_pmbus_stc_config_t::impl_cmd_mask` field
- Add Zone specific callback `mtb_pmbus_stc_config_t::zone_callback` in case if status should be handled

**Zone Write and Zone Read command handling:**

From User point of view, the commands handling during the Zone protocols is almost the same as for standard protocols. The only difference is that during Zone Read protocol, the lost arbitration event may occur when the Middleware loses arbitration to another target. To ensure that Controller completes the read of the command, always check if `MTB_PMBUS_CMD_DONE` event has occurred.

By default, after initialization of PMBus instance, the 0xFE zone is assigned for all pages. It is expected that the proper zone values are assigned by Controller or manually use the `mtb_pmbus_set_default_zones` function.

Zone protocols rely on the pages, but if pages are disabled for PMBus instance, the Zone Write and Zone Read are assigned globally for the whole instance.

### Controller Mode

The Controller mode provides dedicated API functions to execute standard SMBus protocols. The middleware handles protocol-specific details such as command code, byte counts, payload formatting, PEC calculation, and proper bus sequencing.

**Supported SMBus/PMBus Protocols:**

<table style="border: 1px solid black; border-collapse: collapse;">
<tr>
<th style="border: 1px solid black; padding: 8px;">Protocol Name</th>
<th style="border: 1px solid black; padding: 8px;">API Function</th>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Quick Command (Write direction)</td>
<td style="border: 1px solid black; padding: 8px;"><code>mtb_pmbus_ctrl_ex_quick_cmd</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Send Byte</td>
<td style="border: 1px solid black; padding: 8px;"><code>mtb_pmbus_ctrl_ex_send_byte</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Write Byte</td>
<td style="border: 1px solid black; padding: 8px;"><code>mtb_pmbus_ctrl_ex_write_byte</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Write Word</td>
<td style="border: 1px solid black; padding: 8px;"><code>mtb_pmbus_ctrl_ex_write_word</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Write 32</td>
<td style="border: 1px solid black; padding: 8px;"><code>mtb_pmbus_ctrl_ex_write_32</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Write 64</td>
<td style="border: 1px solid black; padding: 8px;"><code>mtb_pmbus_ctrl_ex_write_64</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Receive Byte</td>
<td style="border: 1px solid black; padding: 8px;"><code>mtb_pmbus_ctrl_ex_received_byte</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Read Byte</td>
<td style="border: 1px solid black; padding: 8px;"><code>mtb_pmbus_ctrl_ex_read_byte</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Read Word</td>
<td style="border: 1px solid black; padding: 8px;"><code>mtb_pmbus_ctrl_ex_read_word</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Read 32</td>
<td style="border: 1px solid black; padding: 8px;"><code>mtb_pmbus_ctrl_ex_read_32</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Read 64</td>
<td style="border: 1px solid black; padding: 8px;"><code>mtb_pmbus_ctrl_ex_read_64</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Process Call</td>
<td style="border: 1px solid black; padding: 8px;"><code>mtb_pmbus_ctrl_ex_process_call</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Block Write</td>
<td style="border: 1px solid black; padding: 8px;"><code>mtb_pmbus_ctrl_ex_block_write</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Block Read</td>
<td style="border: 1px solid black; padding: 8px;"><code>mtb_pmbus_ctrl_ex_block_read</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Block Write-Block Read Process Call</td>
<td style="border: 1px solid black; padding: 8px;"><code>mtb_pmbus_ctrl_ex_block_process_call</code></td>
</tr>
</table>

> **Note:** Quick Command with Read direction is not supported due to hardware limitations. See [Quick Command with Read Direction Limitation](design_considerations_hardware_and_timeout.md#quick-command-with-read-direction-limitation-controller-mode) section for details.

**Generic Transfer Execution API:**

For advanced use cases or custom protocols not covered by the standard API functions, the middleware provides a generic transfer execution API.

- `mtb_pmbus_ctrl_execute_transfer` - Generic transfer function, which accepts a transfer configuration structure (`mtb_pmbus_ctrl_stc_transfer_cfg_t`) containing all transfer parameters:
  - Target device address (addr)
  - The pointer to the data buffer (data) - used for both write and read operations
  - Write data size (wr_size) - number of bytes to write
  - Read data size (rd_size) - number of bytes to read
  - Stop condition control (execute_stop) - whether to generate STOP at the end of transfer

> **Note:** When using the generic transfer API, the application is responsible for the proper protocol sequencing and data formatting. The middleware still handles low-level I2C operations, and error detection.

> **Note:** See [Add SMBus/PMBus controller code to your project](controller_mode_quick_start.md#3-add-smbuspmbus-controller-code-to-your-project) section of the Controller mode Quick Start Guide for examples of using the generic transfer API to implement the Write Byte, Write Word, and Read Word protocols.

---

## Timeout Handling

The SMBus specification defines a few timeouts:
- Detect clock low timeout (tTIMEOUT)
- Cumulative clock low extend time (target device) (tLOW:TEXT)
- Cumulative clock low extend time (controller device) (tLOW:CEXT)

This middleware only detects the tTIMEOUT condition. The Timeout handling is a compile-time option.

> **Note:** Under standard configuration, the middleware does not violate tLOW:TEXT. However, enabling logging can significantly impact the response time to received addresses and bytes, making it easy to exceed the cumulative clock low extend time.

---

## Callback Handling

The PMBus middleware provides multiple callbacks for both Target and Controller modes. Almost all callbacks are called inside the ISR handler, so these callback functions must be optimized for the usage in ISR.

### Target Mode

Target mode provides several types of callbacks for handling commands and events: command callbacks, general event callbacks, zone callbacks, and error callbacks.

#### Command Callback

Each command may have its own callback. This callback is registered in the command config structure, see `mtb_pmbus_stc_config_cmd_t::callback`. The list of events is available in `mtb_pmbus_cmd_events_t`. The callback is called only with one event at a specific time moment.

Besides the event, the PMBus middleware also informs about the active page/phase and the received byte. The received byte is actual only for the `MTB_PMBUS_CMD_WRITE_BYTE` event. If the command is not paged and/or phased, the page/phase parameters must be ignored.

The callback can return the bool value, this value is ignored for all events except `MTB_PMBUS_CMD_WRITE_BYTE` and `MTB_PMBUS_CMD_MATCH`. For the `MTB_PMBUS_CMD_WRITE_BYTE` event, the return value will indicate which response (true - ACK, false - NACK) to send after receiving the data byte. The `MTB_PMBUS_CMD_WRITE_BYTE` event is triggered only for data bytes, for example, byte-count byte for Block Write/Read is completely handled inside the middleware. For `MTB_PMBUS_CMD_MATCH`, NACK command is possible if the command is not ready to be processed.

Events visualization for write transfer:

![Command Callback Write](../images/pmbus_cmd_callback_write.png)

Events visualization for read transfer:

![Command Callback Read](../images/pmbus_cmd_callback_read.png)

Events visualization for process call transfer:

![Command Callback Process Call](../images/pmbus_cmd_callback_process_call.png)

Events visualization for PAGE_PLUS_WRITE transfer:

![Command Callback Page Plus Write](../images/pmbus_cmd_callback_p_plus_wr.png)

Events visualization for PAGE_PLUS_READ transfer:

![Command Callback Page Plus Read](../images/pmbus_cmd_callback_p_plus_rd.png)

Events visualization for PAGE_PLUS_READ process call transfer:

![Command Callback Page Plus Process Call](../images/pmbus_cmd_callback_p_plus_pc.png)

Events visualization for Zone Write transfer:

![Command Callback Zone Write](../images/pmbus_cmd_callback_zone_wr.png)

Events visualization for Zone Read transfer:

![Command Callback Zone Read](../images/pmbus_cmd_callback_zone_rd.png)

> **Note:** During Zone Read transfer, the `MTB_PMBUS_CMD_ARB_LOST` event might occur once per each read response. If the `MTB_PMBUS_CMD_ARB_LOST` event has been occurred during response - the following `MTB_PMBUS_CMD_READ_DONE` will be ignored.

Events visualization for Group Command transfer. Callback triggering is analyzed for Device 2:

![Command Callback Group Command](../images/pmbus_cmd_callback_group_command.png)

#### General Callback

The PMBus middleware provides two common callbacks: one for general events (see `mtb_pmbus_handle_cmd_events_t`) and one for Zone features (see `mtb_pmbus_handle_zone_events_t`). The general events callback is required only if any of these events is/are used by the application. The middleware always responds to Quick Command with both RD and WR directions as required by the SMBus/PMBus specification, always sending an ACK after receiving the target address. Additionally, the middleware sends a byte in the Received Byte callback with the default value is 0xFF.

> **Note:** When PMBus mode is enabled, a target address with the RD direction is considered a data content fault. The middleware still sends an ACK after receiving the address but generates the corresponding error in the `mtb_pmbus_handle_error_events_t` callback.

The `mtb_pmbus_handle_error_events_t` callback is also used to report communication errors. This callback is typically invoked when a STOP condition occurs on the bus or when an extraordinary sequence of events happens, such as a bus error or timeout. It is important for the application to use this callback to provide the correct response to fault events. The application can ignore handling certain error events if they are not relevant to the features in use. For example, there is no need to handle PMBus-specific events when only SMBus mode is enabled.

### Controller Mode

Controller mode provides a simpler callback mechanism compared to Target mode. The callbacks are registered in the `mtb_pmbus_ctrl_cfg_t` configuration structure.

#### Event Callback

The `mtb_pmbus_ctrl_cfg_t::callback_events` callback is invoked to report the completion of transfer operations or communication errors. This callback is registered in the controller configuration structure and is called with events from `mtb_pmbus_ctrl_events_t` enumeration.

> **Note:** Implementation of this callback is optional but recommended. It allows the application to handle successful transfer completion and error conditions appropriately. Without this callback, the application can still poll the transfer status, but it will not receive immediate notifications of events.

The callback is called inside the ISR handler, so it must be optimized for ISR usage.

**List of events reported through this callback:**

<table style="border: 1px solid black; border-collapse: collapse;">
<tr>
<th style="border: 1px solid black; padding: 8px;">Event</th>
<th style="border: 1px solid black; padding: 8px;">Value from enumeration</th>
<th style="border: 1px solid black; padding: 8px;">Description</th>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Transfer Done</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_CTRL_TRANSFER_DONE</code></td>
<td style="border: 1px solid black; padding: 8px;">Transfer completed successfully. The controller has successfully completed the requested transfer operation (write, read, or process call). All data bytes were transmitted or received, and all acknowledgments were received as expected. If PEC is enabled, the PEC byte was successfully validated.</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Corrupted Data</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_CTRL_CORRUPTED_DATA</code></td>
<td style="border: 1px solid black; padding: 8px;">Data corruption detected. This event indicates that PEC validation failed - the calculated PEC does not match the received PEC byte.</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Target NACK Address</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_CTRL_TARGET_NACK_ADDR</code></td>
<td style="border: 1px solid black; padding: 8px;">Target device NACKed the address. The target device did not acknowledge its address during the address phase of the transfer.</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Target NACK Command</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_CTRL_TARGET_NACK_CMD</code></td>
<td style="border: 1px solid black; padding: 8px;">Target device NACKed the command code. The target device acknowledged its address but NACKed the command code byte. This indicates that the command code is not supported by the target device or the target device is not ready to accept this command.</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Target NACK Byte</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_CTRL_TARGET_NACK_BYTE</code></td>
<td style="border: 1px solid black; padding: 8px;">Target device NACKed a data byte. The target device NACKed the last data byte sent by the controller during a write or process call operation.</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Timeout</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_CTRL_TIMEOUT</code></td>
<td style="border: 1px solid black; padding: 8px;">Timeout occurred during transfer. This event is generated when the hardware timeout detection mechanism detects that the SCL line has been held low for more than 25 ms, which exceeds the SMBus timeout specification (tTIMEOUT).</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Bus Error</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_CTRL_BUS_ERR</code></td>
<td style="border: 1px solid black; padding: 8px;">Bus error detected. The controller detected an erroneous START or STOP condition on the bus that violates the I2C protocol specification.</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Arbitration Lost</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_CTRL_ARB_LOST</code></td>
<td style="border: 1px solid black; padding: 8px;">Arbitration lost on the bus. The controller lost arbitration to another controller device on the bus. This is a normal condition in multi-controller systems where multiple controllers may attempt to access the bus simultaneously.</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Abort Start</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_CTRL_ABORT_START</code></td>
<td style="border: 1px solid black; padding: 8px;">Controller abort start.</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Block Count Too Big</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_CTRL_BLOCK_COUNT_TOO_BIG</code></td>
<td style="border: 1px solid black; padding: 8px;">Received block count exceeds buffer size. During a block read operation (using <code>mtb_pmbus_ctrl_ex_block_read</code>) or block process call operation (using <code>mtb_pmbus_ctrl_ex_block_process_call</code>), the target device sent a byte count value that exceeds the size parameter specified in the function call. This indicates a mismatch between the expected buffer size and the actual data size reported by the target. After this event, the middleware stops the transfer to prevent buffer overflow.</td>
</tr>
</table>

---

## Command Organization

**Target mode**

The PMBus middleware supports configuration of multiple pages and phases for commands.

Page/Phase configuration for commands. The command supports two types of configuration:
- The command is not paged/phased. In this case, only one of command copies is available for all pages/phases.
- The command is paged/phased. In this case, the command has a number of copies configured in `mtb_pmbus_stc_config_t::num_pages` or `mtb_pmbus_stc_config_t::num_phases`.

The same command can be paged and phased or only paged or only phased or neither.

- If a command is paged/phased, implement a two-dimensional array for the data buffer:

```c
  cmd_data[NUM_PAGES][DATA_SIZE]
  cmd_data[NUM_PHASES][DATA_SIZE]
```

- If a command is paged and phased at the same time, a three-dimensional array is required for the data buffer:

```c
  cmd_data[NUM_PAGES][NUM_PHASES][DATA_SIZE]
```

> **Note:** For the block write/read protocols, increment the DATA_SIZE by one byte. This additional byte is necessary to store the byte number within the package.

### Command Capabilities

The commands have multiple capabilities, which define the command behavior. Refer to the [Command Capabilities Macros section in the API Reference Guide](https://github.com/Infineon/mtb-pmbus/blob/release-v1.2.0/docs/html/group__group__cmd__capabilities__macros.html) for more information.

### Implemented Commands

The PMBus Middleware has implemented several commands from PMBus specification. Usage of implemented commands is optional and command's own implementation can be provided through the general command table.

Perform the next steps to use implemented commands:
- Enable implemented command by a corresponding compile time macro in the mtb_pmbus_conf.h file by setting the macro to 1 (this setting is applicable for all instances).
- Add the corresponding run time macro to `mtb_pmbus_stc_config_t::impl_cmd_mask` (this setting is applicable only for specific instances)
- Ensure that a command with the same code is not defined in the main command table: `mtb_pmbus_stc_config_t::cmd_table`

**List of implemented commands:**

<table style="border: 1px solid black; border-collapse: collapse;">
<tr>
<th style="border: 1px solid black; padding: 8px;">Name</th>
<th style="border: 1px solid black; padding: 8px;">Code</th>
<th style="border: 1px solid black; padding: 8px;">Implementation details</th>
<th style="border: 1px solid black; padding: 8px;">Compile time macro</th>
<th style="border: 1px solid black; padding: 8px;">Run-time macro</th>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">PAGE</td>
<td style="border: 1px solid black; padding: 8px;">0x00</td>
<td style="border: 1px solid black; padding: 8px;">The command stores the active page number and middleware uses it to update the corresponding data buffer. Also, the controller can read the last assigned page number.</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_PAGE</code></td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_PAGE_EN</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">PHASE</td>
<td style="border: 1px solid black; padding: 8px;">0x04</td>
<td style="border: 1px solid black; padding: 8px;">The command stores the active phase number and middleware uses it to update the corresponding data buffer. Also, the controller can read the last assigned phase number.</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_PHASE</code></td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_PHASE_EN</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">PAGE_PLUS_WRITE</td>
<td style="border: 1px solid black; padding: 8px;">0x05</td>
<td style="border: 1px solid black; padding: 8px;">The command allows a temporary change to the page and writing to a command in the same bus transaction.</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE</code></td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE_EN</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">PAGE_PLUS_READ</td>
<td style="border: 1px solid black; padding: 8px;">0x06</td>
<td style="border: 1px solid black; padding: 8px;">The command allows a temporary change to the page and reading from a command in the same bus transaction.</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ</code></td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ_EN</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">ZONE_CONFIG</td>
<td style="border: 1px solid black; padding: 8px;">0x07</td>
<td style="border: 1px solid black; padding: 8px;">The command stores the configured Read and Write Zones. The middleware uses it during Zone Read and Write Protocols.</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_ZONE_CONFIG</code></td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_ZONE_CONFIG_EN</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">ZONE_ACTIVE</td>
<td style="border: 1px solid black; padding: 8px;">0x08</td>
<td style="border: 1px solid black; padding: 8px;">The command sets Read and Write Zones. The middleware uses it during Zone Read and Write Protocols.</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE</code></td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE_EN</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">P2_PLUS_WRITE</td>
<td style="border: 1px solid black; padding: 8px;">0x09</td>
<td style="border: 1px solid black; padding: 8px;">The command allows a temporary change to the page and phase and writing to a command in the same bus transaction.</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE</code></td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE_EN</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">P2_PLUS_READ</td>
<td style="border: 1px solid black; padding: 8px;">0x0A</td>
<td style="border: 1px solid black; padding: 8px;">The command allows a temporary change to the page and phase and reading from a command in the same bus transaction.</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_P2_PLUS_READ</code></td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_P2_PLUS_READ_EN</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">CAPABILITY</td>
<td style="border: 1px solid black; padding: 8px;">0x19</td>
<td style="border: 1px solid black; padding: 8px;">The command returns some key capabilities of the PMBus Device: If PEC is enabled, The Maximum supported Bus Speed, If SMBALERT pin is present, The supported Number Format, AVSBus support is always disabled for this implementation</td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_CAPABILITY</code></td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_CAPABILITY_EN</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">QUERY</td>
<td style="border: 1px solid black; padding: 8px;">0x1A</td>
<td style="border: 1px solid black; padding: 8px;">The command returns info about the requested command: If the command is supported (Defined in command table), The supported directions (WR OR/AND RD), The supported numeric format. This information is configured in <code>mtb_pmbus_stc_config_cmd_t::cmd_cap</code>. The controller can request information only for the command from <code>mtb_pmbus_stc_config_t::cmd_table</code></td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_QUERY</code></td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_QUERY_EN</code></td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">PMBUS_REVISION</td>
<td style="border: 1px solid black; padding: 8px;">0x98</td>
<td style="border: 1px solid black; padding: 8px;">The command returns the PMBus revision configured in <code>mtb_pmbus_stc_config_t::revision</code></td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_REVISION</code></td>
<td style="border: 1px solid black; padding: 8px;"><code>MTB_PMBUS_IMPL_CMD_REVISION_EN</code></td>
</tr>
</table>

### Extended Commands Support

The PMBus middleware provides the built-in support for extended command - PMBUS_COMMAND_EXT (0xFE). Once this command is detected, the middleware switches the active command table to the extended commands table and handles the remaining transaction as a standard command but from the extended commands table.

To use extended commands:
- Enable Extended Command support in Compile Time Options: Set `MTB_PMBUS_SUPPORT_EXT_CMD` to 1U
- Enable Extended Command for specific PMBus instance by setting true for `mtb_pmbus_stc_config_t::enable_ext_cmd`.
- Create Command table with required commands and assign the table address to `mtb_pmbus_stc_config_t::ext_cmd_table`.
- Specify the number of commands in `mtb_pmbus_stc_config_t::ext_cmd_num`.

The maximum number of extended commands is 256. None of these commands are reserved by PMBus middleware.

---

## Data Format Conversion Functions

The PMBus middleware provides a set of functions to convert data between PMBus data formats and Float32 format. The conversion functions are:
- From PMBus LINEAR11 to Float32. See `mtb_pmbus_lin11_to_float`.
- From Float32 to PMBus LINEAR11. See `mtb_pmbus_float_to_lin11`.
- From PMBus LINEAR16 to Float32. See `mtb_pmbus_lin16_to_float`.
- From Float32 to PMBus LINEAR16. See `mtb_pmbus_float_to_lin16`.

> **Note:** LINEAR11 and LINEAR16 are less precise than Float32, so conversion from Float32 to PMBus LINEAR11/16 may lead to loss of precision.

> **Note:** Mantissa of PMBus LINEAR11 and LINEAR16 are not normalized. So different combinations of mantissa and exponent may lead to the same value when converted to Float32:

> ```c
> 0x0004(LIN11 HEX) ->  0 exponent, 4 mantissa   -> 4.0f;
> 0x1001(LIN11 HEX) ->  2 exponent, 1 mantissa   -> 4.0f;
> 0xCA00(LIN11 HEX) -> -7 exponent, 512 mantissa -> 4.0f;
> ```

---

## Optional Signals

The SMBus and PMBus specifications specify a set of optional signals. The middleware supports some of them.

<table style="border: 1px solid black; border-collapse: collapse;">
<tr>
<th style="border: 1px solid black; padding: 8px;">Signal Name</th>
<th style="border: 1px solid black; padding: 8px;">Support (Target mode)</th>
<th style="border: 1px solid black; padding: 8px;">Support (Controller mode)</th>
<th style="border: 1px solid black; padding: 8px;">Specification</th>
<th style="border: 1px solid black; padding: 8px;">Implementation details</th>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">SMBSUS#</td>
<td style="border: 1px solid black; padding: 8px;">Not supported</td>
<td style="border: 1px solid black; padding: 8px;">Not supported</td>
<td style="border: 1px solid black; padding: 8px;">SMBus</td>
<td style="border: 1px solid black; padding: 8px;">This signal must be handled by the Application. When the SMBSUS# signal goes low, the Application can call <code>mtb_pmbus_disable</code> function to disable the middleware. After the SMBSUS# goes high, the Application can call <code>mtb_pmbus_enable</code> to restore the communication.</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">SMBALERT#</td>
<td style="border: 1px solid black; padding: 8px;">Supported</td>
<td style="border: 1px solid black; padding: 8px;">Not supported</td>
<td style="border: 1px solid black; padding: 8px;">SMBus</td>
<td style="border: 1px solid black; padding: 8px;">The SMBALERT# signal has the built-in support in the middleware. See: <code>mtb_pmbus_smbalert_config_mode</code>, <code>mtb_pmbus_smbalert_set_signal</code> and <code>mtb_pmbus_smbalert_clear_signal</code>.</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Control Signal (CONTROL)</td>
<td style="border: 1px solid black; padding: 8px;">Not supported</td>
<td style="border: 1px solid black; padding: 8px;">Not supported</td>
<td style="border: 1px solid black; padding: 8px;">PMBus</td>
<td style="border: 1px solid black; padding: 8px;">This signal must be handled by the Application.</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Write Protect (WP)</td>
<td style="border: 1px solid black; padding: 8px;">Not supported</td>
<td style="border: 1px solid black; padding: 8px;">Not supported</td>
<td style="border: 1px solid black; padding: 8px;">PMBus</td>
<td style="border: 1px solid black; padding: 8px;">This signal must be handled by the Application. Use the <code>mtb_pmbus_cmd_wr_protect</code> and <code>mtb_pmbus_cmd_all_wr_protect</code> to protect commands against write.</td>
</tr>
</table>

---

## Logging

The PMBus middleware provides the possibility to enable the logging feature. The logging can be enabled by defining MTB_PMBUS_LOG_LEVEL with the selected log level in mtb_pmbus_conf.h file. See the available log levels in the [Logging Level Macros section in the API Reference Guide](https://github.com/Infineon/mtb-pmbus/blob/release-v1.2.0/docs/html/group__group__log__level__macros.html).

By default, the logs are printed by the retarget-io middleware. So, initialize this middleware at the application level. If another output method is required, redirect the PMBus logging by defining MTB_PMBUS_CUSTOM_LOG in the mtb_pmbus_conf.h file and provide custom implementation of the mtb_pmbus_log function. Also, you can redefine the buffer size by MTB_PMBUS_LOG_BUF.

> **Note:** If you select MTB_PMBUS_LOG_LEVEL_INFO or MTB_PMBUS_LOG_LEVEL_DEBUG as the log levels, too many log messages can be printed, which leads to a timeout error on the bus.
> Recommended:
> - Increase the data speed of the logging method
> - Redirect the logging data to the buffer and print messages after transfers

---

## Low Power Support

The middleware can operate only in power modes supported by the corresponding I2C/Timer hardware. Typically, only Active and Sleep modes are supported.

---

## Compile Time Options

The PMBus Middleware provides the opportunity to disable unused features to reduce the memory footprint and improve the speed of code execution. The Compile Time Options are applicable for all instances, so if Instance 0 uses PEC and Instance 1 does not use it, the `MTB_PMBUS_SUPPORT_PEC` compile time option must be enabled.

Also, if you disable some of the compile time options, not all APIs will be available. For example: `mtb_pmbus_stc_config_t::enable_pec` is available only if `MTB_PMBUS_SUPPORT_PEC` is set to 1U.

Use the mtb_pmbus_conf.h file to reflect PMBus Middleware configuration in your application. This file will be automatically copied into your project once the middleware is added by the Library Manager.

The list of Compile Time Options for the target and their default values can be found in the [Target Compile Time Options Macros section in the API Reference Guide](https://github.com/Infineon/mtb-pmbus/blob/release-v1.2.0/docs/html/group__group__target__cto__macros.html).
The list of Compile Time Options for both target and controller and their default values can be found in the [Common Compile Time Options Macros section in the API Reference Guide](https://github.com/Infineon/mtb-pmbus/blob/release-v1.2.0/docs/html/group__group__common__cto__macros.html).

