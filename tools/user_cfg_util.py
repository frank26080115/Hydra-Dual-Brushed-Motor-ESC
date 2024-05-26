import sys, os
import re
from intelhex import IntelHex

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
        self.version = 4 # change this according to makefile

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
        self.install_item(None, "version_minor",  "uint8_t", 4) # change this according to makefile
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
        self.install_item("telem_port", "telemetry_port", "uint8_t")
        self.install_item("telem_baud", "telemetry_baud", "uint32_t")
        self.install_item("telem_rate", "telemetry_rate", "uint8_t")
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
        crc = calc_crc(all_data)
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

def calc_crc(all_bytes):
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
    while i < len(all_bytes):
        b = all_bytes[i]
        crc = crc8tab[crc ^ b]
        i += 1
    return crc

if __name__ == '__main__':
    print("user cfg util")
    fwaddr = 0x08001000
    cfg = UserCfg()
    i = 1
    while i < len(sys.argv):
        argv = sys.argv[i]
        if argv == "c":
            cfg.load_struct_from_c()
            cfg.load_names_from_c()
        elif argv == "install":
            cfg.install_all_items()
        elif argv == "initializer":
            initializers = cfg.write_initializer()
            for line in initializers:
                print(line)
            print(len(cfg.get_bytes()))
        elif argv.startswith("fwaddr:"):
            fwaddr = int(argv[7:], 16)
        elif argv.startswith("read:"):
            fp = argv[5:]
            fe = os.path.splitext(fp)[1].lower()
            if fe == ".hex":
                cfg.load_firmware(fp, fwaddr)
            else:
                cfg.load_text_file(fp)
        elif argv.startswith("write:"):
            fp = argv[6:]
            with open(fp, "w") as f:
                lines = cfg.write_text()
                f.writelines(lines)
        elif argv == "write":
            lines = cfg.write_text()
            for line in lines:
                print(line)
        i += 1
