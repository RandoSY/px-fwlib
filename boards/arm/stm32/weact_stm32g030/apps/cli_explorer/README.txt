WeAct STM32G030 CLI Explorer - Build Instructions
===================================================

Target board
------------
WeAct Studio STM32G030F6P6/F8P6 core board.

The STM32G030F6P6 is the 32 KB flash device. The STM32G030F8P6 is the
64 KB flash device.

Required tools
--------------
Use the Arm GNU Toolchain and GNU make already used by px-fwlib. The tested
toolchain was Arm GNU Toolchain 15.2.1.

Build the STM32G030F6P6 (32 KB)
--------------------------------
Open a terminal in this directory:

    boards\arm\stm32\weact_stm32g030\apps\cli_explorer

Build the recommended optimized image:

    make clean
    make build=release

Build a debug image instead:

    make build=debug

Build the STM32G030F8P6 (64 KB)
--------------------------------
Always clean when changing the configured flash size:

    make build=release clean
    make build=release FLASH_SIZE=64k

or:

    make build=debug clean
    make build=debug FLASH_SIZE=64k

Build output
------------
The build creates these files in BUILD_RELEASE or BUILD_DEBUG:

    cli_explorer.elf   Debuggable ELF image
    cli_explorer.hex   Intel HEX image for a programmer
    cli_explorer.bin   Raw binary image
    cli_explorer.map   Link map
    cli_explorer.lss   Disassembly/listing
    cli_explorer.sym   Symbol table

The release image size is approximately 28 KB. It fits the STM32G030F6P6
32 KB flash device.

Flashing
--------
Flash cli_explorer.hex or cli_explorer.bin using the preferred SWD tool.
The board's SWD pins are PA13 (SWDIO) and PA14 (SWCLK). PA14 is also the
active-high user button, so button activity can interfere with an attached
debugger.

Terminal setup
--------------
Connect a 3.3 V USB-UART adapter:

    USART1 TX: PA11 header net
    USART1 RX: PA12 header net
    GND:       board GND

Use:

    115200 baud, 8 data bits, no parity, 1 stop bit

The application prints a startup banner. Type:

    help

to display the complete command tree. The CLI supports command history,
VT100 editing, echo, and filtered help such as:

    help adc
    help uart

Initial hardware checks
-----------------------
Run these commands first:

    info
    gpio info all
    adc info
    adc s 0
    led on
    led off
    beep 250
    rtc r

ADC test
--------
Connect a known voltage between 0 V and the board's 3.3 V supply to PA0.
Then run:

    adc s 0
    adc bin 0
    adc all

The ADC output includes the raw 12-bit value, hexadecimal value, calculated
millivolts, and binary representation. The default voltage reference
assumption is 3300 mV. Adjust it when necessary:

    adc v 3300

Continuous sampling can be stopped by pressing the PA14 button or sending a
byte over the terminal.

UART loopback test
------------------
USART1 is also the CLI console. To test receive or exchange commands, connect
PA11 TX to PA12 RX with an external jumper or attach another UART device.

    uart info
    uart w 0x55 0xaa
    uart x 2 100 0x12 0x34
    uart s 250 "test" crlf
    uart err

Changing the UART configuration also changes the command terminal:

    uart cfg 1 115200 8 n 1

SPI test
--------
SPI1 uses:

    PA5: SCK
    PA6: MISO
    PA7: MOSI
    PA8: default manual chip select

Attach an SPI slave before issuing transfers:

    spi cfg
    spi x 0x9f 0x00 0x00 0x00
    spi cs lo
    spi cs hi

RTC test
--------
The board LSE crystal on PC14/PC15 supplies the RTC clock:

    rtc r
    rtc w 26-08-22 14:30:00
    rtc r

ADC meter example
-----------------
The separate ADC meter is located at:

    boards\arm\stm32\weact_stm32g030\examples\adc_meter

Build the 32 KB version:

    cd ..\..\examples\adc_meter
    make clean
    make build=release

The meter uses PA0 for ADC input, PA14 for the key, PA1 for RED, PA2 for
GREEN, and PA3 for the buzzer. It displays the twelve ADC bits MSB first:

    GREEN = 1
    RED   = 0

Safety notes
------------
The mem commands directly access the STM32 memory map. Invalid addresses or
unsafe peripheral writes can fault the processor or change hardware state.
Use only addresses from the STM32G030 reference manual.

The CLI explorer has been compile-tested for both x6 and x8 flash sizes. A
physical board and connected peripherals are still required for electrical
and runtime validation.

Pull-request automation
-----------------------
The package root contains prepare_stm32g030_pr.ps1. From the package root,
the author can apply the overlay, run all builds, create a branch, commit, push,
and open a GitHub pull request with:

    pwsh .\prepare_stm32g030_pr.ps1 `
        -RepositoryPath C:\src\px-fwlib `
        -PackagePath .\stm32g030-cli-explorer-pr-package.zip `
        -BaseBranch main `
        -RunBuilds `
        -Commit `
        -Push `
        -CreatePullRequest

Commit, push, and pull-request creation are opt-in switches. The script refuses
to use a dirty checkout unless -AllowDirty is explicitly supplied.
