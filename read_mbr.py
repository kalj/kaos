#!/usr/bin/env python3

import struct
import argparse


class MbrReader:
    def __init__(self, fname):
        with open(fname, "rb") as f:
            self.bs = f.read()
        self.jump_bytes = self.bs[0:3]
        assert self.jump_bytes[0] == 0xEB
        assert self.jump_bytes[2] == 0x90
        self.jump_offset = self.bs[1] + 2

    def handle_dos20bpb(self):
        self.OEM_name = self.bs[3:0xB].decode()
        self.bytes_per_sector = struct.unpack_from("H", self.bs, 0xB)[0]
        self.sectors_per_cluster = struct.unpack_from("B", self.bs, 0xD)[0]
        self.reserved_sectors = struct.unpack_from("H", self.bs, 0xE)[0]
        self.num_FATs = struct.unpack_from("B", self.bs, 0x10)[0]
        self.num_rootdir_entries = struct.unpack_from("H", self.bs, 0x11)[0]
        self.tot_sectors = struct.unpack_from("H", self.bs, 0x13)[0]
        self.media_descriptor = struct.unpack_from("B", self.bs, 0x15)[0]
        self.sectors_per_FAT = struct.unpack_from("H", self.bs, 0x16)[0]

        print("jump offset:           ", "   ", f"0x{self.jump_offset:X}")
        print("OEM name:              ", "(8)", self.OEM_name)
        print("bytes per sector:      ", "(2)", self.bytes_per_sector)
        print("sectors per cluster:   ", "(1)", self.sectors_per_cluster)
        print("reserved sectors:      ", "(2)", self.reserved_sectors)
        print("num FATs:              ", "(1)", self.num_FATs)
        print("num rootdir entries:   ", "(2)", self.num_rootdir_entries)
        print("tot log. sectors:      ", "(2)", self.tot_sectors)
        print("media descriptor:      ", "(1)", f"0x{self.media_descriptor:02X}")
        print("log. sectors per FAT:  ", "(2)", self.sectors_per_FAT)

    def handle_dos30bpb(self):
        self.handle_dos20bpb()
        print("phy. sectors per track:", struct.unpack_from("H", self.bs, 0x18)[0])
        print("number of heads:       ", struct.unpack_from("H", self.bs, 0x1A)[0])
        print("hidden sectors:        ", struct.unpack_from("H", self.bs, 0x1C)[0])

    def handle_dos32bpb(self):
        self.handle_dos30bpb()
        print("total sectors:         ", struct.unpack_from("H", self.bs, 0x1E)[0])

    def handle_dos331bpb(self):
        self.handle_dos20bpb()
        print(
            "phy. sectors per track:", "(2)", struct.unpack_from("H", self.bs, 0x18)[0]
        )
        print(
            "number of heads:       ", "(2)", struct.unpack_from("H", self.bs, 0x1A)[0]
        )
        print(
            "hidden sectors:        ", "(4)", struct.unpack_from("I", self.bs, 0x1C)[0]
        )
        print(
            "large tot log sectors: ", "(4)", struct.unpack_from("I", self.bs, 0x20)[0]
        )

    def handle_dos34bpb(self):
        self.handle_dos331bpb()
        print("physical drive number: ", struct.unpack_from("B", self.bs, 0x24)[0])
        flags = struct.unpack_from("B", self.bs, 0x25)[0]
        print("flags etc:             ", "0x{:X}".format(flags))
        boot_signature = struct.unpack_from("B", self.bs, 0x26)[0]
        print("extended boot sign.:   ", "0x{:02X}".format(boot_signature))
        serial_num = struct.unpack_from("I", self.bs, 0x27)[0]
        print("volume serial num:     ", f"{serial_num} (0x{serial_num:x})")

    def handle_dos40ebpb(self):
        self.handle_dos331bpb()
        print(
            "physical drive number: ", "(1)", struct.unpack_from("B", self.bs, 0x24)[0]
        )
        flags = struct.unpack_from("B", self.bs, 0x25)[0]
        print("flags etc:             ", "(1)", "0x{:0X}".format(flags))
        boot_signature = struct.unpack_from("B", self.bs, 0x26)[0]
        print("extended boot sign.:   ", "(1)", "0x{:02X}".format(boot_signature))
        serial_num = struct.unpack_from("I", self.bs, 0x27)[0]
        print("volume serial num:     ", "(4)", f"{serial_num} (0x{serial_num:x})")
        print("volume label:          ", "(11)", self.bs[0x2B : (0x2B + 11)])
        print("file system type:      ", "(8)", self.bs[0x36 : (0x36 + 8)])

    def handle_dos71ebpb28(self):
        self.handle_dos331bpb()
        print("log. sectors per FAT:  ", struct.unpack_from("I", self.bs, 0x24)[0])
        print("mirroring flags etc:   ", struct.unpack_from("H", self.bs, 0x28)[0])
        print("version:               ", struct.unpack_from("H", self.bs, 0x2A)[0])
        print("root dir. cluster:     ", struct.unpack_from("I", self.bs, 0x2C)[0])
        print("loc. of FS info sector:", struct.unpack_from("H", self.bs, 0x30)[0])
        print("loc. of backup sectors:", struct.unpack_from("H", self.bs, 0x32)[0])
        print("boot filename:         ", self.bs[0x34 : (0x34 + 8)])
        print("physical drive number: ", struct.unpack_from("B", self.bs, 0x40)[0])
        flags = struct.unpack_from("B", self.bs, 0x41)[0]
        print("flags etc:             ", "0x{:02X}".format(flags))
        boot_signature = struct.unpack_from("B", self.bs, 0x42)[0]
        print("extended boot sign.:   ", "0x{:02X}".format(boot_signature))
        serial_num = struct.unpack_from("I", self.bs, 0x43)[0]
        print("volume serial num:     ", f"{serial_num} (0x{serial_num:x})")

    def handle_dos71ebpb29(self):
        self.handle_dos331bpb()
        print("log. sectors per FAT:  ", struct.unpack_from("I", self.bs, 0x24)[0])
        print("mirroring flags etc:   ", struct.unpack_from("H", self.bs, 0x28)[0])
        print("version:               ", struct.unpack_from("H", self.bs, 0x2A)[0])
        print("root dir. cluster:     ", struct.unpack_from("I", self.bs, 0x2C)[0])
        print("loc. of FS info sector:", struct.unpack_from("H", self.bs, 0x30)[0])
        print("loc. of backup sectors:", struct.unpack_from("H", self.bs, 0x32)[0])
        print("boot filename:         ", self.bs[0x34 : (0x34 + 8)])
        print("physical drive number: ", struct.unpack_from("B", self.bs, 0x40)[0])
        flags = struct.unpack_from("B", self.bs, 0x41)[0]
        print("flags etc:             ", "0x{:02X}".format(flags))
        boot_signature = struct.unpack_from("B", self.bs, 0x42)[0]
        print("extended boot sign.:   ", "0x{:02X}".format(boot_signature))
        serial_num = struct.unpack_from("I", self.bs, 0x43)[0]
        print("volume serial num:     ", f"{serial_num} (0x{serial_num:x})")
        print("volume label:          ", self.bs[0x47 : (0x47 + 11)])
        print("file system type:      ", self.bs[0x52 : (0x52 + 8)])

    def handle_bpb(self):
        bpb_size = self.jump_offset - 0xB
        if bpb_size == 13:
            print("BPB: DOS 2.0 BPB")
            self.handle_dos20bpb()
        elif bpb_size == 19:
            print("BPB: DOS 3.0 BPB")
            self.handle_dos30bpb()
        elif bpb_size == 21:
            print("BPB: DOS 3.2 BPB")
            self.handle_dos32bpb()
        elif bpb_size == 25:
            print("BPB: DOS 3.31 BPB")
            self.handle_dos331bpb()
        elif bpb_size == 32:
            print("BPB: DOS 3.4 EBPB")
            self.handle_dos34bpb()
        elif bpb_size == 51:
            print("BPB: DOS 4.0 EBPB")
            self.handle_dos40ebpb()
        elif bpb_size == 60:
            print("BPB: DOS 7.1 EBPB (extended boot signature 0x28)")
            self.handle_dos71ebpb28()
        elif bpb_size == 79:
            print("BPB: DOS 7.1 EBPB (extended boot signature 0x29)")
            self.handle_dos71ebpb29()
        else:
            print("UNKNOWN BPB, size:", bpb_size)
            self.handle_dos20bpb()
            print("Bytes left before code:")
            print(self.bs[0x18 : self.jump_offset])

    def get_tot_size(self):
        return self.bytes_per_sector * self.tot_sectors

    def list_root_dir(self):
        fat_offset = self.reserved_sectors * self.bytes_per_sector
        fat_size = self.sectors_per_FAT * self.bytes_per_sector

        print("FAT offset:", fat_offset)
        print("FAT size:", fat_size)

        fat1 = self.bs[fat_offset : (fat_offset + fat_size)]
        fat2 = self.bs[(fat_offset + fat_size) : (fat_offset + 2 * fat_size)]
        assert fat1 == fat2

        root_dir_offset = fat_offset + self.num_FATs * fat_size

        # print(self.bs[root_dir_offset : root_dir_offset + 128])

        print("Root dir entries:")
        for i in range(self.num_rootdir_entries):
            entry = self.bs[
                (root_dir_offset + i * 32) : (root_dir_offset + (i + 1) * 32)
            ]

            if any(v != 0 for v in entry):
                name_we = entry[0:8]
                suffix = entry[8:11]

                attr = entry[11]
                attrs = []
                if attr & (1 << 0):
                    attrs.append("ro")
                if attr & (1 << 1):
                    attrs.append("hidden")
                if attr & (1 << 2):
                    attrs.append("sysfile")
                if attr & (1 << 3):
                    attrs.append("vollbl")
                if attr & (1 << 4):
                    attrs.append("subdir")
                if attr & (1 << 5):
                    attrs.append("archive")

                time_word = struct.unpack_from("H", entry, 22)[0]
                secs = time_word & 0x1F
                mins = (time_word >> 5) & 0x3F
                hours = (time_word >> 11) & 0x1F
                date_word = struct.unpack_from("H", entry, 24)[0]
                day = date_word & 0x1F
                month = (date_word >> 5) & 0xF
                year = 1980 + ((date_word >> 9) & 0x7F)
                start_cluster = struct.unpack_from("H", entry, 26)[0]
                size = struct.unpack_from("I", entry, 28)[0]
                datestr = f"{year}-{month:02}-{day:02}"
                timestr = f"{hours:02}:{mins:02}:{secs:02}"
                print(
                    f"  {i:4}  name_we={name_we}, suffix={suffix}, attr={attr:b} ({'|'.join(attrs)}), "
                    f"time={timestr}, date={datestr}, start={start_cluster}, size={size}"
                )
                full_name = name_we.decode().rstrip()
                if suf := suffix.decode().rstrip():
                    full_name += "." + suf
                # print(
                #     f" {i:4}  {full_name:12} {datestr} {timestr}  {size:7}  ({'|'.join(attrs)})"
                # )

    def read_file(self, path):
        fat_offset = self.reserved_sectors * self.bytes_per_sector

        print(f"fat offset: {fat_offset}, sector: {fat_offset//self.bytes_per_sector}")

        fat_size = self.sectors_per_FAT * self.bytes_per_sector
        fat1 = self.bs[fat_offset : (fat_offset + fat_size)]

        print("FAT:")
        # fatstr = fat1.hex()
        i = 0
        while True:
            bi = (i * 3) // 2
            v = struct.unpack_from("H", fat1, bi)[0]
            v = v & 0xFFF if (i % 2) == 0 else v >> 4
            if v == 0:
                break
            print(f"{v:03x}")
            i += 1

        root_dir_offset = fat_offset + self.num_FATs * fat_size

        the_entry = None
        for i in range(self.num_rootdir_entries):
            entry = self.bs[
                (root_dir_offset + i * 32) : (root_dir_offset + (i + 1) * 32)
            ]

            name_we = entry[0:8]
            suffix = entry[8:11]
            full_name = name_we.decode().rstrip()
            if suf := suffix.decode().rstrip():
                full_name += "." + suf
            if full_name.upper() == path.upper():
                the_entry = entry
                break

        if the_entry is None:
            raise RuntimeError("File not found")

        data_offset = root_dir_offset + self.num_rootdir_entries * 32

        print(
            f"data_offset: 0x{data_offset:x}, sector: {data_offset//self.bytes_per_sector}"
        )

        size = struct.unpack_from("I", the_entry, 28)[0]
        start_cluster = struct.unpack_from("H", the_entry, 26)[0]

        print("start_cluster:", start_cluster)

        cluster_size = self.sectors_per_cluster * self.bytes_per_sector
        cluster = start_cluster
        contents = b""
        while True:
            print("cluster:", cluster)
            cluster_address = data_offset + (cluster - 2) * cluster_size
            print(f"cluster_address: 0x{cluster_address:x}")
            cluster_data = self.bs[cluster_address : (cluster_address + cluster_size)]
            contents += cluster_data

            # even case, read
            even_cluster = cluster % 2 == 0
            first_byte_address = (cluster * 3) // 2
            bb = struct.unpack_from("H", fat1, first_byte_address)[0]
            fat_entry = bb & 0xFFF if even_cluster else (bb >> 4) & 0xFFF
            if fat_entry & 0xFF8 == 0xFF8:
                break
            cluster = fat_entry
        return contents[:size]


parser = argparse.ArgumentParser()
parser.add_argument("image")
subparsers = parser.add_subparsers(dest="command")
subparsers.add_parser("list_root")
read_file_parser = subparsers.add_parser("read_file")
read_file_parser.add_argument("filename")

args = parser.parse_args()
m = MbrReader(args.image)
m.handle_bpb()
print("Total Size:", m.get_tot_size() / 1024, "kB")

if args.command == "list_root":
    m.list_root_dir()
elif args.command == "read_file":
    print("Reading a file:", m.read_file(args.filename))
