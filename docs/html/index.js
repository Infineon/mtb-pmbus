var index =
[
    [ "Overview", "index.html#overview", null ],
    [ "Features", "index.html#features", null ],
    [ "When to Use", "index.html#when-to-use", null ],
    [ "Prerequisites", "index.html#prerequisites", [
      [ "Supported Devices", "index.html#supported-devices", null ]
    ] ],
    [ "Quick Start", "index.html#quick-start", [
      [ "Target Mode Guides", "index.html#target-mode-guides", null ],
      [ "Controller Mode Guides", "index.html#controller-mode-guides", null ]
    ] ],
    [ "Design Considerations", "index.html#design-considerations", [
      [ "Solution Configuration", "index.html#solution-configuration", null ],
      [ "Initialization Sequence", "index.html#initialization-sequence", [
        [ "Target Mode", "index.html#target-mode", null ],
        [ "Controller Mode", "index.html#controller-mode", null ]
      ] ],
      [ "Communication Protocols", "index.html#communication-protocols", [
        [ "Target Mode", "index.html#target-mode-1", null ],
        [ "Controller Mode", "index.html#controller-mode-1", null ]
      ] ],
      [ "Timeout Handling", "index.html#timeout-handling", null ],
      [ "Callback Handling", "index.html#callback-handling", [
        [ "Target Mode", "index.html#target-mode-2", [
          [ "Command Callback", "index.html#command-callback", null ],
          [ "General Callback", "index.html#general-callback", null ]
        ] ],
        [ "Controller Mode", "index.html#controller-mode-2", [
          [ "Event Callback", "index.html#event-callback", null ]
        ] ]
      ] ],
      [ "Command Organization", "index.html#command-organization", [
        [ "Command Capabilities", "index.html#command-capabilities", null ],
        [ "Implemented Commands", "index.html#implemented-commands", null ],
        [ "Extended Commands Support", "index.html#extended-commands-support", null ]
      ] ],
      [ "Data Format Conversion Functions", "index.html#data-format-conversion-functions", null ],
      [ "Optional Signals", "index.html#optional-signals", null ],
      [ "Logging", "index.html#logging", null ],
      [ "Low Power Support", "index.html#low-power-support", null ],
      [ "Compile Time Options", "index.html#compile-time-options", null ],
      [ "Host Notify Protocol", "index.html#host-notify-protocol", [
        [ "Target Mode", "index.html#target-mode-3", null ],
        [ "Controller Mode", "index.html#controller-mode-3", null ]
      ] ],
      [ "Hardware-Dependent Layer", "index.html#hardware-dependent-layer", [
        [ "Limitations and Exceptions of the Zone Feature (Target Mode)", "index.html#limitations-and-exceptions-of-the-zone-feature-target-mode", null ],
        [ "Quick Command with Read Direction Limitation (Controller Mode)", "index.html#quick-command-with-read-direction-limitation-controller-mode", null ],
        [ "SCB I2C Configuration", "index.html#scb-i2c-configuration", [
          [ "Target Mode", "index.html#target-mode-4", null ],
          [ "Controller Mode", "index.html#controller-mode-4", null ]
        ] ],
        [ "Different Timeout Detection HW Implementations", "index.html#different-timeout-detection-hw-implementations", null ],
        [ "Timeout Detection Configuration", "index.html#timeout-detection-configuration", null ],
        [ "Frequency Selection for Timeout Handling", "index.html#frequency-selection-for-timeout-handling", null ],
        [ "Hardware Configuration of the SMBALERT Signal", "index.html#hardware-configuration-of-the-smbalert-signal", null ]
      ] ]
    ] ],
    [ "Compatible Software", "index.html#compatible-software", null ],
    [ "Industry Standards and Compliance", "index.html#industry-standards-and-compliance", [
      [ "MISRA-C:2012 Compliance", "index.html#misra-c2012-compliance", [
        [ "Verification Environment", "index.html#verification-environment", null ],
        [ "Project Deviation", "index.html#project-deviation", null ]
      ] ]
    ] ],
    [ "Release Notes and Changelog", "index.html#release-notes-and-changelog", null ],
    [ "License", "index.html#license", null ],
    [ "More information", "index.html#more-information", null ],
    [ "Target Mode Quick Start Guide", "guide_target_mode_quick_start.html", [
      [ "Add mtb-pmbus middleware to your project", "guide_target_mode_quick_start.html#autotoc_md1-add-mtb-pmbus-middleware-to-your-project", null ],
      [ "Configure SCB blocks and GPIO pins", "guide_target_mode_quick_start.html#autotoc_md2-configure-scb-blocks-and-gpio-pins", null ],
      [ "Add SMBus/PMBus code to your project", "guide_target_mode_quick_start.html#autotoc_md3-add-smbuspmbus-code-to-your-project", null ],
      [ "Verify Target Mode Workability", "guide_target_mode_quick_start.html#autotoc_md4-verify-target-mode-workability", null ]
    ] ],
    [ "Target Mode Quick Start Guide (Using Solution Personality)", "guide_target_mode_personality.html", [
      [ "Add mtb-pmbus middleware to your project", "guide_target_mode_personality.html#autotoc_md1-add-mtb-pmbus-middleware-to-your-project-1", null ],
      [ "Configure SCB blocks, timeout Timer, GPIO pins", "guide_target_mode_personality.html#autotoc_md2-configure-scb-blocks-timeout-timer-gpio-pins", null ],
      [ "Configure PMBus instance", "guide_target_mode_personality.html#autotoc_md3-configure-pmbus-instance", null ],
      [ "Add PMBus code to your project", "guide_target_mode_personality.html#autotoc_md4-add-pmbus-code-to-your-project", null ],
      [ "Verify Target Mode Workability", "guide_target_mode_personality.html#autotoc_md5-verify-target-mode-workability", null ]
    ] ],
    [ "Verify Target Mode Workability", "guide_verify_target_mode.html", [
      [ "Build and Program", "guide_verify_target_mode.html#build-and-program", null ],
      [ "Quick Command and Read 32 protocols test (SMBus/PMBus mode)", "guide_verify_target_mode.html#quick-command-and-read-32-protocols-test-smbuspmbus-mode", null ],
      [ "PAGE command and Write/Read Word protocols with page support test (PMBus mode only)", "guide_verify_target_mode.html#page-command-and-writeread-word-protocols-with-page-support-test-pmbus-mode-only", null ]
    ] ],
    [ "Controller Mode Quick Start Guide", "guide_controller_mode_quick_start.html", [
      [ "Add mtb-pmbus middleware to your project", "guide_controller_mode_quick_start.html#autotoc_md1-add-mtb-pmbus-middleware-to-your-project-2", null ],
      [ "Configure SCB blocks", "guide_controller_mode_quick_start.html#autotoc_md2-configure-scb-blocks", null ],
      [ "Add SMBus/PMBus controller code to your project", "guide_controller_mode_quick_start.html#autotoc_md3-add-smbuspmbus-controller-code-to-your-project", null ],
      [ "Verify Controller Mode Workability", "guide_controller_mode_quick_start.html#autotoc_md4-verify-controller-mode-workability", null ]
    ] ],
    [ "Controller Mode Quick Start Guide (Using Solution Personality)", "guide_controller_mode_personality.html", [
      [ "Add mtb-pmbus middleware to your project", "guide_controller_mode_personality.html#autotoc_md1-add-mtb-pmbus-middleware-to-your-project-3", null ],
      [ "Configure SCB blocks, timeout Timer, GPIO pins", "guide_controller_mode_personality.html#autotoc_md2-configure-scb-blocks-timeout-timer-gpio-pins-1", null ],
      [ "Configure PMBus instance", "guide_controller_mode_personality.html#autotoc_md3-configure-pmbus-instance-1", null ],
      [ "Add PMBus controller code to your project", "guide_controller_mode_personality.html#autotoc_md4-add-pmbus-controller-code-to-your-project", null ],
      [ "Verify Controller Mode Workability", "guide_controller_mode_personality.html#autotoc_md5-verify-controller-mode-workability", null ]
    ] ],
    [ "Verify Controller Mode Workability", "guide_verify_controller_mode.html", [
      [ "Build and Program", "guide_verify_controller_mode.html#build-and-program-1", null ],
      [ "Quick Command protocol test", "guide_verify_controller_mode.html#quick-command-protocol-test", null ],
      [ "Read 32 protocol test", "guide_verify_controller_mode.html#read-32-protocol-test", null ],
      [ "Generic transfer examples test", "guide_verify_controller_mode.html#generic-transfer-examples-test", null ]
    ] ]
];