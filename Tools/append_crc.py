import sys
import crcmod.predefined
import struct
from intelhex import IntelHex

crc_func = crcmod.predefined.mkCrcFun('crc-32-mpeg')

flash_start = int(sys.argv[1], 16)
flash_size = int(sys.argv[2])
bin_file = sys.argv[3]
hex_file = sys.argv[4]


with open(bin_file, 'rb') as f:
    data = f.read()
    if len(data) < flash_size:
        data += b'\xFF' * (flash_size - len(data))
    data = data[:-4]

crc = crc_func(data)
print(f'CRC: 0x{hex(crc)[2:].upper()}')
crc = struct.pack('<I', crc)
data += crc

with open(bin_file, 'wb') as f:
    f.write(data)

out_hex = IntelHex()
for a in range(flash_start, flash_start + flash_size - 4):
    out_hex[a] = 255

ih = IntelHex(hex_file)
out_hex.merge(ih, overlap='replace')

out_hex.puts(flash_start + flash_size - 4, crc)
with open(hex_file, 'w') as f:
    out_hex.write_hex_file(f)
