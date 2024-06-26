#!/usr/bin/env python

import os, sys, time, datetime
import argparse

try:
    from intelhex import IntelHex
except ImportError:
    print("ERROR: You need the Python package `intelhex`! https://github.com/python-intelhex/intelhex \r\nTo install it, maybe run the command\r\n        pip install intelhex")
    input("press the ENTER key to exit")
    sys.exit()

def main():
    global verbose

    got_file = False
    if len(sys.argv) == 2:
        if os.path.isfile(sys.argv[1]):
            got_file = True

    parser = argparse.ArgumentParser()
    if got_file == False:
        parser.add_argument("-f", "--firmware",                     default="../bin",     type=str, help="firmware file (or directory)")
    parser.add_argument("-a", "--fwaddr",     metavar="fwaddr",     default="",           type=str, help="firmware start address")
    parser.add_argument("-e", "--eepromaddr", metavar="eepromaddr", default="",           type=str, help="eeprom address")
    parser.add_argument("-m", "--addrmulti",  metavar="addrmulti",  default="",           type=str, help="address multiplier")
    parser.add_argument("-v", "--verbose",                          action="store_true",            help="verbose")
    args = parser.parse_args()

    if got_file:
        args.firmware = sys.argv[1]
        print("input file: \"%s\"" % args.firmware)

    verbose = args.verbose

    download_bootloaders()

    files_to_do = []

    if os.path.isdir(args.firmware):
        import glob
        g = glob.glob(os.path.join(args.firmware, "*.hex"))
        files_to_do.extend(g)
    else:
        files_to_do.append(args.firmware)

    for f in files_to_do:
        proc_hex_file(f)

def proc_hex_file(fpath):
    global verbose
    global dl_dir
    if fpath.endswith(".with-bootloader.hex"):
        return
    print("processing file: \"%s\"" % fpath)
    fw_fullpath = os.path.abspath(fpath)
    if os.path.isfile(fw_fullpath):
        fw_basename = os.path.basename(fw_fullpath)
        fw_namesplit = os.path.splitext(fw_basename)
        fw_justname = fw_namesplit[0].strip()
        fw_ext = fw_namesplit[1].strip().lower()
        fw_dir = os.path.dirname(fw_fullpath)
        if fw_ext != ".hex":
            print("ERROR: the file must be a *.hex file type")
            return
        else:
            ndir = os.path.join(fw_dir, "with-bootloader")
            if os.path.isdir(ndir) == False:
                os.makedirs(ndir)
                if verbose:
                    print("created save directory \"%s\"" % ndir)

            try:
                fw_ihex = IntelHex(fw_fullpath)
            except Exception as ex:
                print("ERROR: something went wrong while loading the file \"%s\"" % fw_fullpath)
                try:
                    exc_type, value, exc_traceback = sys.exc_info()
                    print(exc_type)
                    print(value)
                    print(sys.exc_info()[2])
                    import traceback
                    traceback.print_exc()
                except:
                    return

            if verbose:
                print("firmware input file: %s (%s) (%s)" % (fw_justname, fw_ext, fw_dir))

            fw_new_path = os.path.join(ndir, fw_justname + ".with-bootloader.hex")

            if verbose:
                print("firmware loaded - addr from 0x%08X to 0x%08X" % (fw_ihex.minaddr(), fw_ihex.maxaddr()))

            fwfile_id_addr = 0x08001100 # reference the file `version.c` and the linker script
            fwfile_id = 0
            fwfile_id      += int(fw_ihex[fwfile_id_addr])
            fwfile_id_addr += 1
            fwfile_id      += int(fw_ihex[fwfile_id_addr]) << 8
            fwfile_id_addr += 1
            fwfile_id      += int(fw_ihex[fwfile_id_addr]) << 16
            fwfile_id_addr += 1
            fwfile_id      += int(fw_ihex[fwfile_id_addr]) << 24
            pin_num = (fwfile_id & 0xFF00) >> 8
            port_num = int((fwfile_id & 0xFF) / 4)
            if verbose:
                print("embedded identification: 0x%08X , GPIO %s  pin %u" % (fwfile_id, chr(ord('A') + port_num), pin_num))

            bootloader_file = None
            mcu_id = int((fwfile_id & 0x00FF0000) >> 16)
            pin_str = "p%s%u" % (chr(ord('a') + port_num), pin_num)
            if mcu_id == 0x51:
                bootloader_file = "bootloader_f051_%s.bin" % pin_str
                fwaddr     = 0x08001000
                eep_addr   = 0x7C00
            elif mcu_id == 0x71:
                bootloader_file = "bootloader_g071_64k.bin"
                fwaddr     = 0x08001000
                eep_addr   = 0xF800
            elif mcu_id == 0x21:
                bootloader_file = "bootloader_at421_%s.hex" % pin_str
                fwaddr     = 0x08001000
                eep_addr   = 0x7C00
            elif mcu_id == 0x35:
                bootloader_file = "bootloader_gd32f350_%s.bin" % pin_str
                fwaddr     = 0x08001000
                eep_addr   = 0xF800

            if bootloader_file is not None:
                bootloader_file = os.path.join(dl_dir, bootloader_file)
                if verbose:
                    print("desired bootloader file is \"%s\"" % bootloader_file)

                flash_start = 0x08000000
                split_tup = os.path.splitext(bootloader_file)
                bl_file_ext = split_tup[1].lower()

                if bl_file_ext == ".bin":
                    with open(bootloader_file, "rb") as blf:
                        ba = bytearray(blf.read())
                elif bl_file_ext == ".hex":
                    bl_ihex = IntelHex(bootloader_file)
                    if bl_ihex.minaddr() != flash_start:
                        raise Exception("bootloader ihex start address is wrong: 0x%08X" % bl_ihex.minaddr())
                    ba = bl_ihex.tobinarray(start = flash_start)
                else:
                    raise Exception("unknown bootloader file extension \"*%s\"" % bl_file_ext)

                j = flash_start
                for d in ba:
                    fw_ihex[j] = d
                    j += 1

                if mcu_id == 0x51:
                    # embed the correct bootloader version into the EEPROM region so that AM32 does not erase it
                    blver = fw_ihex[flash_start + 0x0FFC]
                    blver_addr = flash_start + eep_addr + 2
                    fw_ihex[blver_addr] = blver

                fw_ihex.tofile(fw_new_path, format='hex')
                print("saved new file: \"%s\"" % fw_new_path)

    else:
        print("ERROR: the file \"%s\" does not exist" % fw_fullpath)
        return

def download_bootloaders():
    global verbose
    global dl_dir
    import urllib.request
    urls = [["https://github.com/AlkaMotors/AM32_Bootloader_F051/releases/download/v11/PA2_BOOTLOADER_V11.bin", "bootloader_f051_pa2.bin"],
            ["https://github.com/AlkaMotors/AM32_Bootloader_F051/releases/download/v11/PB4_BOOTLOADER_V11.bin", "bootloader_f051_pb4.bin"],
            ["https://github.com/AlkaMotors/g071Bootloader/releases/download/v7/G071_Bootloader_64_v7.bin"    , "bootloader_g071_64k.bin"],
            ["https://raw.githubusercontent.com/AlkaMotors/AT32F421_AM32_Bootloader/922493dd0e54bae1c92cecdd9fd5472ce099dd21/Objects/F421_PA2_BOOTLOADER_V2.hex", "bootloader_at421_pa2.hex"],
            ["https://raw.githubusercontent.com/AlkaMotors/AT32F421_AM32_Bootloader/922493dd0e54bae1c92cecdd9fd5472ce099dd21/Objects/F421_PB4_BOOTLOADER_V2.hex", "bootloader_at421_pb4.hex"],
            ["https://raw.githubusercontent.com/frank26080115/g071Bootloader/gd32f350/AM32_GD32F350_BOOTLOADER_PA2_V8.bin", "bootloader_gd32f350_pa2.bin"],
            ["https://raw.githubusercontent.com/frank26080115/g071Bootloader/gd32f350/AM32_GD32F350_BOOTLOADER_PB4_V8.bin", "bootloader_gd32f350_pb4.bin"],
            ]
    dl_dir = "downloaded_bootloaders"
    if os.path.exists(dl_dir) == False:
        os.makedirs(dl_dir)
        if verbose:
            print("created bootloader download directory \"%s\"" % dl_dir)
    for i in urls:
        npath = os.path.join(dl_dir, i[1])
        if os.path.isfile(npath) == False:
            urllib.request.urlretrieve(i[0], npath)
            if verbose:
                print("download bootloader file \"%s\" to \"%s\"" % (i[0], i[1]))
        else:
            if verbose:
                print("skip download bootloader file \"%s\"" % i[1])

def quit_nicely(c = 0):
    # we don't want the window to disappear too quickly
    if os.name == "nt":
        print("press any key to exit")
        os.system('pause >nul')
    else:
        #print("press any key to exit")
        #os.system('read -n 1 key')
        print("press the ENTER key to exit")
        input()
    try:
        quit()
    except:
        sys.exit(c)

if __name__ == '__main__':
    try:
        main()
    except Exception as ex:
        print("ERROR: unexpected fatal exception occured")
        try:
            exc_type, value, exc_traceback = sys.exc_info()
            print(exc_type)
            print(value)
            print(sys.exc_info()[2])
            import traceback
            traceback.print_exc()
        except:
            pass
        quit_nicely(-1)
