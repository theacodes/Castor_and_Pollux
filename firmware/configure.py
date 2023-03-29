#!/usr/bin/env python3

import argparse
import pathlib

from wintertools import buildgen
from wintertools.third_party import ninja_syntax

# Check the python version before doing anything else.
buildgen.check_python_version()

# Make sure we're in the right directory.
buildgen.ensure_directory()


# MCU configuration


# The MCU has a few variants, called "devices" in the CMSIS headers. Each one
# has its own header file and linker script. Gemini uses the SAMD21G18A.
MCU = "SAMD21G18A"

# Size of the MCU variant's memories. These are all specified in the datasheet
# for the SAMD21G18A
FLASH_SIZE = 0x40000
RAM_SIZE = 0x8000

# The amount of SRAM to set aside for the program stack. This is
# application-specific. Wintertools enables warnings about using more stack
# than available to help tune this.
STACK_SIZE = 0x800

# Each device has a seperate linker script. This was copied from
# third_party/samd21/gcc
LINKER_SCRIPT = f"scripts/{MCU.lower()}.ld"


# Gemini-specific sources, includes, and defines.


PROGRAM = "gemini-firmware"

SRCS = [
    "src/**/*.c",
    # The generated build info source file. It provides various details about
    # the build to the program. The header is at src/lib/gem_build_info.h
    "build/generated_build_info.c",
    # libwinter is common winterbloom code.
    "third_party/libwinter/samd/*.c",
    "third_party/libwinter/samd/samd21/*.c",
    "third_party/libwinter/teeth.c",
    "third_party/libwinter/wntr_assert.c",
    "third_party/libwinter/wntr_bezier.c",
    "third_party/libwinter/wntr_button.c",
    "third_party/libwinter/wntr_colorspace.c",
    "third_party/libwinter/wntr_error_correction.c",
    "third_party/libwinter/wntr_midi_core.c",
    "third_party/libwinter/wntr_midi_sysex_dispatcher.c",
    "third_party/libwinter/wntr_periodic_waveform.c",
    "third_party/libwinter/wntr_random.c",
    "third_party/libwinter/wntr_ticks.c",
    # Tiny printf
    "third_party/printf/*.c",
    # For fix16_t
    "third_party/libfixmath/*.c",
    # Structy
    "third_party/structy/*.c",
    # SEGGER's RTT for transferring printf output back to the host.
    "third_party/rtt/SEGGER_RTT.c",
    # We're a bit more picky about TinyUSB's sources, since we only use a very
    # small bit of TinyUSB's functionality.
    "third_party/tinyusb/src/tusb.c",
    "third_party/tinyusb/src/class/midi/midi_device.c",
    "third_party/tinyusb/src/common/tusb_fifo.c",
    "third_party/tinyusb/src/device/usbd.c",
    "third_party/tinyusb/src/device/usbd_control.c",
    "third_party/tinyusb/src/portable/microchip/samd/dcd_samd.c",
]

INCLUDES = [
    "src/config",
    "third_party/samd21/include",
    "third_party/cmsis/include",
]

DEFINES = buildgen.SAMD21.defines(MCU)

DEFINES.update(
    dict(
        # Defines for TinyUSB
        CFG_TUSB_MCU="OPT_MCU_SAMD21",
        # Defines for wntr:
        # - Enable the ARM Microtrace Buffer which can help with debugging.
        WNTR_ENABLE_MTB=1,
        # Set the MIDI SysEx identifier
        WNTR_MIDI_SYSEX_IDENTIFIER=0x77,
        # Defines for libfixmath:
        # - Use a faster, less accurate sin() function.
        FIXMATH_FAST_SIN=1,
        # - Don't cache look-up tables, Gemini can't fit them all in RAM.
        FIXMATH_NO_CACHE=1,
        # Defines for printf:
        # - Disable float support since we don't use floats in Gemini.
        PRINTF_DISABLE_SUPPORT_FLOAT=1,
        PRINTF_DISABLE_SUPPORT_EXPONENTIAL=1,
    )
)


# Toolchain configuration. Wintertools does most of the work here.


COMMON_FLAGS = buildgen.SAMD21.common_flags()

COMPILE_FLAGS = buildgen.SAMD21.cc_flags() + [
    # Check each function's stack usage against the configured stack size to
    # avoid overflowing the stack.
    f"-Wstack-usage={STACK_SIZE}",
]

DEBUG_COMPILER_FLAGS = [
    "-ggdb3 -Og",
]

RELEASE_COMPILER_FLAGS = [
    # Generally you'd want to use -Os to optimize for size, but since Gemini
    # is super tiny we can optimize for speed instead.
    "-O2"
]

LINK_FLAGS = buildgen.SAMD21.ld_flags() + [
    f"-Wl,--script={LINKER_SCRIPT}",
    # The linker script expects a __stack_size__ symbol to know how much space
    # to set aside for the stack.
    f"-Wl,--defsym=__stack_size__={STACK_SIZE}",
]

DEBUG_DEFINES = dict(DEBUG=1)

RELEASE_DEFINES = dict(NDEBUG=1)


# Buildfile generation


def generate_build(configuration, run_generators, enable_tidy, enable_format):
    srcs = buildgen.expand_srcs(SRCS)
    INCLUDES.extend(buildgen.includes_from_srcs(srcs))

    compiler_flags = COMMON_FLAGS + COMPILE_FLAGS
    linker_flags = COMMON_FLAGS + LINK_FLAGS

    if configuration == "debug":
        DEFINES.update(DEBUG_DEFINES)
        compiler_flags += DEBUG_COMPILER_FLAGS
    elif configuration == "release":
        DEFINES.update(RELEASE_DEFINES)
        compiler_flags += DEBUG_COMPILER_FLAGS

    buildfile_path = pathlib.Path("./build.ninja")
    buildfile = buildfile_path.open("w")
    writer = ninja_syntax.Writer(buildfile)

    # Global variables

    writer.comment("This is generated by configure.py- don't edit it directly!")
    writer.newline()

    buildgen.toolchain_variables(
        writer,
        cc_flags=compiler_flags,
        linker_flags=linker_flags,
        includes=INCLUDES,
        defines=DEFINES,
    )

    # Use wintertools' common rules for compiling and such.
    buildgen.common_rules(writer)

    # Builds for compiling, linking, and outputting elf, bin, and uf2, and
    # checking size.
    objects = buildgen.compile_build(writer, srcs)
    buildgen.link_build(writer, PROGRAM, objects)
    buildgen.binary_formats_build(writer, PROGRAM)
    buildgen.size_build(writer, PROGRAM, FLASH_SIZE, RAM_SIZE)

    # Builds for generated files

    # Build info is always generated, even if generators are disabled.
    buildgen.build_info_build(writer, configuration)

    if run_generators:
        # Structies
        buildgen.structy_build(
            writer,
            "data/gem_settings.structy",
            c="src/generated",
            py="../factory/libgemini",
            js="../user_guide/docs/scripts",
        )

        buildgen.structy_build(
            writer,
            "data/gem_monitor_update.structy",
            c="src/generated",
            py="../factory/libgemini",
        )

        # Look-up tables
        buildgen.py_generated_file_build(
            writer,
            "scripts/generate_ramp_table_data.py",
            "src/generated/gem_ramp_table_data.c",
        )

    # Formatting and linting
    if enable_format:
        format_files = list(pathlib.Path(".").glob("src/**/*.[c,h]"))
        buildgen.clang_format_build(writer, format_files)

    if enable_tidy:
        tidy_files = list(pathlib.Path(".").glob("src/**/*.c"))
        buildgen.clang_tidy_build(writer, tidy_files)

    # Special reconfigure build
    buildgen.reconfigure_build(writer)

    # All done. :)
    writer.close()


def main():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument("--config", choices=["debug", "release"], default="debug")
    parser.add_argument("--skip-checks", action="store_true", default=False)
    parser.add_argument("--no-generators", action="store_true", default=False)
    parser.add_argument("--enable-tidy", action="store_true", default=False)
    parser.add_argument("--no-format", action="store_true", default=False)

    args = parser.parse_args()

    if not args.skip_checks:
        buildgen.check_gcc_version()

    generate_build(
        args.config, not args.no_generators, args.enable_tidy, not args.no_format
    )

    print("Created build.ninja")


if __name__ == "__main__":
    main()
