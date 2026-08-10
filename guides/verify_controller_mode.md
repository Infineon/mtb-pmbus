# Verify Controller Mode Workability

This section describes how to verify that the PMBus controller device is working correctly.

## Build and Program

1. Build and program your project.
2. Open a terminal application (e.g., PuTTY, Tera Term) and connect to the device COM port.
3. Check if the I2C transport is initialized correctly and the controller is ready.

![Controller Start](../images/pmbus_qsg_ctrl_start.png)

4. Connect the PMBus target device (address 0x18) to the I2C bus.
5. Reset the controller device to start the tests.

## Quick Command protocol test

1. The controller will automatically send Quick Command to the target device (address 0x18).
2. Observe the User LED toggle on the target device.
3. Check the terminal output for the transfer completion status.
   - Controller terminal output:

   ![Quick Command Controller](../images/pmbus_qsg_ctrl_quick_cmd_ctrl.png)

   - Target terminal output:

   ![Quick Command Target](../images/pmbus_qsg_ctrl_quick_cmd_trgt.png)

## Read 32 protocol test

1. The controller will read 4 bytes from CMD1 (0x10) on the target device.
2. This command reads the LED toggle counter value.
3. Observe the terminal output showing the read data and counter value.
   - Controller terminal output:

   ![Read 32 Controller](../images/pmbus_qsg_ctrl_read32_ctrl.png)

   - Target terminal output:

   ![Read 32 Target](../images/pmbus_qsg_ctrl_read32_trgt.png)

## Generic transfer examples test

1. The controller will send PAGE command (0x00) with page number (0x01) using generic transfer API.
   - Controller terminal output:

   ![Generic Transfer PAGE Controller](../images/pmbus_qsg_ctrl_generic_transfer_page_ctrl.png)

   - Target terminal output:

   ![Generic Transfer PAGE Target](../images/pmbus_qsg_ctrl_generic_transfer_page_trgt.png)

2. The controller will write 0xABCD to CMD2 (0x11) using Write Word protocol.
   - Controller terminal output:

   ![Generic Transfer Write Controller](../images/pmbus_qsg_ctrl_generic_transfer_write_ctrl.png)

   - Target terminal output:

   ![Generic Transfer Write Target](../images/pmbus_qsg_ctrl_generic_transfer_write_trgt.png)

3. The controller will read data from CMD2 (0x11) using Read Word protocol.
   - Controller terminal output:

   ![Generic Transfer Read Controller](../images/pmbus_qsg_ctrl_generic_transfer_read_ctrl.png)

   - Target terminal output:

   ![Generic Transfer Read Target](../images/pmbus_qsg_ctrl_generic_transfer_read_trgt.png)

4. Observe the terminal output showing all transfer results.

