# pyright: reportUndefinedVariable=false

Import("env")

from os.path import join


def merge_firmware(source, target, env):
    build_dir = env.subst("$BUILD_DIR")
    framework_dir = env.PioPlatform().get_package_dir(
        "framework-arduinoespressif32"
    )
    python = env.subst("$PYTHONEXE")
    uploader = env.subst("$UPLOADER")
    output = join(build_dir, "firmware-merged.bin")
    bootloader = join(build_dir, "bootloader.bin")
    partitions = join(build_dir, "partitions.bin")
    boot_app0 = join(framework_dir, "tools", "partitions", "boot_app0.bin")
    firmware = join(build_dir, "firmware.bin")

    command = (
        f'"{python}" "{uploader}" --chip esp32 merge-bin '
        f'-o "{output}" --flash-mode dio --flash-size 4MB '
        f'0x1000 "{bootloader}" 0x8000 "{partitions}" '
        f'0xe000 "{boot_app0}" 0x10000 "{firmware}"'
    )
    env.Execute(command)


env.AddPostAction("$BUILD_DIR/" + "$" + "{PROGNAME}.bin", merge_firmware)
