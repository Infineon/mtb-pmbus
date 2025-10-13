# PMBus Middleware Library

## Overview

The purpose of the PMBus middleware library is to provide a solution for implementing SMBus/PMBus target devices in embedded systems. PMBus (Power Management Bus) is an open standard digital power management protocol that enables communication with power conversion and management devices. Based on the SMBus protocol, PMBus extends its capabilities with a standardized command language designed specifically for power management applications.

The PMBus middleware provides a robust implementation that simplifies the development of PMBus-compliant devices, enabling users to focus on their specific application rather than on the communication protocol details.

Use the PMBus middleware for:
* Power supply monitoring and control
* Power conversion device management
* Battery management systems
* Power distribution systems
* Any application requiring standardized power management communication

### Features:

* **Target Mode Support** - Enables devices to respond to PMBus controller commands.
* **PEC (Packet Error Code) Support** - Ensures data integrity through error detection.
* **Comprehensive Protocol Support**:
  * SMBus Protocols: Quick Command, Send Byte, Receive Byte, Write Byte/Word, Read Byte/Word, Process Call, Block Write/Read, Block Write-Read Process Call, Write 32/64, Read 32/64
  * PMBus Zone Read and Zone Write Protocols
  * PMBus Group Command Protocol
* **Compile time options** - Allows the user to disable unused functionality to reduce memory consumption.
* **Page and Phase Management** - Built-in handling of pages and phases for complex power systems
* **Multi-instance Support** - Allows multiple PMBus devices on a single system
* **Timeout handling** - Middleware handles SMBus timeout error
* **Logging support** - Allows the user to enable different logging messages for debug purposes
* **Data Format conversion APIs** - Built-in APIs for converting data between PMBus LINEAR11/16 and Float32

## Quick Start

The [Quick Start Guide section of the PMBus Middleware API Reference Guide](https://infineon.github.io/mtb-pmbus/html/index.html#section_pmbus_quick_start)
describes step-by-step instructions to set up a SMBus/PMBus application.

## Supported Toolchains

Refer to [Release Notes](./RELEASE.md) for a list of supported toolchains.

## Supported Devices

* PSC3M5

## More information

For more information, refer to the following documents:
* [Release Notes](./RELEASE.md)
* [PMBus API Reference Guide](https://infineon.github.io/mtb-pmbus/html/index.html)
* [PMBus Home Page](https://pmbus.org/)
* [SMBus Home Page](https://www.smbus.org/)
* [ModusToolbox Software Environment, Quick Start Guide, Documentation, and Videos](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software/)
* [ModusToolbox Introduction](https://documentation.infineon.com/modustoolbox/docs/zhf1731521206417/index.html)
* [Code Examples for ModusToolbox Software](https://github.com/Infineon/Code-Examples-for-ModusToolbox-Software)
* [Infineon Technologies AG](https://www.infineon.com)

---
© 2025, Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation.
