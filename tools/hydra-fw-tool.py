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
    parser.add_argument("-a", "--fwaddr",     metavar="fwaddr",     default="",           type=str, help="firmware start address")
    parser.add_argument("-e", "--eepromaddr", metavar="eepromaddr", default="",           type=str, help="eeprom address")
    parser.add_argument("-m", "--addrmulti",  metavar="addrmulti",  default="",           type=str, help="address multiplier")
    parser.add_argument("-r", "--fullsave",                         action="store_true",            help="reads contents from ESC and saves it, does not perform write")
    parser.add_argument("-d", "--demo",                             action="store_true",            help="demo mode, does not actually write")
    parser.add_argument("-w", "--nowait",                           action="store_true",            help="do not wait for user to press anything")
    parser.add_argument("-v", "--verbose",                          action="store_true",            help="verbose")
    args = parser.parse_args()

    if args.verbose:
        print("verbose output is ON")
        print("version: V1.0")

    if got_file:
        args.firmware = sys.argv[1]
        print("input file: \"%s\"" % args.firmware)

    if got_file or (args.firmware is not None and len(args.firmware) > 4):
        if os.path.isfile(args.firmware) == False:
            print("ERROR: the file \"%s\" does not exist" % args.firmware)
            quit_nicely(-1)

    no_wait = args.nowait

    ports = get_all_comports(False)
    if args.serialport is None or args.serialport == "auto" or args.serialport == "":
        got_port = False
        while got_port == False:
            if len(ports) == 1:
                print("auto detected serial port: %s" % ports[0])
                x = input("confirm start using \"%s\"? (YES or no): " % ports[0])
                if x.strip() == "YES":
                    got_port = True
                    args.serialport = ports[0]
                elif x.lower().strip()[0] == 'n':
                    quit_nicely(-1)
                elif x.lower().strip()[0] == 'y':
                    print("ERROR: cannot understand input \"%s\", if you meant YES, please type it in all capital letters" % s)
                else:
                    print("ERROR: cannot understand input \"%s\"" % s)
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

    if args.fullsave == False:
        bypass_mismatch = False

        should_be = []
        if (fwfile_id & 0x00FF0000) == 0x00510000:
            should_be.append(mcuid_f051)
            if args.verbose:
                print("firmware file metadata claims  MCU = F051")
            fwaddr     = 0x08001000
            eep_addr   = 0x7C00
            addr_multi = 1
        elif (fwfile_id & 0x00FF0000) == 0x00710000:
            should_be.append(mcuid_g071_64k)
            should_be.append(mcuid_g071_128k)
            if args.verbose:
                print("firmware file metadata claims MCU = G071")
            fwaddr     = 0x08001000
            eep_addr   = 0xF800
            addr_multi = 1
        elif (fwfile_id & 0x00FF0000) == 0x00210000:
            should_be.append(mcuid_at32f421)
            if args.verbose:
                print("firmware file metadata claims  MCU = AT32F421")
            fwaddr     = 0x08001000
            eep_addr   = 0x7C00
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
                should_be = [mcuid_f051, mcuid_g071_64k, mcuid_g071_128k, mcuid_g071_128k, mcuid_at32f421]
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
            else:
                quit_nicely(-1)

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
        if args.preserveeeprom:
            fw_binarr = fw_binarr[:eep_addr - (fwaddr & 0xFFFF)]
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
                wcrc = crc16(barr)
                rcrc = crc16(data)
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

def ask_user_confirm(p):
    global no_wait
    if no_wait:
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

def crc16(data, length = None):
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
    crc = crc16(data)
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
        #calcedcrc = crc16(data)
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
    except Exception as ex:
        pass
    return None

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
