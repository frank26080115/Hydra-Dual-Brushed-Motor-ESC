#!/usr/bin/env python

import argparse
import os, time

try:
    from intelhex import IntelHex
except ImportError:
    sys.exit("ERROR: You need the Python package `intelhex`! https://github.com/python-intelhex/intelhex \r\nTo install it, maybe run the command\r\n        pip install intelhex")

try:
    import serial
    from serial.tools.list_ports import comports
except ImportError:
    sys.exit("ERROR: You need the Python package `pyserial`! https://pypi.org/project/pyserial/ \r\nTo install it, maybe run the command\r\n        pip install pyserial")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("outfile",                                                        type=str, help="output file")
    parser.add_argument("serialport",                               default="auto",       type=str, help="serial port name")
    parser.add_argument("-s", "--size",       metavar="size",       default="0x8000",     type=str, help="size of dump")
    parser.add_argument("-c", "--chunksize",  metavar="chunksize",  default="128",        type=str, help="chunk size")
    parser.add_argument("-m", "--mcu",        metavar="mcu",        default="",           type=str, help="MCU (overrides addresses)")
    parser.add_argument("-v", "--verbose",                          action="store_true",            help="verbose")
    args = parser.parse_args()

    port_name = args.serialport
    if port_name == "auto" or args.verbose:
        ports = get_all_comports(args.verbose)
        if port_name == "auto":
            if len(ports) > 0:
                port_name = ports[0]
            else:
                print("no serial ports available")
                quit()

    fw_fullpath = os.path.abspath(args.outfile)
    if fw_fullpath.lower().endswith(".bin") == False:
        fw_fullpath += ".bin"
        while "..bin" in fw_fullpath:
            fw_fullpath = fw_fullpath.replace("..bin", ".bin")
    fw_basename = os.path.basename(fw_fullpath)
    fw_namesplit = os.path.splitext(fw_basename)
    fw_justname = fw_namesplit[0].strip()
    fw_ext = fw_namesplit[1].strip().lower()
    if args.verbose:
        print("output file:")
        print("\t" + fw_fullpath)
        print("\t%s (%s)" % (fw_basename, fw_ext))

    mcu = None
    addr_multi = 1
    if args.mcu is not None and len(args.mcu) > 0:
        mcu = args.mcu.strip().lower()
        if mcu == "f051":
            fwaddr   = 0x08001000
            eep_addr = 0x7C00
            addr_multi = 1
            fw_size = 0x8000
        elif mcu == "g071-64k":
            fwaddr   = 0x08001000
            eep_addr = 0xF800
            addr_multi = 1
            fw_size = 0x8000 * 2
        elif mcu == "g071-128k":
            fwaddr   = 0x08001000
            eep_addr = 0xF800
            addr_multi = 4
            fw_size = 0x8000 * 4
        else:
            raise Exception("Unknown (or unsupported) MCU specified")
        if args.verbose:
            print("MCU \"%s\": FW-addr 0x%08X ; EEP-addr 0x%04X ; addr-multi = %u" % (fwaddr, eep_addr, addr_multi))

    if "0x" in args.chunksize.lower():
        chunksize = int(args.chunksize, 16)
    else:
        try:
            chunksize = int(args.chunksize, 10)
        except:
            chunksize = int(args.chunksize, 16)

    if (chunksize % 16) != 0:
        raise Exception("chunk size invalid, must be a multiple of 16")
    if chunksize > 256 or chunksize < 16:
        raise Exception("chunk size invalid, out of range (16 to 256)")
    if (1024 % chunksize) != 0:
        raise Exception("chunk size invalid, must reach page boundaries")

    if args.verbose:
        print("chunk size = %u" % (chunksize))

    if mcu is None:
        if "0x" in args.size.lower():
            fw_size = int(args.size, 16)
        else:
            try:
                fw_size = int(args.size, 10)
            except:
                fw_size = int(args.size, 16)

    ser = serial.serial_for_url(port_name, do_not_open=True)
    ser.baudrate = 19200
    ser.bytesize = serial.EIGHTBITS
    ser.parity   = serial.PARITY_NONE
    ser.stopbits = serial.STOPBITS_ONE
    ser.timeout  = 1

    try:
        ser.open()
        if args.verbose:
            print("serial port opened")
        time.sleep(1)
    except serial.SerialException as e:
        print('Could not open serial port {}: {}\n'.format(ser.name, e))
        quit()

    fw_barr = bytearray()
    i = 0
    done = False
    while i < fw_size and done == False:
        thischunk = chunksize
        if (i + thischunk) >= fw_size:
            thischunk = fw_size - i
            done = True

        if i != 0:
            print("\r", end="")
        print("reading 0x%04X    " % i, end="")

        tries = 3
        while tries > 0:
            send_setaddress(ser, int(i / addr_multi))
            data = send_readcmd(ser, i, thischunk)
            if data is not None:
                fw_barr.extend(data)
                break
            elif tries <= 0:
                raise Exception("too many CRC errors at 0x%04X\r\n" % (i))
        i += thischunk
    with open(fw_fullpath, "wb") as f:
        f.write(fw_barr)
    print("\rfinished dumping to file \"%s\", all done" % fw_fullpath)

def get_all_comports(to_print):
    if to_print:
        print("COM ports:")
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

def append_crc(data):
    crc = crc16(data)
    data.append(crc & 0xFF)
    data.append(((crc & 0xFF00) >> 8) & 0xFF)
    return data

def send_setaddress(ser, addr):
    try:
        x = bytearray([0xFF, 0x00, 0x00, 0x00])
        x[2] = (addr & 0xFF00) >> 8;
        x[3] = (addr & 0x00FF) >> 0;
        x = append_crc(x)
        ser.write(x)
        y = ser.read(len(x) + 1)
        if len(y) < len(x) + 1:
            raise Exception("did not read enough data, len %u" % len(y))
        if y[-1] != 0x30:
            raise Exception("did not get valid ack, 0x%02X" % y[-1])
    except Exception as ex:
        print("ERROR during set address command (@ 0x%04X), exception: %s" % (addr, ex))
        quit()

def send_setbuffer(ser, addr, buflen):
    try:
        x = bytearray([0xFE, 0x00, 0, buflen])
        x = append_crc(x)
        ser.write(x)
        y = ser.read(len(x))
        if len(y) < len(x):
            raise Exception("did not read enough data, len %u" % len(y))
        time.sleep(0.001)
    except Exception as ex:
        print("ERROR during set buffer command (@ 0x%04X %u), exception: %s" % (addr, buflen, ex))
        quit()

def send_payload(ser, addr, x):
    try:
        x = append_crc(x)
        ser.write(x)
        y = ser.read(len(x) + 1)
        if len(y) < len(x) + 1:
            raise Exception("did not read enough data, len %u" % len(y))
        if y[-1] != 0x30:
            raise Exception("did not get valid ack, 0x%02X" % y[-1])
    except Exception as ex:
        print("ERROR during payload transfer (@ 0x%04X), exception: %s" % (addr, ex))
        quit()

def send_flash(ser, addr):
    try:
        x = bytearray([0x01, 0x01])
        x = append_crc(x)
        ser.write(x)
        y = ser.read(len(x) + 1)
        if len(y) < len(x) + 1:
            raise Exception("did not read enough data, len %u" % len(y))
        if y[-1] != 0x30:
            raise Exception("did not get valid ack, 0x%02X" % y[-1])
    except Exception as ex:
        print("ERROR during flash command (@ 0x%04X), exception: %s" % (addr, ex))
        quit()

def send_readcmd(ser, addr, buflen):
    try:
        x = bytearray([0x03, buflen])
        x = append_crc(x)
        ser.write(x)
        time.sleep(0.01)
        y = bytearray()
        expected = len(x) + buflen + 3
        remaining = expected
        while len(y) < expected and remaining > 0:
            z = ser.read(remaining)
            remaining -= len(z)
            y.extend(z)
        if y[-1] != 0x30:
            raise Exception("did not get valid ack, 0x%02X" % y[-1])
        y = y[len(x):]
        data = y[:-3]
        if len(data) != buflen:
            raise Exception("length of read data does not match, %u != %u" % (len(data), buflen))
        calcedcrc = crc16(data)
        rxedcrc = (y[-2] << 8) | y[-3]
        if rxedcrc != calcedcrc:
            print("\nWARNING: CRC mismatch @ 0x%04X, rx 0x%04X != calc 0x%04X" % (addr, rxedcrc, calcedcrc))
            return None
        return data
    except Exception as ex:
        print("ERROR during read command (@ 0x%04X %u), exception: %s" % (addr, buflen, ex))
        quit()

def format_arr(data):
    s = "["
    for i in data:
        s += "%02X " % i
    s = s.strip()
    s += "]"
    return s

if __name__ == '__main__':
    main()
