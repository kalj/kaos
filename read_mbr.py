#!/usr/bin/env python3

import struct
import sys

with open(sys.argv[1], "rb") as f:
    bs = f.read(512)

jump_bytes = bs[0:3]
assert jump_bytes[0] == 0xEB
assert jump_bytes[2] == 0x90
jump_offset = bs[1] + 2


def handle_dos20bpb():
    OEM_name = bs[3:0xB].decode()
    bytes_per_sector = struct.unpack_from("H", bs, 0xB)[0]
    sectors_per_cluster = struct.unpack_from("B", bs, 0xD)[0]
    reserved_sectors = struct.unpack_from("H", bs, 0xE)[0]
    num_FATs = struct.unpack_from("B", bs, 0x10)[0]
    num_rootdir_entries = struct.unpack_from("H", bs, 0x11)[0]
    tot_sectors = struct.unpack_from("H", bs, 0x13)[0]
    media_descriptor = struct.unpack_from("B", bs, 0x15)[0]
    sectors_per_FAT = struct.unpack_from("H", bs, 0x16)[0]

    print("jump offset:           ", f"0x{jump_offset:X}")
    print("OEM name:              ", OEM_name)
    print("bytes per sector:      ", bytes_per_sector)
    print("sectors per cluster:   ", sectors_per_cluster)
    print("reserved sectors:      ", reserved_sectors)
    print("num FATs:              ", num_FATs)
    print("num rootdir entries:   ", num_rootdir_entries)
    print("tot log. sectors:      ", tot_sectors)
    print("media descriptor:      ", media_descriptor)
    print("log. sectors per FAT:  ", sectors_per_FAT)


def handle_dos30bpb():
    handle_dos20bpb()
    print("phy. sectors per track:", struct.unpack_from("H", bs, 0x18)[0])
    print("number of heads:       ", struct.unpack_from("H", bs, 0x1A)[0])
    print("hidden sectors:        ", struct.unpack_from("H", bs, 0x1C)[0])


def handle_dos32bpb():
    handle_dos30bpb()
    print("total sectors:         ", struct.unpack_from("H", bs, 0x1E)[0])


def handle_dos331bpb():
    handle_dos20bpb()
    print("phy. sectors per track:", struct.unpack_from("H", bs, 0x18)[0])
    print("number of heads:       ", struct.unpack_from("H", bs, 0x1A)[0])
    print("hidden sectors:        ", struct.unpack_from("I", bs, 0x1C)[0])
    print("large tot log sectors: ", struct.unpack_from("I", bs, 0x20)[0])


def handle_dos34bpb():
    handle_dos331bpb()
    print("physical drive number: ", struct.unpack_from("B", bs, 0x24)[0])
    flags = struct.unpack_from("B", bs, 0x25)[0]
    print("flags etc:             ", "0x{:X}".format(flags))
    boot_signature = struct.unpack_from("B", bs, 0x26)[0]
    print("extended boot sign.:   ", "0x{:X}".format(boot_signature))
    print("vol. serial num:       ", struct.unpack_from("I", bs, 0x27)[0])


def handle_dos40ebpb():
    handle_dos331bpb()
    print("physical drive number: ", struct.unpack_from("B", bs, 0x24)[0])
    flags = struct.unpack_from("B", bs, 0x25)[0]
    print("flags etc:             ", "0x{:X}".format(flags))
    boot_signature = struct.unpack_from("B", bs, 0x26)[0]
    print("extended boot sign.:   ", "0x{:X}".format(boot_signature))
    print("volume serial num:     ", struct.unpack_from("I", bs, 0x27)[0])
    print("volume label:          ", bs[0x2B : (0x2B + 11)])


def handle_dos71ebpb28():
    handle_dos331bpb()
    print("log. sectors per FAT:  ", struct.unpack_from("I", bs, 0x24)[0])
    print("mirroring flags etc:   ", struct.unpack_from("H", bs, 0x28)[0])
    print("version:               ", struct.unpack_from("H", bs, 0x2A)[0])
    print("root dir. cluster:     ", struct.unpack_from("I", bs, 0x2C)[0])
    print("loc. of FS info sector:", struct.unpack_from("H", bs, 0x30)[0])
    print("loc. of backup sectors:", struct.unpack_from("H", bs, 0x32)[0])
    print("boot filename:         ", bs[0x34 : (0x34 + 8)])
    print("physical drive number: ", struct.unpack_from("B", bs, 0x40)[0])
    flags = struct.unpack_from("B", bs, 0x41)[0]
    print("flags etc:             ", "0x{:X}".format(flags))
    boot_signature = struct.unpack_from("B", bs, 0x42)[0]
    print("extended boot sign.:   ", "0x{:X}".format(boot_signature))
    print("volume serial num:     ", struct.unpack_from("I", bs, 0x43)[0])


def handle_dos71ebpb29():
    handle_dos331bpb()
    print("log. sectors per FAT:  ", struct.unpack_from("I", bs, 0x24)[0])
    print("mirroring flags etc:   ", struct.unpack_from("H", bs, 0x28)[0])
    print("version:               ", struct.unpack_from("H", bs, 0x2A)[0])
    print("root dir. cluster:     ", struct.unpack_from("I", bs, 0x2C)[0])
    print("loc. of FS info sector:", struct.unpack_from("H", bs, 0x30)[0])
    print("loc. of backup sectors:", struct.unpack_from("H", bs, 0x32)[0])
    print("boot filename:         ", bs[0x34 : (0x34 + 8)])
    print("physical drive number: ", struct.unpack_from("B", bs, 0x40)[0])
    flags = struct.unpack_from("B", bs, 0x41)[0]
    print("flags etc:             ", "0x{:X}".format(flags))
    boot_signature = struct.unpack_from("B", bs, 0x42)[0]
    print("extended boot sign.:   ", "0x{:X}".format(boot_signature))
    print("volume serial num:     ", struct.unpack_from("I", bs, 0x43)[0])
    print("volume label:          ", bs[0x47 : (0x47 + 11)])
    print("file system type:      ", bs[0x52 : (0x52 + 8)])


bpb_size = jump_offset - 0xB
if bpb_size == 13:
    print("BPB: DOS 2.0 BPB")
    handle_dos20bpb()
elif bpb_size == 19:
    print("BPB: DOS 3.0 BPB")
    handle_dos30bpb()
elif bpb_size == 21:
    print("BPB: DOS 3.2 BPB")
    handle_dos32bpb()
elif bpb_size == 25:
    print("BPB: DOS 3.31 BPB")
    handle_dos331bpb()
elif bpb_size == 32:
    print("BPB: DOS 3.4 EBPB")
    handle_dos34bpb()
elif bpb_size == 51:
    print("BPB: DOS 4.0 EBPB")
    handle_dos40ebpb()
elif bpb_size == 60:
    print("BPB: DOS 7.1 EBPB (extended boot signature 0x28)")
    handle_dos71ebpb28()
elif bpb_size == 79:
    print("BPB: DOS 7.1 EBPB (extended boot signature 0x29)")
    handle_dos71ebpb29()
else:
    print("UNKNOWN BPB, size:", bpb_size)
    handle_dos20bpb()
    print("Bytes left before code:")
    print(bs[0x18:jump_offset])


# print("...")
# print("Physical drive num: ", bs[0x1FD])
