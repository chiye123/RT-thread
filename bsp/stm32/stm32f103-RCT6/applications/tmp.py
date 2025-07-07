import struct

def crc16(x):
    a = 0xFFFF
    b = 0xA001
    for i in x:
        a ^= i if isinstance(i, int) else ord(i)
        for i in range(8):
            last = a % 2
            a >>= 1
            if last == 1:
                a ^= b
    return a

def create_update_pack(deviceType=0, deviceNum=1, data=()):
    tmp = []
    tmp += struct.pack('B', deviceType)
    tmp += struct.pack('B', deviceNum)
    tmp += struct.pack('>H', len(data)-1)
    tmp += struct.pack('B', data[0])
    for i in data[1:]:
        if deviceType == 9:
            tmp += struct.pack('B', i if isinstance(i,int) else ord(i))
        else:
            tmp += struct.pack('>H', i if isinstance(i,int) else ord(i))
    crc = struct.pack('>H', crc16(tmp))
    tmp += crc
    print(tmp)
    # return tmp

create_update_pack(1, 1, (0,123))
create_update_pack(9, 1, (0,'a','b'))

def pkg_decode(data):
    DeviceID = data[:12].decode('utf-8')
    deviceType = data[12]
    deviceNum = data[13]
    recv_data_len = struct.unpack('>H', data[14:16])[0]
    deviceErroeCode = data[16]
    recv_crc = struct.unpack('>H', data[-2:])[0]
    crc = crc16(data[12:-2])
    print('DeviceID:', DeviceID)
    print('deviceType:', deviceType)
    print('deviceNum:', deviceNum)
    print('recv_data_len:', recv_data_len)
    print('deviceErroeCode:', deviceErroeCode)
    print('recv_crc:', recv_crc)
    print('crc:', crc)

data = struct.pack('B', [0x53,0x54,0x4D,0x33,0x32,0x46,0x31,0x30,0x33,0x52,0x43,0x54,0x01,0x01,0x00,0x01,0x00,0x00,0x7B,0xCE,0x4B])
data = b'\x53\x54\x4D\x33\x32\x46\x31\x30\x33\x52\x43\x54\x01\x01\x00\x01\x00\x00\x7B\xCE\x4B'
pkg_decode(data)
pkg_decode([1, 1, 0, 1, 0, 0, 123, 206, 75])