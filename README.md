# PMBus Middleware Library

# Overview

The purpose of the PMBus middleware library is to provide a solution for implementing SMBus/PMBus target devices in embedded systems. PMBus (Power Management Bus) is an open standard digital power management protocol that enables communication with power conversion and management devices. Based on the SMBus protocol, PMBus extends its capabilities with a standardized command language designed specifically for power management applications.

The PMBus middleware provides a robust implementation that simplifies the development of PMBus-compliant devices, enabling users to focus on their specific application needs rather than the details of the communication protocol.

Use the PMBus middleware for:
* Power supply monitoring and control
* Power conversion device management
* Battery management systems
* Power distribution systems
* Any application requiring standardized power management communication

# Features

The PMBus middleware provides support for SMBus/PMBus communication:

**Target Mode features:**
- Built-in handling of pages and phases
- PEC (Packet Error Code) support
- SMBus protocols: Quick Command, Send Byte, Receive Byte,
  Write Byte/Word, Read Byte/Word, Process Call, Block Write/Read, Block Write-Read
  Process Call, SMBus Host Notify Protocol, Write 32/64, and Read 32/64
- Zone Read and Zone Write protocols
- Extended Command Protocol
- Group Command Protocol
- Multi-instance support
- Timeout detection mechanisms

**Controller Mode features:**
- APIs to execute SMBus protocols: Quick Command with WR direction, Send Byte, Receive Byte,
  Write Byte/Word, Read Byte/Word, Process Call, Block Write/Read, Block Write-Read
  Process Call, SMBus Host Notify Protocol, Write 32/64, Read 32/64
- API to execute generic SMBus/PMBus transactions
- PEC (Packet Error Code) support
- Multi-instance support
- Timeout detection mechanisms
- Host Mode with Host Notify Protocol support

# When to Use

Use the PMBus middleware when developing embedded applications that require:

* **Power supply monitoring and control** - Implement digital feedback loops, voltage/current monitoring, and fault management for DC-DC converters, AC-DC power supplies, and voltage regulators
* **Power conversion device management** - Configure and monitor point-of-load (POL) converters, multiphase controllers, and hot-swap controllers via standardized PMBus commands
* **Battery management systems** - Communicate with battery chargers, fuel gauges, and protection circuits using SMBus/PMBus protocols
* **Power distribution systems** - Build intelligent power distribution units (PDUs) with real-time telemetry, sequencing control, and fault detection
* **Server and datacenter power management** - Implement PMBus-compliant power shelves, rack power modules, and power supply units (PSUs)
* **Industrial automation** - Integrate digital power management into PLCs, motor drives, and industrial power systems

The middleware is ideal when you need standardized SMBus/PMBus protocol compliance with minimal development effort, whether implementing a target device that responds to controller commands or a controller that manages multiple power devices on the bus.

# Prerequisites

## Supported Devices

* PSOC&trade; Control C3 M7/M8/P7/P8
* PSOC&trade; Control C3 M3/M5/P2/P5
* PSOC&trade; Control C3 M6/P6

# Quick Start

This section provides step-by-step guides to quickly get started with the PMBus middleware for both Target and Controller modes.

## Target Mode Guides

- [Target Mode Quick Start Guide](guides/target_mode_quick_start.md) - Manual configuration approach
- [Target Mode Quick Start Guide (Using Solution Personality)](guides/target_mode_personality_quick_start.md) - Using Device Configurator
- [Verify Target Mode Workability](guides/verify_target_mode.md) - Testing and verification steps

## Controller Mode Guides

- [Controller Mode Quick Start Guide](guides/controller_mode_quick_start.md) - Manual configuration approach
- [Controller Mode Quick Start Guide (Using Solution Personality)](guides/controller_mode_personality_quick_start.md) - Using Device Configurator
- [Verify Controller Mode Workability](guides/verify_controller_mode.md) - Testing and verification steps

## Migration Steps from Manual Configuration

If your project currently uses manual PMBus configuration and you are moving to the Solution personality flow:

1. Back up any local PMBus command tables, callback implementations, and middleware compile-time options.
2. Create a PMBus instance in Device Configurator using the Solution personality.
3. Reapply project-specific settings in the PMBus Configurator (mode, address, data rate, Pages/Phases, optional features).
4. Compare generated configuration against your previous manual setup and restore any application-level custom logic.
5. Validate with the relevant quick-start verification guide:
    - [Verify Target Mode Workability](guides/verify_target_mode.md)
    - [Verify Controller Mode Workability](guides/verify_controller_mode.md)

---

# Design Considerations

Design Considerations are documented in the following pages:

- [Design Considerations: Setup and Initialization](guides/design_considerations_setup_and_initialization.md)
- [Design Considerations: Protocols and Features](guides/design_considerations_protocols_and_features.md)
- [Design Considerations: Hardware and Timeout](guides/design_considerations_hardware_and_timeout.md)

---

# Compatible Software

Refer to [Release Notes](./RELEASE.md) for a list of supported toolchains.

# Industry Standards and Compliance

The PMBus middleware is designed to comply with the following industry standards:

- [PMBus Power System Management Protocol Specification](https://pmbus.org/current-specifications/) - Revision 1.4
- [System Management Bus (SMBus) Specification](https://www.smbus.org/specs/) - Version 3.3.1

## MISRA-C:2012 Compliance

This section describes MISRA-C:2012 compliance and deviations for the PMBus.

MISRA stands for Motor Industry Software Reliability Association. The MISRA specification covers a set of 10 mandatory rules, 110 required rules and 39 advisory rules that apply to the firmware design and has been put together by the Automotive Industry to enhance the quality and robustness of the firmware code embedded in automotive devices.

The MISRA specification defines two categories of deviations (see section 5.4 of the MISRA-C:2012 specification):
* **Project Deviations** - deviations applicable for a particular class of circumstances.
* **Specific Deviations** - deviations applicable for a single instance in a single file.

Project Deviations are documented in the current section below.

Specific deviations are documented in the source code, close to the deviation occurrence. For each deviation, a special macro identifies the relevant rule or directive number, and reason.

### Verification Environment

This section provides a MISRA compliance analysis environment description.

<table style="border: 1px solid black; border-collapse: collapse;">
<tr>
<th style="border: 1px solid black; padding: 8px;">Component</th>
<th style="border: 1px solid black; padding: 8px;">Name</th>
<th style="border: 1px solid black; padding: 8px;">Version</th>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Test Specification</td>
<td style="border: 1px solid black; padding: 8px;">MISRA-C:2012 Guidelines for the use of the C language in critical systems</td>
<td style="border: 1px solid black; padding: 8px;">March 2013</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">MISRA Checking Tool</td>
<td style="border: 1px solid black; padding: 8px;">Coverity Static Analysis Tool</td>
<td style="border: 1px solid black; padding: 8px;">2022.12.0</td>
</tr>
</table>

### Project Deviation

The list of deviated required rules is provided in the table below. Advisory rules deviation is not documented, as not required per MISRA specification.

<table style="border: 1px solid black; border-collapse: collapse;">
<tr>
<th style="border: 1px solid black; padding: 8px;">Rule ID</th>
<th style="border: 1px solid black; padding: 8px;">Rule Description</th>
<th style="border: 1px solid black; padding: 8px;">Description of Deviation(s)</th>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Rule 3.1</td>
<td style="border: 1px solid black; padding: 8px;">The character sequences /* and // shall not be used within a comment.</td>
<td style="border: 1px solid black; padding: 8px;">Required. Using of the special comment symbols is needed for Doxygen comment support; it does not have any impact.</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Rule 5.1</td>
<td style="border: 1px solid black; padding: 8px;">External identifiers shall be distinct.</td>
<td style="border: 1px solid black; padding: 8px;">Required. Toolchains from "Supported Software" and "Tools" documentation section are verified to work with functions whose names have similar first 31 symbols.</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Rule 5.5</td>
<td style="border: 1px solid black; padding: 8px;">Identifiers shall be distinct from macro names.</td>
<td style="border: 1px solid black; padding: 8px;">Required. This rule applies to the ISO:C90 standard. The middleware conforms to ISO:C99, which does not require this limitation.</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Rule 5.8</td>
<td style="border: 1px solid black; padding: 8px;">Identifiers that define objects or functions with external linkage shall be unique.</td>
<td style="border: 1px solid black; padding: 8px;">Required. During the code analysis, the same source files are compiled multiple times with device-specific options. All object and function identifiers are unique for each specific run.</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Rule 5.9</td>
<td style="border: 1px solid black; padding: 8px;">Identifiers for objects with internal linkage shall be unique.</td>
<td style="border: 1px solid black; padding: 8px;">Required. During the code analysis, the same source files are compiled multiple times with device-specific options. All object and function identifiers are actually unique for each specific run.</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Rule 8.6</td>
<td style="border: 1px solid black; padding: 8px;">An identifier with external linkage shall have exactly one external definition.</td>
<td style="border: 1px solid black; padding: 8px;">Required. During the code analysis, the same source files are compiled multiple times with device-specific options. All object and function identifiers are unique for each specific run.</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Rule 11.8</td>
<td style="border: 1px solid black; padding: 8px;">A cast must not remove any const or volatile qualifications from type "pointed" to "by a pointer"</td>
<td style="border: 1px solid black; padding: 8px;">Required. Casting a volatile pointer to non-volatile is required in an interrupt context to manipulate a user-provided mtb_pmbus_stc_t object. Volatile is preserved during the variables lifecycle to prevent compiler optimization.</td>
</tr>
<tr>
<td style="border: 1px solid black; padding: 8px;">Rule 21.6</td>
<td style="border: 1px solid black; padding: 8px;">The Standard Library input/output functions shall not be used.</td>
<td style="border: 1px solid black; padding: 8px;">Required. Deviated since usage of printf is required for logging.</td>
</tr>
</table>

# Release Notes and Changelog
- [RELEASE.md](./RELEASE.md) - Detailed release notes for all versions

# License
This software is provided under the **Infineon End User License Agreement (EULA)**. Use, reproduction, and distribution are permitted solely as described in the accompanying license agreement.

- **[LICENSE](LICENSE)** - Infineon End User License Agreement

# More information

For more information, refer to the following documents:
* [PMBus Home Page](https://pmbus.org/)
* [SMBus Home Page](https://www.smbus.org/)

---
© 2026, Infineon Technologies AG, or an affiliate of Infineon Technologies AG.  All rights reserved.

