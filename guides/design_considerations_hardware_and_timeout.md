# Design Considerations: Hardware and Timeout

## Host Notify Protocol

> **Note:** To reduce the memory consumption in case of using Target and Controller modes on the same device, the Host Notify Protocol has a separate compile-time options:
> - `MTB_PMBUS_SUPPORT_HOST_NOTIFY` for Target mode
> - `MTB_PMBUS_CTRL_SUPPORT_HOST_NOTIFY` for Controller mode

### Target Mode

Enabling the Host Notify Protocol allows the Target device act as a controller and initiates a transfer to notify the Host about an urgent event. This feature is depends on the hardware settings. Please, refer to the [SCB I2C Configuration](#scb-i2c-configuration) section for details on how to configure the hardware for Host Notify Protocol support.

Result of the Host Notify Protocol transfer is reported through the `mtb_pmbus_handle_host_notify_t` callback.

### Controller Mode

Enabling the Host Notify Protocol allows the controller act role of the Host and receive notifications from Target devices that support the Host Notify Protocol. When a Target device initiates a Host Notify transfer, the Controller receives the notification and report it through the `mtb_pmbus_ctrl_handle_host_notify_t` callback. This feature is depends on the hardware settings. Please, refer to the [SCB I2C Configuration](#scb-i2c-configuration) section for details on how to configure the hardware for Host Notify Protocol support.

---

## Hardware-Dependent Layer

### Limitations and Exceptions of the Zone Feature (Target Mode)

According to the PMBus specification, during a ZONE_READ transaction, the controller generates an Acknowledge (ACK) bit for the last byte read before issuing a repeated start condition. This behavior contradicts the I2C specification, which states that a read transaction must end with a Not Acknowledge (NACK) bit to indicate that the last byte has been read. The PMBus middleware is built on top of the I2C communication block, so it cannot reliably detect a restart or stop event during a read transaction if the last byte read by the controller was acknowledged with an ACK bit.

Note the ZONE_READ with PEC case: a sequence that also cannot be correctly recognized by the middleware due to hardware limitations, occurs before the stop event when receiving data from the last device in the zone: /Read last data byte/NACK/PEC/NACK/STOP.

The PEC can be disabled for the PMBus middleware by setting `MTB_PMBUS_SUPPORT_PEC` to 0U or `mtb_pmbus_stc_config_t::enable_pec` to false. Disabling PEC allows the middleware to correctly identify the end of a ZONE_READ read transaction.

### Quick Command with Read Direction Limitation (Controller Mode)

The controller cannot execute the Quick Command protocol with Read direction due to hardware limitations of the SCB I2C block. According to the SMBus/PMBus specification, the Quick Command with Read direction consists of sending the target address with the Read bit set, followed immediately by a STOP condition after receiving the ACK bit from the target device, without reading any data bytes.

The SCB hardware does not support generating a STOP condition immediately after receiving an ACK for the address byte with the Read direction bit set, without reading at least one data byte. This hardware constraint makes it impossible for the middleware to correctly implement the Quick Command with Read direction protocol as specified in the SMBus/PMBus specifications.

> **Note:** The Quick Command with Write direction is fully supported and operates correctly, as the hardware can generate a STOP condition immediately after the ACK bit for an address with the Write direction.

### SCB I2C Configuration

#### Target Mode

The steps to configure the SCB I2C for PMBus Target operation:
1. Open the Device Configurator: Navigate to the Peripherals tab and enable the SCB as I2C.
2. Configure the I2C General settings:
   - Set the Mode to Slave. Set the Master-Slave if Host Notify feature is required.
   - Set the desired Data Rate (100 kHz, 400 kHz, or 1 MHz).
   - Disable the Use TX FIFO and Use RX FIFO options.
   - Enable Accept Matching Address in RX FIFO option.
3. Configure the I2C Slave settings:
   - Set the Slave Address to the desired 7-bit address.
   - Set the Address Mask to 0.
   - Enable the Accept General Call option if required.
4. Configure the I2C Connection settings:
   - Set the SCL and SDA pins to the desired GPIO pins. The Device Configurator will automatically configure the pins Drive Mode and the other settings.
   - Set the Clock to the desired clock source to meet the Data Rate requirement.
5. Save the configuration and close the Device Configurator.

#### Controller Mode

The steps to configure the SCB I2C for PMBus Controller operation:
1. Open the Device Configurator: Navigate to the Peripherals tab and enable the SCB as I2C.
2. Configure the I2C General settings:
   - Set the Mode to Master. Set the Master-Slave if Host Notify feature is required.
     > **Note:** In case of using more than one Controller instance on the same device, this setting would define which instance will support Host Notify feature.
   - Set the desired Data Rate (100 kHz, 400 kHz, or 1 MHz)
   - Disable the Use RX FIFO but enable the Use TX FIFO option
   - Set the Address to 8 if Host Notify feature is required.
   - Set the Address Mask to 254 if Host Notify feature is required.
   - Disable Accept Matching Address in RX FIFO option if Host Notify feature is required.
3. Configure the I2C Connection settings:
   - Set the SCL and SDA pins to the desired GPIO pins. The Device Configurator will automatically configure the pins Drive Mode and the other settings.
   - Set the Clock to the desired clock source to meet the Data Rate requirement
4. Save the configuration and close the Device Configurator.

> **Note:** Refer to the [Timeout Detection Configuration](#timeout-detection-configuration) section for Timeout Detection configuration.

> **Note:** Refer to the [Initialization Sequence](design_considerations_setup_and_initialization.md#initialization-sequence) section for adding initialization code for PMBus Middleware.

### Different Timeout Detection HW Implementations

PMBus instances in both target and controller modes support two primary methods for handling timeouts in the middleware, both of which are closely tied to the hardware capabilities of the supported devices. To determine the specific capabilities of your device, consult the Technical Reference Manual (TRM) documentation.

- **Approach 1.** Involves utilizing the Timeout Generation Support (TGS) counter, which is a component of the hardware communication block. The primary function of the TGS counter is to detect when data transfer is stuck or incomplete, thereby enabling the implementation of timeout handling mechanisms.
- **Approach 2.** Employs an external timer triggered by a signal from the communication line, such as the SCL. This solution relies on the internal connection between the communication line and a timer to generate a trigger signal, thereby facilitating the timeout handling.

### Timeout Detection Configuration

To enable Timeout Detection, use the macro `MTB_PMBUS_ENABLE_TIMEOUT`. To select the method for the handling timeout, use the following macros:
- `MTB_PMBUS_HAL_USE_TGS`
- `MTB_PMBUS_HAL_USE_TCPWM`

**To initialize the TGS timeout handling, follow these steps:**

1. Open the Device Configurator: Navigate to the Peripherals tab and select the SCB applied to PMBus.
2. Enable TGS Timeout Generation: Go to the TGS section and enable the Timer (Enable) checkbox.
3. Configure TGS; Assign the peripheral clock to TGS and set the Reload value. The Reload value is calculated as TGS_FREQUENCY_CLOCK * 0.025.
4. Configure Reload Event: Enable the Reload event and select the option "On falling edge of I2C SCL".
5. Configure Counter: Enable the Counter gated and select the option "While I2C SCL is high".
6. Configure Stop Event: Enable the Stop event and select the option "On rising edge of I2C SCL".
7. Add Initialization Code:

```c
    cy_en_scb_tgs_status_t pdl_tgs_status;

    pdl_tgs_status = Cy_SCB_TGSx_Init(PMBUS_I2C_HW, MTB_PMBUS_TGS_TIMER_NUM, &PMBUS_I2C_TGS_CONFIG);
    if (CY_SCB_TGS_SUCCESS != pdl_tgs_status)
    {
        MTB_PMBUS_LOG_ERR("Error during TGS initialization. Status: %X", pdl_tgs_status);
    }
    else
    {
        Cy_SCB_TGSx_Enable(PMBUS_I2C_HW, MTB_PMBUS_TGS_TIMER_NUM);
    }
```

- **Target mode**: Add this code after the SCB I2C initialization.
- **Controller mode**: Place this code in the `mtb_pmbus_ctrl_hw_resources_ctrl_t` callback (callback_hw) in the MTB_PMBUS_CTRL_HW_RESOURCES_INIT event.

**To initialize the TCPWM timeout handling, follow these steps:**

1. Open the Device Configurator: Navigate to the Peripherals tab and enable the TCPWM as the Timer-Counter.
2. Configure TCPWM; Assign the peripheral clock to the Timer-Counter and set Run mode to One shot in the General section.
3. Set Period: Set the Period in the General section calculated as TIMER_FREQUENCY_CLOCK * 0.025.
4. Configure Compare: Select the Compare option as Compare and set the Compare value to the same value as the Period.
5. Configure Interrupt Source: Go to the Interrupt Source section and enable the Compare and Capture checkbox.
6. Configure Inputs: Move to the Inputs section, select the Stop Input as Rising Edge, and assign the scl_filtered signal of SCB to the Stop Signal.
7. Configure Reload Input: Select the Reload Input as Falling Edge and assign the scl_filtered signal of SCB to the Reload Signal.

**Target mode initialization:**

Create the Timer IRQ handler:

```c
void timer_isr(void)
{
    mtb_pmbus_timer_isr(&pmbus_inst);
}
```

Initialize the Timer (this code should be added after the SCB I2C initialization):

```c
    cy_en_tcpwm_status_t pdl_tcpwm_status = Cy_TCPWM_Counter_Init(PMBUS_TIMEOUT_HW, PMBUS_TIMEOUT_NUM, &PMBUS_TIMEOUT_CONFIG);
    if (CY_TCPWM_SUCCESS != pdl_tcpwm_status)
    {
        MTB_PMBUS_LOG_ERR("Error during TCPWM PDL initialization. Status: %X", pdl_tcpwm_status);
    }
    else
    {
        cy_stc_sysint_t timer_isr_cfg =
        {
            .intrSrc = PMBUS_TIMEOUT_IRQ,
            .intrPriority = 3U
        };

        pld_interrupt_status = Cy_SysInt_Init(&timer_isr_cfg, timer_isr);
        if (CY_SYSINT_SUCCESS != pld_interrupt_status)
        {
            MTB_PMBUS_LOG_ERR("Error during Timer Interrupt initialization. Status: %X", pld_interrupt_status);
        }
        else
        {
            MTB_PMBUS_LOG_INF("Timer is initialized");
        }
    }
```

Add TCPWM HW pointer and counter number to the mtb_pmbus_stc_config_hal_t:

```c
mtb_pmbus_stc_config_hal_t pmbus_hal_cfg =
{
    .hw_ptr = PMBUS_I2C_HW,
    .pdl_i2c_context = &i2c_pdl_context,
#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
    .timeout_tcpwm_base = PMBUS_TIMEOUT_HW,
    .timeout_tcpwm_cntnum = PMBUS_TIMEOUT_NUM,
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
#if (defined (MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U))
    .smbalert_port_addr = PMBUS_SMBALERT_PORT,
    .smbalert_pin_num = PMBUS_SMBALERT_PIN,
#endif /* #if (defined (MTB_PMBUS_SUPPORT_SMBALERT) && (MTB_PMBUS_SUPPORT_SMBALERT != 0U)) */
};
```

Provide/update `mtb_pmbus_hw_resources_ctrl_t` for TCPWM as HW resource:

```c
void hw_resource_enable_callback(mtb_pmbus_hw_resources_ctrl_action_t action)
{
    if (action == MTB_PMBUS_HW_RESOURCES_ENABLE)
    {
        Cy_SCB_I2C_Enable(PMBUS_I2C_HW);
#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
        Cy_TCPWM_Counter_Enable(PMBUS_TIMEOUT_HW, PMBUS_TIMEOUT_NUM);
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
    }
    else if (action == MTB_PMBUS_HW_RESOURCES_DISABLE)
    {
        Cy_SCB_I2C_Disable(PMBUS_I2C_HW, &i2c_pdl_context);
#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
        Cy_TCPWM_Counter_Disable(PMBUS_TIMEOUT_HW, PMBUS_TIMEOUT_NUM);
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
    }
}
```

Provide/update `mtb_pmbus_hw_isr_ctrl_t` to enable/disable TCPWM interrupt:

```c
void hw_isr_enable(void)
{
    NVIC_EnableIRQ((IRQn_Type) PMBUS_I2C_IRQ);
#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
    NVIC_EnableIRQ((IRQn_Type) PMBUS_TIMEOUT_IRQ);
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
}
 
void hw_isr_disable(void)
{
    NVIC_DisableIRQ((IRQn_Type) PMBUS_I2C_IRQ);
#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
    NVIC_DisableIRQ((IRQn_Type) PMBUS_TIMEOUT_IRQ);
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
}
```

**Controller mode initialization:**

Create a Timer IRQ handler:

```c
void timer_isr(void)
{
    mtb_pmbus_ctrl_timer_isr(&pmbus_inst);
}
```

Initialize the Timer (this code should be placed in the `mtb_pmbus_ctrl_hw_resources_ctrl_t` callback (callback_hw) in the MTB_PMBUS_CTRL_HW_RESOURCES_INIT event):

```c
    cy_en_tcpwm_status_t pdl_tcpwm_status = Cy_TCPWM_Counter_Init(PMBUS_TIMEOUT_HW, PMBUS_TIMEOUT_NUM, &PMBUS_TIMEOUT_config);
    if (CY_TCPWM_SUCCESS != pdl_tcpwm_status)
    {
        MTB_PMBUS_LOG_ERR("Error during TCPWM PDL initialization. Status: %X", pdl_tcpwm_status);
    }
    else
    {
        cy_stc_sysint_t timer_isr_cfg =
        {
            .intrSrc = PMBUS_TIMEOUT_IRQ,
            .intrPriority = 3U
        };

        cy_en_sysint_status_t pld_interrupt_status = Cy_SysInt_Init(&timer_isr_cfg, timer_isr);
        if (CY_SYSINT_SUCCESS != pld_interrupt_status)
        {
            MTB_PMBUS_LOG_ERR("Error during Timer Interrupt initialization. Status: %X", pld_interrupt_status);
        }
        else
        {
            MTB_PMBUS_LOG_INF("Timer is initialized");
        }
    }
```

Enable the Timer (this code should be placed in the `mtb_pmbus_ctrl_hw_resources_ctrl_t` callback (callback_hw) in the MTB_PMBUS_CTRL_HW_RESOURCES_ENABLE event):

```c
            Cy_TCPWM_Counter_Enable(PMBUS_TIMEOUT_HW, PMBUS_TIMEOUT_NUM);
```

Disable the Timer (this code should be placed in the `mtb_pmbus_ctrl_hw_resources_ctrl_t` callback (callback_hw) in the MTB_PMBUS_CTRL_HW_RESOURCES_DISABLE event):

```c
            Cy_TCPWM_Counter_Disable(PMBUS_TIMEOUT_HW, PMBUS_TIMEOUT_NUM);
```

Add TCPWM HW pointer and counter number to the mtb_pmbus_ctrl_stc_config_hal_t:

```c
mtb_pmbus_ctrl_stc_config_hal_t pmbus_hal_cfg =
{
    .hw_ptr = PMBUS_I2C_HW,
    .pdl_i2c_context = &pdl_context,
#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
    .timeout_tcpwm_base = PMBUS_TIMEOUT_HW,
    .timeout_tcpwm_cntnum = PMBUS_TIMEOUT_NUM,
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
};
```

Provide/update `mtb_pmbus_ctrl_hw_isr_ctrl_t` to enable/disable TCPWM interrupt:

```c
void hw_isr_enable(void)
{
    NVIC_EnableIRQ((IRQn_Type) PMBUS_I2C_IRQ);
#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
    NVIC_EnableIRQ((IRQn_Type) PMBUS_TIMEOUT_IRQ);
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
}
 
void hw_isr_disable(void)
{
    NVIC_DisableIRQ((IRQn_Type) PMBUS_I2C_IRQ);
#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
    NVIC_DisableIRQ((IRQn_Type) PMBUS_TIMEOUT_IRQ);
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
}
```

### Frequency Selection for Timeout Handling

The assigned clock is crucial for the timeout handling because it samples the SCL line with a frequency twice as high as the frequency of the communication signal. For example, for the 400 kHz I2C speed, the minimal value of the HIGH pulse of the SCL is 0.6 us (period 0.12 us). In this case, the sample frequency is expected to be (1 / 0.12 us) * 2 = 1.666666 MHz. Considering the clock accuracy, the sampled frequency is expected to be slightly higher, for example, 1.8 MHz. The Reload value must be configured to 45000 to measure 25 ms with this input frequency.

> **Note:** For some I2C speeds, the reload value may be higher than 65535, which imposes limitations on the use of the 16-bit timer for high I2C speeds.

### Hardware Configuration of the SMBALERT Signal

According to the SMBUS spec, the SMBALERT# signal is a wired-AND signal. This means that the output of the combined signal is only true (or high) if all the individual input signals are true (or high). In the Wired-AND connection, multiple signals are connected together through a common wire or bus. Each signal is typically an open-collector or open-drain output, which means that they can only pull the signal low (to ground) but not drive it high. When all the signals are inactive (high impedance), the combined signal is pulled high by an external pull-up resistor. Use the Open-Drain-Drives-Low mode for the SMBALERT# signal to meet the requirements of the WIRED-AND connection. To determine the specific capabilities of this mode, consult the Technical Reference Manual (TRM) documentation.

