#!/usr/bin/env python
# -*- encoding: utf-8 -*-
# Python version: 3.8.
'''
@File    :   header_app.py
@Time    :   2022/04/17 20:42:36
@Author  :   Wu Xueru 
@Version :   1.0
@Contact :   t01051@163.com
@License :   
@Desc    :   
'''

# here put the import lib
import argparse
import struct
import time

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

def add_offset_512(file_name=''):
    f_name = file_name
    f_path = ''
    if '\\' in file_name:
        f_path,f_name = file_name.rsplit('\\', 1)
        f_path += '\\'
        print(f_path, f_name)
    if '/' in file_name:
        f_path,f_name = file_name.rsplit('/', 1)
        f_path += '/'
        print(f_path, f_name)

    with open(f_path+f_name, 'rb') as f:
        f_bin = f.read()

    print('bin file size:', len(f_bin))

    # 修改时间和长度
    f_bin = f_bin[0:520] + struct.pack('<I', int(time.time())) + f_bin[520+4:]
    f_bin = f_bin[0:528] + struct.pack('<I', len(f_bin)) + f_bin[528+4:]

    f_list = []
    end = 0
    for i in range(0, len(f_bin), 512):
        if i + 512 > len(f_bin):
            end = len(f_bin)
        else:
            end = i + 512
        f_list.append(f_bin[i:end])

    # 计算块流水校验
    sum = 0
    checksum = 0
    index = 0
    for i in f_list:
        sum += len(i)
        index += 1
        checksum = crc16(list(struct.pack('<H', checksum))+list(i))
        # print(index, '=>', hex(checksum))

    # 修改校验
    f_bin = f_bin[0:516] + struct.pack('<I', checksum) + f_bin[516+4:]

    # 保存
    with open(f_path+'header_'+f_name, 'wb') as f:
        f.write(f_bin)
    
    version = struct.unpack('<I', f_bin[512:512+4])[0]
    checksum = struct.unpack('<I', f_bin[516:516+4])[0]
    _time = struct.unpack('<I', f_bin[520:520+4])[0]
    start_addr = struct.unpack('<I', f_bin[524:524+4])[0]
    size = struct.unpack('<I', f_bin[528:528+4])[0]

    print('len:', len(f_list), 'sum:', sum) 
    print('version:', hex(version))
    print('checksum:', hex(checksum))
    print('time:', hex(_time))
    print('start_addr:', hex(start_addr))
    print('size:', hex(size), size)
    print('SP:', hex(struct.unpack('<I', f_bin[0:0+4])[0]))
    print('PC:', hex(struct.unpack('<I', f_bin[4:4+4])[0]))
    print('output file:', f_path+'header_'+f_name)


if __name__ == '__main__':

    # add_offset_512('rt-thread.bin')

    parser = argparse.ArgumentParser(description='add headre for iap bin file')
    parser.add_argument('--bin', help='bin file', type=str, required=True)
    args = parser.parse_args()

    if '.bin' in args.bin:
        add_offset_512(args.bin)
    else:
        print('you input file not .bin!')
