# pyright: reportUndefinedVariable=false

Import("env")

from os.path import join


def mergeFirmware(source, target, env):
    buildDir = env.subst("$BUILD_DIR")

    frameworkDir = env.PioPlatform().get_package_dir(
        "framework-arduinoespressif32"
    )

    pythonPath = env.subst("$PYTHONEXE")
    uploader = env.subst("$UPLOADER")

    outputFile = join(buildDir, "firmware-merged.bin")
    bootloader = join(buildDir, "bootloader.bin")
    partitions = join(buildDir, "partitions.bin")
    firmware = join(buildDir, "firmware.bin")

    bootApp = join(
        frameworkDir,
        "tools",
        "partitions",
        "boot_app0.bin"
    )

    command = (
        f'"{pythonPath}" "{uploader}" --chip esp32 merge-bin '
        f'-o "{outputFile}" --flash-mode dio --flash-size 4MB '
        f'0x1000 "{bootloader}" '
        f'0x8000 "{partitions}" '
        f'0xe000 "{bootApp}" '
        f'0x10000 "{firmware}"'
    )

    env.Execute(command)


firmwarePath = "$BUILD_DIR/${PROGNAME}.bin"

env.AddPostAction(firmwarePath, mergeFirmware)