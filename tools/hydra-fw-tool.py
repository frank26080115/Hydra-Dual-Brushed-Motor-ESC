#!/usr/bin/env python

# to build EXE for Windows: `pyinstaller37 -F --exclude-module _bootlocale --hidden-import=pkg_resources.py2_warn hydra-fw-tool.py`

import argparse
import os, sys, time, datetime, re

try:
    from intelhex import IntelHex
except ImportError:
    print("ERROR: You need the Python package `intelhex`! https://github.com/python-intelhex/intelhex \r\nTo install it, maybe run the command\r\n        pip install intelhex")
    input("press the ENTER key to exit")
    sys.exit()

try:
    import serial
    from serial.tools.list_ports import comports
except ImportError:
    print("ERROR: You need the Python package `pyserial`! https://pypi.org/project/pyserial/ \r\nTo install it, maybe run the command\r\n        pip install pyserial")
    input("press the ENTER key to exit")
    sys.exit()

def main():
    global args
    global no_wait

    got_file = False
    if len(sys.argv) == 2:
        if os.path.isfile(sys.argv[1]):
            got_file = True

    parser = argparse.ArgumentParser()
    if got_file == False:
        parser.add_argument("-f", "--firmware",                         default="",           type=str, help="firmware file")
    parser.add_argument("-s", "--serialport",                       default="auto",       type=str, help="serial port name")
    parser.add_argument("-p", "--preserveeeprom",                   action="store_true",            help="preserve eeprom")
    parser.add_argument("-c", "--cfgfile",                          default="",           type=str, help="specify config text file")
    parser.add_argument("-a", "--fwaddr",     metavar="fwaddr",     default="",           type=str, help="firmware start address")
    parser.add_argument("-e", "--eepromaddr", metavar="eepromaddr", default="",           type=str, help="eeprom address")
    parser.add_argument("-m", "--addrmulti",  metavar="addrmulti",  default="",           type=str, help="address multiplier")
    parser.add_argument("-r", "--fullsave",                         action="store_true",            help="reads contents from ESC and saves it, does not perform write")
    parser.add_argument("-d", "--demo",                             action="store_true",            help="demo mode, does not actually write")
    parser.add_argument("-w", "--nowait",                           action="store_true",            help="do not wait for user to press anything")
    parser.add_argument("-v", "--verbose",                          action="store_true",            help="verbose")
    args = parser.parse_args()

    no_wait = args.nowait

    if args.verbose:
        print("verbose output is ON")
        print("version: V1.5")

    if got_file:
        args.firmware = sys.argv[1]
        print("input file: \"%s\"" % args.firmware)

    if got_file or (args.firmware is not None and len(args.firmware) > 4) and args.fullsave == False:
        if os.path.isfile(args.firmware) == False:
            print("ERROR: the file \"%s\" does not exist" % args.firmware)
            quit_nicely(-1)

    ports = get_all_comports(False)
    if args.serialport is None or args.serialport == "auto" or args.serialport == "":
        got_port = False
        while got_port == False:
            if len(ports) == 1:
                print("auto detected serial port: %s" % ports[0])
                x = ask_user_confirm("confirm start using \"%s\"?" % ports[0], auto = False)
                if x:
                    got_port = True
                    args.serialport = ports[0]
                else:
                    quit_nicely(-1)
            else:
                tries = 2
                while True:
                    print("available serial ports: ")
                    if len(ports) > 0:
                        for i in ports:
                            print("\t%s" % i)
                    else:
                        print("\t(none available)")
                    x = input("please enter the name of the serial port: ")
                    ports = get_all_comports(False) # refresh the list

                    if x in ports:
                        # exact match
                        args.serialport = x
                        got_port = True
                        break
                    else:
                        # attempt a number only match
                        xp = re.findall(r'\d+', x)
                        if len(xp) == 1:
                             for p in ports:
                                 pp = re.findall(r'\d+', p)
                                 if len(pp) > 0:
                                     if int(xp[0]) == int(pp[-1]):
                                         args.serialport = p
                                         got_port = True
                                         break
                        if got_port:
                            print("user specified port \"%s\"" % args.serialport)
                            break
                    if got_port == False:
                        print("ERROR: that is not one of the available ports")
                        if no_wait:
                            quit_nicely(-1)
                        if len(x) > 0:
                            tries -= 1
                        else:
                            tries = 2
                        if tries > 0:
                            continue
                        else:
                            print("user insisted on using serial port: %s" % x)
                            args.serialport = x
                            got_port = True
                            break
                    else:
                        break
    else:
        if args.verbose:
            print("user specified using serial port: %s" % args.serialport)

    if args.fullsave:
        dated_filename = "read-" + datetime.datetime.now().strftime("%Y%m%d%H%M%S") + ".hex"

    while True:
        if args.firmware is None or len(args.firmware) <= 0:
            f = try_windows_openfiledialog()
            if f is None:
                args.firmware = input("please enter the path to the firmware *.hex file: ").strip('"')
            else:
                args.firmware = f

        if (args.firmware is None or len(args.firmware) <= 0) and args.fullsave == False:
            continue # ask again
        elif (args.firmware is None or len(args.firmware) <= 0) and args.fullsave == True:
            args.firmware = dated_filename # if blank, use a default unique timestamped name
            print("using unique name \"%s\"" % dated_filename)
        if args.fullsave == True and args.firmware.lower().endswith(".hex") == False:
            args.firmware += ".hex"

        fw_fullpath = os.path.abspath(args.firmware)
        if os.path.isfile(fw_fullpath) or args.fullsave:
            fw_basename = os.path.basename(fw_fullpath)
            fw_namesplit = os.path.splitext(fw_basename)
            fw_justname = fw_namesplit[0].strip()
            fw_ext = fw_namesplit[1].strip().lower()
            fw_dir = os.path.dirname(fw_fullpath)
            if fw_ext != ".hex":
                print("ERROR: the file must be a *.hex file type")
                args.firmware = ""
                if no_wait:
                    quit_nicely(-1)
                continue
            elif args.fullsave == False:
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
                        pass
                    if no_wait == False:
                        print("")
                        print("you should choose another file")
                        args.firmware = ""
                        continue
                    else:
                        quit_nicely(-1)

                if args.verbose:
                    print("firmware: %s (%s) (%s)" % (fw_justname, fw_ext, fw_dir))
                break
            elif args.fullsave:
                if os.path.isfile(fw_fullpath):
                    fw_fullpath = os.path.join(fw_dir, fw_justname + "." + dated_filename)
                    print("destination file already exists, saving to different unique name \"%s\"" % fw_fullpath)
                break
        else:
            print("ERROR: the file \"%s\" does not exist" % fw_fullpath)
            if no_wait:
                quit_nicely(-1)
            args.firmware = ""
            continue

    if args.fullsave == False:
        if args.verbose:
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

        if args.verbose:
            print("firmware embedded identification: 0x%08X" % fwfile_id)

    ser = serial.serial_for_url(args.serialport, do_not_open=True)
    # serial port parameters must match AM32 bootloader
    ser.baudrate = 19200
    ser.bytesize = serial.EIGHTBITS
    ser.parity   = serial.PARITY_NONE
    ser.stopbits = serial.STOPBITS_ONE
    ser.timeout  = 1
    try:
        ser.open()
        if args.verbose:
            print("serial port opened")
        time.sleep(2)
    except serial.SerialException as e:
        print('ERROR: could not open serial port {}: {}\n'.format(ser.name, e))
        if args.demo == False:
            quit_nicely(-1)

    # AM32 bootloader can be queried for its identification
    bootloader_id = bootloader_query(ser)
    hex_id = (''.join('0x{:02X}, '.format(a) for a in bootloader_id)).strip()
    if args.verbose:
        print("hardware bootloader ID array: { %s }" % hex_id)

    if len(bootloader_id) < 9:
        print("ERROR: hardware bootloader failed to reply")
        if args.demo == False:
            quit_nicely(-1)

    mcuid_f051      = [0x34, 0x37, 0x31, 0x00, 0x1F, 0x06, 0x06, 0x01, 0x30]
    mcuid_g071_64k  = [0x34, 0x37, 0x31, 0x64, 0x35, 0x06, 0x06, 0x01, 0x30]
    mcuid_g071_128k = [0x34, 0x37, 0x31, 0x64, 0x2B, 0x06, 0x06, 0x01, 0x30]
    # https://github.com/AlkaMotors/AM32_Bootloader_F051/blob/9976cd7fe3dd12d012b0e86e0b2787b3cb7ae159/Core/Src/main.c#L74
    # https://github.com/AlkaMotors/g071Bootloader/blob/f86fc79a05c7ddcc25d598f4b2c952204c98c674/Core/Src/main.c#L62
    mcuid_at32f421  = [0x34, 0x37, 0x31, 0x00, 0x1F, 0x06, 0x06, 0x01, 0x30]
    # https://github.com/AlkaMotors/AT32F421_AM32_Bootloader/blob/922493dd0e54bae1c92cecdd9fd5472ce099dd21/Src/main.c#L99
    mcuid_gd32f350  = [0x34, 0x37, 0x31, 0x64, 0x35, 0x06, 0x06, 0x02, 0x31]

    if args.fullsave == False:
        bypass_mismatch = False

        mcu_id = int((fwfile_id & 0x00FF0000) >> 16)
        should_be = []
        if mcu_id == 0x51:
            should_be.append(mcuid_f051)
            if args.verbose:
                print("firmware file metadata claims  MCU = F051")
            fwaddr     = 0x08001000
            eep_addr   = 0x7C00
            addr_multi = 1
        elif mcu_id == 0x71:
            should_be.append(mcuid_g071_64k)
            should_be.append(mcuid_g071_128k)
            if args.verbose:
                print("firmware file metadata claims MCU = G071")
            fwaddr     = 0x08001000
            eep_addr   = 0xF800
            addr_multi = 1
        elif mcu_id == 0x21:
            should_be.append(mcuid_at32f421)
            if args.verbose:
                print("firmware file metadata claims  MCU = AT32F421")
            fwaddr     = 0x08001000
            eep_addr   = 0x7C00
            addr_multi = 1
        elif mcu_id == 0x35:
            should_be.append(mcuid_gd32f350)
            if args.verbose:
                print("firmware file metadata claims MCU = GD32F350")
            fwaddr     = 0x08001000
            eep_addr   = 0xF800
            addr_multi = 1
        else:
            print("ERROR: firmware file cannot be verified, the embedded metadata cannot be parsed (0x%08X)" % fwfile_id)
            if ask_user_confirm("continue anyways?"):
                bypass_mismatch = True
            else:
                quit_nicely(-1)

        pin_num_file  = (fwfile_id & 0xFF00) >> 8
        pin_num_boot  = bootloader_id[3] & 0x0F
        port_num_file = int((fwfile_id & 0xFF) / 4)
        port_num_boot = (bootloader_id[3] & 0xF0) >> 4
        if args.verbose:
            print("firmware file metadata claims  GPIO %s  pin %u" % (chr(ord('A') + port_num_file), pin_num_file))
            print("hardware bootloader    claims  GPIO %s  pin %u" % (chr(ord('A') + port_num_boot), pin_num_boot))

        if bypass_mismatch == False and pin_num_file != pin_num_boot:
            print("ERROR: pin number does not match between ESC hardware (%u) and firmware metadata (%u)" % (pin_num_boot, pin_num_file))
            if ask_user_confirm("continue anyways?"):
                bypass_mismatch = True
            else:
                quit_nicely(-1)

        if bypass_mismatch == False and pin_num_file != pin_num_boot:
            print("ERROR: GPIO port number does not match between ESC hardware (%u) and firmware metadata (%u)" % (port_num_boot, port_num_file))
            if ask_user_confirm("continue anyways?"):
                bypass_mismatch = True
            else:
                quit_nicely(-1)

        match_res = bootloader_match(bootloader_id, should_be)
        if match_res >= 0: # good match
            if (fwfile_id & 0x00FF0000) == 0x00710000 and match_res == 1:
                # 128 KB memory requires an address multiplier
                addr_multi = 4
        else: # no match
            print("ERROR: hardware bootloader identity cannot be verified: { %s }" % hex_id)
            if ask_user_confirm("continue anyways?"):
                bypass_mismatch = True
                should_be = [mcuid_f051, mcuid_g071_64k, mcuid_g071_128k, mcuid_g071_128k, mcuid_at32f421, mcuid_gd32f350]
                match_res = bootloader_match(bootloader_id, should_be)
                fwaddr = 0x08001000
                addr_multi = 1
                if match_res == 0:
                    fw_size = 1024 * 32
                    eep_addr = 0x7C00
                    addr_multi = 1
                elif match_res == 1:
                    fw_size = 1024 * 64
                    eep_addr = 0xF800
                    addr_multi = 1
                elif match_res == 2:
                    fw_size = 1024 * 128
                    eep_addr = 0xF800
                    addr_multi = 4
                elif match_res == 3:
                    fw_size = 1024 * 32
                    eep_addr = 0x7C00
                    addr_multi = 1
                elif match_res == 4:
                    fw_size = 1024 * 64
                    eep_addr = 0xF800
                    addr_multi = 1
            else:
                quit_nicely(-1)

        check_firmware_name(ser, fw_ihex, addr_multi)

        if mcu_id == 0x51:
            # embed the correct bootloader version into the EEPROM region so that AM32 does not erase it
            blversion = get_am32_bootloader_version(ser, addr_multi)
            if blversion > 0 and blversion != 0xFF:
                blversion_addr = (fwaddr & 0xFF000000) + eep_addr + 2
                fw_ihex[blversion_addr] = blversion
                if args.verbose:
                    print("automatic embed bootloader version 0x%02X to 0x%08X" % (blversion, blversion_addr))

    else: # full save
        should_be = [mcuid_f051, mcuid_g071_64k, mcuid_g071_128k, mcuid_at32f421]
        match_res = bootloader_match(bootloader_id, should_be)
        if match_res < 0:
            print("ERROR: hardware bootloader identity cannot be verified: { %s }" % hex_id)
        if match_res == 0:
            fw_size = 1024 * 32
            addr_multi = 1
        elif match_res == 1:
            fw_size = 1024 * 64
            addr_multi = 1
        elif match_res == 2:
            fw_size = 1024 * 128
            addr_multi = 4
        elif match_res == 3:
            fw_size = 1024 * 32
            addr_multi = 1
        else:
            print("assuming large size of 64K")
            fw_size = 1024 * 64
            addr_multi = 1
        if args.verbose:
            print("FW size %u , addr-multi %u" % (fw_size, addr_multi))

    if len(args.fwaddr) > 0:
        if "0x" in args.fwaddr.lower():
            fwaddr = int(args.fwaddr, 16)
        else:
            fwaddr = int(args.fwaddr)
        print("overriding firmware start address to 0x%08X" % fwaddr)

    if len(args.eepromaddr) > 0:
        if "0x" in args.eepromaddr.lower():
            eep_addr = int(args.eepromaddr, 16)
        else:
            eep_addr = int(args.eepromaddr)
        print("overriding EEPROM address to 0x%04X" % eep_addr)

    if len(args.addrmulti) > 0:
        if "0x" in args.addrmulti.lower():
            addr_multi = int(args.addrmulti, 16)
        else:
            addr_multi = int(args.addrmulti)
        print("overriding address multiplier to %u" % addr_multi)

    chunksize = 128

    if args.fullsave == False:
        fw_binarr = fw_ihex.tobinarray(start = fwaddr)

        if args.cfgfile is None or len(args.cfgfile) <= 0: # user did not specify a text file
            if args.preserveeeprom:
                fw_binarr = fw_binarr[:eep_addr - (fwaddr & 0xFFFF)]
        else:
            cfg = UserCfg()
            cfg.install_all_items()
            fool_sz = 5 + 12
            if args.preserveeeprom:
                send_setaddress(ser, int((eep_addr & 0x00FFFFFF) / addr_multi))
                eedata = send_readcmd(ser, (eep_addr & 0x00FFFFFF), cfg.get_length() + fool_sz)
                cfg.load_bytes(eedata[fool_sz:])
                if args.verbose:
                    print("loaded %d bytes from device EEPROM" % len(eedata))
            else:
                def_idx = cfg.load_binary(fw_binarr[eep_addr - (fwaddr & 0xFFFF):]) # first look for EEPROM payload embedded
                if def_idx < 0: # no embedded config found, look again for defaults in the whole firmware
                    def_idx = cfg.load_binary(fw_binarr)
                elif args.verbose:
                    print("found config payload at 0x%08X from input file" % (def_idx + eep_addr))
                if def_idx > 0:
                    eedata = fw_binarr[def_idx:]
                else:
                    raise Exception("config tool cannot determine default values")
                if args.verbose:
                    print("firmware file config will be merged with new config \"%s\"" % (os.path.basename(args.cfgfile)))
            fool_data = eedata[:fool_sz]
            #if args.verbose:
            #    print("AM32 EE header: %s" % str(fool_data))
            if mcu_id == 0x51 and blversion > 0 and blversion != 0xFF:
                fool_data[2] = blversion
            v = eedata[fool_sz + 6]
            if args.verbose:
                print("EE versions: read %d, tool %d" % (v, cfg.get_version()))
            if v != cfg.get_version():
                print("ERROR: config file tool version \"%d\" does not match firmware EEPROM version \"%d\"" % (cfg.get_version(), v))
                if ask_user_confirm("continue anyways?") == False:
                    quit_nicely(-1)
            txt_cnt = cfg.load_text_file(args.cfgfile)
            if args.verbose:
                print("loaded %d config items from specified text file" % txt_cnt)
            fw_binarr = fw_binarr[:eep_addr - (fwaddr & 0xFFFF)]
            fw_binarr.extend(fool_data)
            fw_binarr.extend(cfg.get_bytes())

        i = 0
        start_addr = fwaddr & 0xFFFF

        if args.verbose:
            print("total size = %u ; start addr = 0x%04X ; chunk size = %u" % (len(fw_binarr), start_addr, chunksize))

        j = start_addr
        done = False
        while i < len(fw_binarr) and done == False:
            thischunk = chunksize
            if (i + thischunk) >= len(fw_binarr):
                thischunk = len(fw_binarr) - i
                while (thischunk % 4) != 0:
                    thischunk += 1
                    fw_binarr.append(0xFF)
                done = True
            barr = fw_binarr[i:i + thischunk]

            draw_progress_bar("writing to 0x%04X -" % j, i, 0, len(fw_binarr), ret = (i != 0))

            if args.demo == False:
                send_setaddress(ser, int(j / addr_multi))
                send_setbuffer(ser, j, thischunk)
                send_payload(ser, j, barr)
                send_flash(ser, j)
            i += thischunk
            j += thischunk

        blank_progress_bar()
        print("\rfinished all writes, begin verification...")

    i = 0
    done = False
    if args.fullsave == False:
        j = start_addr
        while i < len(fw_binarr) and done == False:
            thischunk = chunksize
            if (i + thischunk) >= len(fw_binarr): # past the end of the firmware blob
                thischunk = len(fw_binarr) - i # set the chunk size to only be the remainder
                done = True
            barr = fw_binarr[i:i + thischunk]

            draw_progress_bar("verifying 0x%04X -" % j, i, 0, len(fw_binarr), ret = (i != 0))

            tries = 3
            while tries > 0:
                send_setaddress(ser, int(j / addr_multi))
                data = send_readcmd(ser, j, thischunk)
                if len(barr) != len(data):
                    data_exception("verification read length at 0x%04X does not match, %u != %u" % (j, len(barr), len(data)))
                wcrc = calc_crc16(barr)
                rcrc = calc_crc16(data)
                tries -= 1
                if wcrc == rcrc or args.demo:
                    break
                if wcrc != rcrc and tries <= 0:
                    data_exception("verification read contents at 0x%04X does not match,\r\n\tdata %s\r\n\tread %s (%u %u %u)\r\n" % (j, format_arr(barr), format_arr(data), i, thischunk, len(fw_binarr)))
            i += thischunk
            j += thischunk
        blank_progress_bar()
        print("\rfinished verification")
    else:
        # save to file
        fw_barr = bytearray()
        while i < fw_size and done == False:
            thischunk = chunksize
            if (i + thischunk) >= fw_size:
                thischunk = fw_size - i
                done = True

            draw_progress_bar("reading 0x%04X -" % i, i, 0, fw_size, ret = (i != 0))

            tries = 3
            while tries > 0:
                send_setaddress(ser, int(i / addr_multi))
                data = send_readcmd(ser, i, thischunk)
                if data is not None:
                    fw_barr.extend(data)
                    break
                elif tries <= 0:
                    data_exception("too many CRC errors at 0x%04X\r\n" % (i))
            i += thischunk
        blank_progress_bar()
        print("\rfinished reading")
        fw_ihex = IntelHex()
        addr = 0x08000000
        for b in fw_barr:
            fw_ihex[addr] = b
            addr += 1
        fw_ihex.tofile(fw_fullpath, format='hex')
        print("saved new file: \"%s\"" % fw_fullpath)

    try:
        ser.close()
    except Exception as ex:
        print("ERROR: exception when closing serial port: %s" % (str(ex)))

    print("all done!!! 100% [##############################]")
    quit_nicely()

def quit_nicely(c = 0):
    global no_wait
    if no_wait == False:
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

def ask_user_confirm(p, auto = True):
    global no_wait
    if no_wait and auto:
        return False
        
    tries = 2
    while True:
        x = input(p + " (YES or no): ")
        if x == "YES": # require case sensitivity for confirmation
            return True
        elif x.lower().startswith("n"):
            return False
        else:
            print("ERROR: cannot understand response")
            tries -= 1
            if tries <= 0:
                return False

def get_all_comports(to_print):
    ports = []
    for n, (port, desc, hwid) in enumerate(sorted(comports()), 1):
        if to_print:
            print('--- {:2}: {:20} {!r}\n'.format(n, port, desc))
        ports.append(port)
    return ports

def calc_crc16(data, length = None):
    if length is None:
        length = len(data)
    crc = 0
    i = 0
    while i < length:
        xb = data[i]
        j = 0
        while j < 8:
            if ((xb & 0x01) ^ (crc & 0x0001)) != 0:
                crc = (crc >> 1    ) & 0xFFFF
                crc = (crc ^ 0xA001) & 0xFFFF
            else:
                crc = (crc >> 1    ) & 0xFFFF
            xb = (xb >> 1) & 0xFF
            j += 1
        i += 1
    return crc & 0xFFFF

def bootloader_match(bootloader_id, should_be):
    j = 0
    submatch = False
    for sb in should_be:
        if len(sb) != len(bootloader_id):
            continue
        i = 0
        submatch = True
        while i < len(sb) and i < len(bootloader_id):
            if i != 3 and sb[i] != bootloader_id[i]:
                submatch = False
                break
            i += 1
        if submatch:
            return j
        j += 1
    return -1

def append_crc(data):
    crc = calc_crc16(data)
    data.append(crc & 0xFF)
    data.append(((crc & 0xFF00) >> 8) & 0xFF)
    return data

def read_serial(ser, rlen, timeout = 2):
    tstart = datetime.datetime.now()
    data = []
    while rlen > 0:
        x = ser.read(rlen)
        if len(x) > 0:
            data.extend(x)
            rlen -= len(x)
        tnow = datetime.datetime.now()
        if (tnow - tstart).total_seconds() > timeout:
            break
        time.sleep(0)
    return data

def serial_write(ser, data, rlen = -1, chunk_sz = 512, timeout = 2):
    tstart = datetime.datetime.now()
    ret = []
    while len(data) > 0:
        if len(data) > chunk_sz and chunk_sz > 0:
            this_chunk = data[0:chunk_sz]
        else:
            this_chunk = data
        ser.write(this_chunk)
        ser.flush()
        r = read_serial(ser, len(this_chunk)) # this reads the echo from the half-duplex USB linker
        if len(r) > 0:
            ret.extend(r)
        else:
            ser.close()
            ser.open()
        if len(data) > chunk_sz and chunk_sz > 0:
            data = data[chunk_sz:]
        else:
            break
        tnow = datetime.datetime.now()
        if (tnow - tstart).total_seconds() > timeout:
            break
        time.sleep(0.01)
    if rlen >= 0 and len(ret) < rlen:
        r = read_serial(ser, rlen - len(ret)) # this reads the rest of the reply
        if len(r) > 0:
            ret.extend(r)
    return ret # the reply packet will contain an echo of what is transmitted, remember to split this array later

def bootloader_query(ser):
    try:
        x = bytearray([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0D, 0x42, 0x4C, 0x48, 0x65, 0x6C, 0x69, 0xF4, 0x7D]) # this will trigger a reply to the query, https://github.com/AlkaMotors/AM32_Bootloader_F051/blob/9976cd7fe3dd12d012b0e86e0b2787b3cb7ae159/Core/Src/main.c#L217
        y = serial_write(ser, x, len(x) + 9)
        time.sleep(0.001) # this is only here to put a gap in the logic analyzer display, so I can decipher the stream visially
        return y[len(x):]
    except Exception as ex:
        print("ERROR during bootloader query, exception: %s" % (ex))
        quit_nicely(-1)

def get_am32_bootloader_version(ser, addr_multi = 1):
    send_setaddress(ser, int(0x0FFC / addr_multi))
    data = send_readcmd(ser, 0x0FFC, 32)
    return data[0]

def check_firmware_name(ser, ihex, addr_multi = 1):
    global args
    addr = 0x08001100
    send_setaddress(ser, int((addr & 0x00FFFFFF) / addr_multi))
    data = send_readcmd(ser, (addr & 0x00FFFFFF), 40)
    fwname_read = ""
    i = 7
    while i < 40:
        d = data[i]
        if d != 0:
            fwname_read += chr(d)
        else:
            break
        i += 1
    if fwname_read.startswith("Hydra "):
        i = 0
        mismatch_i = -1
        match = True
        flname = ""

        while i < 40:
            if i != 4 and i != 5 and i != 6: # version numbers don't matter
                d  = int(data[i])
                d2 = int(ihex[addr + i])
                if d == d2 and d == 0 and i > 7:
                    break
                elif d != d2:
                    mismatch_i = i
                    match = False
                    break
                elif d == d2:
                    flname += chr(d2)
            i += 1
        if match:
            if args.verbose:
                print("hardware previous firmware name \"%s\" matches firmware file" % fwname_read)
            return True
        else:
            if mismatch_i < 7:
                print("WARNING: previous Hydra HW ID 0x%02X%02X%02X%02X does not match new firmware file HW ID 0x%02X%02X%02X%02X" % (data[3], data[2], data[1], data[0], ihex[addr + 3], ihex[addr + 2], ihex[addr + 1], ihex[addr + 0]))
            else:
                print("WARNING: previously installed firmware name is \"%s\", does not match the new firmware name \"%s\"" % (fwname_read, flname))
            x = ask_user_confirm("continue the installation?", auto = False)
            if x == False:
                quit_nicely(-1)
            return True
    else:
        if args.verbose:
            print("hardware previous firmware name is not Hydra")
        return False

def send_setaddress(ser, addr):
    try:
        x = bytearray([0xFF, 0x00, 0x00, 0x00])
        x[2] = (addr & 0xFF00) >> 8;
        x[3] = (addr & 0x00FF) >> 0;
        x = append_crc(x)
        y = serial_write(ser, x, len(x) + 1)
        if len(y) < len(x) + 1:
            raise Exception("did not read enough data, len %u < %u" % (len(y), len(x) + 1))
        if y[-1] != 0x30:
            raise Exception("did not get valid ack, 0x%02X" % y[-1])
        time.sleep(0.001) # this is only here to put a gap in the logic analyzer display, so I can decipher the stream visially
    except Exception as ex:
        print("\n\rERROR during set address command (@ 0x%04X), exception: %s" % (addr, ex))
        quit_nicely(-1)

def send_setbuffer(ser, addr, buflen):
    try:
        x = bytearray([0xFE, 0x00, 0, buflen])
        x = append_crc(x)
        y = serial_write(ser, x, len(x))
        if len(y) < len(x):
            raise Exception("did not read enough data, len %u < %u" % (len(y), len(x)))
        time.sleep(0.0035) # requires a long timeout, the bootloader code is reliant on a timeout before running the parser, but we have no ACK to indicate if this has occured, so we must hard-code a delay
    except Exception as ex:
        print("\n\rERROR during set buffer command (@ 0x%04X %u), exception: %s" % (addr, buflen, ex))
        quit_nicely(-1)

def send_payload(ser, addr, x):
    try:
        x = append_crc(x)
        y = serial_write(ser, x, len(x) + 1)
        if len(y) < len(x) + 1:
            raise Exception("did not read enough data, len %u < %u" % (len(y), len(x) + 1))
        if y[-1] != 0x30:
            raise Exception("did not get valid ack, 0x%02X" % y[-1])
    except Exception as ex:
        print("\n\rERROR during payload transfer (@ 0x%04X), exception: %s" % (addr, ex))
        quit_nicely(-1)

def send_flash(ser, addr):
    try:
        x = bytearray([0x01, 0x01])
        x = append_crc(x)
        y = serial_write(ser, x, len(x) + 1)
        if len(y) < len(x) + 1:
            raise Exception("did not read enough data, len %u < %u" % (len(y), len(x) + 1))
        if y[-1] != 0x30:
            raise Exception("did not get valid ack, 0x%02X" % y[-1])
    except Exception as ex:
        print("\n\rERROR during flash command (@ 0x%04X), exception: %s" % (addr, ex))
        quit_nicely(-1)

def send_readcmd(ser, addr, buflen):
    try:
        x = bytearray([0x03, buflen])
        x = append_crc(x)
        expected = len(x) + buflen + 3
        y = serial_write(ser, x, expected)
        if y[-1] != 0x30:
            raise Exception("did not get valid ack, 0x%02X" % y[-1])
        y = y[len(x):]
        data = y[:-3]
        if len(data) != buflen:
            raise Exception("length of read data does not match, %u != %u" % (len(data), buflen))
        #calcedcrc = calc_crc16(data)
        #rxedcrc = (y[-2] << 8) | y[-3]
        #if rxedcrc != calcedcrc:
        #    print("\nWARNING: CRC mismatch @ 0x%04X, rx 0x%04X != calc 0x%04X" % (addr, rxedcrc, calcedcrc))
        return data
    except Exception as ex:
        print("\n\rERROR during read command (@ 0x%04X %u), exception: %s" % (addr, buflen, ex))
        quit_nicely(-1)

def format_arr(data):
    s = "["
    for i in data:
        s += "%02X " % i
    s = s.strip()
    s += "]"
    return s

def draw_progress_bar(headtxt, num, lower, upper, endtxt = "", ret = True, width = 36):
    global progbar_size
    percent = int(round(num * 100 / (upper - lower)))
    percent = 99 if percent >= 99 else percent # will disappear at 100 anyways
    s = headtxt + " %2u%% [" % (percent)
    pt = num * width / (upper - lower)
    i = 0
    while i < width:
        if i <= pt:
            s += "#"
        else:
            s += "."
        i += 1
    s += "]"
    if len(endtxt) > 0:
        s += " " + endtxt.strip()
    s = s.strip()
    if ret:
        s = '\r' + s
    print(s, end="")
    progbar_size = len(s) # remember the bar size so it can be cleared later

def blank_progress_bar():
    global progbar_size
    print("\r", end="")
    i = 0
    while i <= progbar_size:
        print(" ", end="")
        i += 1

def data_exception(s):
    print("\n\r" + s)
    quit_nicely(-1)

def try_windows_openfiledialog():
    try:
        import easygui
        x = easygui.fileopenbox(title='Select Firmware File', filetypes=["*.hex"])
        if x is None:
            # user explicitly cancelled, we can quit
            sys.exit(-1)
        return x
    except Exception as ex:
        pass
    return None

class UserCfgItem(object):
    def __init__(self, name, struct_name, byte_idx, ctype, byte_size, val = 0):
        self.name = name
        self.struct_name = struct_name
        self.value = val
        self.ctype = ctype
        self.byte_index = byte_idx # this index is 0 for the start of the magic word
        self.byte_size = byte_size
        signed = True if self.ctype.startswith("int") and self.ctype.endswith("_t") else False
        if "signed" in self.ctype:
            signed = True
        self.signed = signed

    def get_bytes(self):
        x = int(self.value).to_bytes(self.byte_size, byteorder='little', signed=self.signed)
        if len(x) != self.byte_size:
            x = x[:self.byte_size]
        return x

    def set_bytes(self, data):
        self.value = int.from_bytes(data, byteorder="little", signed=self.signed)

class UserCfg(object):
    def __init__(self):
        self.init_idx = 0
        self.items = []
        self.version = 3 # change this according to makefile

    def install_item(self, name, struct_name, c_type, val = 0):
        byte_size = c_type_to_bytes(c_type)
        itm = UserCfgItem(name, struct_name, self.init_idx, c_type, byte_size, val = val)
        self.items.append(itm)
        self.init_idx += byte_size

    def install_all_items(self):
        # the result of this list must match the structure defined in "types.h"
        # the __main__ of this python script can generate this section
        self.install_item(None, "magic", "uint32_t", 0xDEADBEEF)
        self.install_item(None, "version_major",  "uint8_t", 0) # change this according to makefile
        self.install_item(None, "version_minor",  "uint8_t", 5) # change this according to makefile
        self.install_item(None, "version_eeprom", "uint8_t", self.version) # change this according to makefile
        self.install_item("vsplitmode", "voltage_split_mode", "uint8_t")
        self.install_item("loadbal", "load_balance", "bool")
        self.install_item("inputmode", "input_mode", "uint8_t")
        self.install_item("tankmix", "tank_arcade_mix", "bool")
        self.install_item("phasemap", "phase_map", "uint8_t")
        self.install_item("baud", "baud", "uint32_t")
        self.install_item("voltdiv", "voltage_divider", "uint32_t")
        self.install_item("curroffset", "current_offset", "uint32_t")
        self.install_item("currscale", "current_scale", "uint32_t")
        self.install_item("adcfilter", "adc_filter", "uint16_t")
        self.install_item("channel_1", "channel_1", "uint8_t")
        self.install_item("channel_2", "channel_2", "uint8_t")
        self.install_item("channel_mode", "channel_mode", "uint8_t")
        self.install_item("channel_brake", "channel_brake", "uint8_t")
        self.install_item("master_arm", "channel_masterarm", "uint8_t")
        self.install_item("rc_mid", "rc_mid", "uint16_t")
        self.install_item("rc_range", "rc_range", "uint16_t")
        self.install_item("rc_deadzone", "rc_deadzone", "uint16_t")
        self.install_item("pwm_period", "pwm_period", "uint32_t")
        self.install_item("pwm_deadtime", "pwm_deadtime", "uint32_t")
        self.install_item("braking", "braking", "bool")
        self.install_item("chanswap", "chan_swap", "bool")
        self.install_item("flip1", "flip_1", "bool")
        self.install_item("flip2", "flip_2", "bool")
        self.install_item("tied", "tied", "bool")
        self.install_item("armdur", "arm_duration", "uint32_t")
        self.install_item("disarmtime", "disarm_timeout", "uint32_t")
        self.install_item("templim", "temperature_limit", "uint8_t")
        self.install_item("currlim", "current_limit", "uint32_t")
        self.install_item("voltlim", "voltage_limit", "uint32_t")
        self.install_item("cellmaxvolt", "cell_max_volt", "uint16_t")
        self.install_item("lowbattstretch", "lowbatt_stretch", "uint16_t")
        self.install_item("curlimkp", "currlim_kp", "int32_t")
        self.install_item("curlimki", "currlim_ki", "int32_t")
        self.install_item("curlimkd", "currlim_kd", "int32_t")
        self.install_item("dirpwm_1", "dirpwm_chancfg_A", "uint8_t")
        self.install_item("dirpwm_2", "dirpwm_chancfg_B", "uint8_t")
        self.install_item("dirpwm_3", "dirpwm_chancfg_C", "uint8_t")
        self.install_item("tonevol", "tone_volume", "uint8_t")
        self.install_item(None, "useless", "uint32_t")

    def give_name(self, struct_name, name):
        ret = False
        i = 0
        while i < len(self.items):
            if self.items[i].struct_name == struct_name:
                self.items[i].name = name
                ret = True
            i += 1
        return ret

    def give_value(self, name, value):
        ret = False
        i = 0
        while i < len(self.items):
            if self.items[i].name == name or self.items[i].struct_name == name:
                self.items[i].value = int(value)
                ret = True
            i += 1
        return ret

    def get_version(self):
        return self.version

    def load_struct_from_c(self, fpath = "../src-app/types.h"):
        encountered = False
        finished = False
        with open(fpath) as f:
            line = f.readline()
            while line:
                if "//" in line:
                    line = line[:line.index("//")]
                line = line.strip()
                if line.endswith(";") == False:
                    line = f.readline()
                    continue
                line = line.strip()
                line = line[:line.index(";")].rstrip()
                if ' ' not in line and '\t' not in line:
                    line = f.readline()
                    continue
                wsidx = max(line.rfind(' '), line.rfind('\t'))
                name = line[wsidx:].strip()
                ctype = line[:wsidx].strip()
                if name == "magic" and c_type_to_bytes(ctype) == 4:
                    encountered = True
                if name == "chksum" or name == "checksum":
                    finished = True
                if encountered and finished == False:
                    self.install_item(None, name, ctype)
                line = f.readline()

    def load_names_from_c(self, fpath = "../src-app/userconfig.c"):
        with open(fpath) as f:
            line = f.readline()
            while line:
                if "//" in line:
                    line = line[:line.index("//")]
                line = line.strip()
                if line.startswith("DCLR_ITM"):
                    print(line)
                    try:
                        start_quote = line.find('"')
                        end_quote = line.rfind('"')
                        name = line[start_quote + 1:end_quote]
                        line = line[end_quote+1:]
                        match = re.search(r'\W([^\s\W]+)\W', line)
                        if match:
                            struct_name = match.group(1).strip()
                            self.give_name(struct_name, name)
                    except Exception as ex:
                        print(ex)
                        pass
                line = f.readline()

    def load_text_file(self, file_path):
        cnt = 0
        with open(file_path) as f:
            lines = f.readlines()
            lnum = 0
            for line in lines:
                ori_line = line
                lnum += 1
                try:
                    if "//" in line:
                        line = line[:line.index("//")]
                    if "#" in line:
                        line = line[:line.index("#")]
                    line = line.strip()
                    line_parts = line.split()
                    if len(line_parts) >= 2:
                        if self.give_value(line_parts[0], line_parts[1]):
                            cnt += 1
                        else:
                            print("WARNING: config item name \"%s\" is not understood" % line_parts[0])
                except Exception as ex:
                    print("ERROR on config file line [%d] \"%s\": %s" % (lnum, ori_line, str(ex)))
        return cnt

    def load_bytes(self, data):
        i = 0
        while i < len(data):
            did = False
            j = 0
            while j < len(self.items):
                if i == self.items[j].byte_index:
                    ii = i
                    k = 0
                    seg = []
                    while k < self.items[j].byte_size:
                        seg.append(data[i])
                        k += 1
                        i += 1
                        did = True
                    self.items[j].set_bytes(seg)
                    break
                j += 1
            if did == False:
                i += 1

    def load_binary(self, binarr):
        i = 0
        clen = 16
        while i < len(binarr) - clen:
            chunk = binarr[i:]
            chunk = chunk[:clen]
            if chunk[0] == ord('H') and chunk[1] == ord('Y') and chunk[2] == ord('D') and chunk[3] == ord('R') and chunk[4] == ord('A') and chunk[5] == 0:
                if chunk[-1] == 0xDE and chunk[-2] == 0xAD and chunk[-3] == 0xBE and chunk[-4] == 0xEF:
                    j = i + 12
                    self.load_bytes(binarr[j:])
                    return i - 5
            i += 1
        return -1

    def load_firmware(self, hexpath = "../bin/HYDRA_EMAXFORMULA_F051_V0m4.hex", fwaddr = 0x08001000):
        fw_ihex = IntelHex(hexpath)
        return self.load_binary(fw_ihex.tobinarray(start = fwaddr))

    def get_bytes(self, header = None):
        all_data = []
        for i in self.items:
            all_data.extend(i.get_bytes())
        if header is not None:
            i = 0
            while i < len(header):
                all_data[i] = header[i]
                i += 1
        crc = calc_crc8(all_data)
        all_data.append(crc)
        return all_data

    def get_length(self):
        return self.init_idx + 1

    def write_initializer(self):
        lines = []
        for i in self.items:
            lines.append("self.install_item(%s, \"%s\", \"%s\") # %d ; %d ; %d" % ("None" if i.name is None else ("\"" + i.name + "\""), i.struct_name, i.ctype, i.byte_index, i.byte_size, len(i.get_bytes())))
        return lines

    def write_text(self):
        lines = []
        for i in self.items:
            if i.name is not None and len(i.name) > 0:
                lines.append("%s %d" % (i.name, i.value))
        barr = self.get_bytes()
        i = 0
        s = ""
        while i < len(barr):
            s += "%02X " % barr[i]
            i += 1
            if (i % 16) == 0:
                lines.append("# " + s)
                s = ""
        if len(s) > 0:
            lines.append("# " + s)
        return lines

def c_type_to_bytes(c_type):
    type_to_bytes = {
        # order of this list matters!
        "long long"  : 8,
        "long double": 16,
        "long"       : 4,
        "char"       : 1,
        "short"      : 2,
        "float"      : 4,
        "double"     : 8,
        "int8_t"     : 1,
        "int16_t"    : 2,
        "int32_t"    : 4,
        "int64_t"    : 8,
        "bool"       : 1,
        "int"        : 4,
    }
    for k, v in type_to_bytes.items():
        if k in c_type:
            return v
    if "chksum" or "checksum" in c_type:
        return 1
    raise Exception("unknown C data-type %s" % c_type)

def calc_crc8(data):
    # this function must match the same CRC function used in the firmware
    crc8tab = [
        0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54, 0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
        0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06, 0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
        0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0, 0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
        0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2, 0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
        0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9, 0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
        0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B, 0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
        0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D, 0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
        0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F, 0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
        0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB, 0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
        0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9, 0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
        0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F, 0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
        0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D, 0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
        0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26, 0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
        0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74, 0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
        0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82, 0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
        0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0, 0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9,]

    crc = 0
    i = 0
    while i < len(data):
        b = data[i]
        crc = crc8tab[crc ^ b]
        i += 1
    return crc

if __name__ == '__main__':
    try:
        main()
    except Exception as ex:
        print("\r\nERROR: unexpected fatal exception occured")
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
