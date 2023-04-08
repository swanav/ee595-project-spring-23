#!/usr/bin/env python3

import argparse
import os
import shutil
import subprocess
import sys

def build() -> int:
    # Save the current files in the directory
    files: list[str] = os.listdir(os.getcwd())
    # Check if the build directory exists, if not create it
    if 'build' not in files:
        os.mkdir('build')
    # Execute the makefile
    print('Building...')
    ret: int = subprocess.run(['make']).returncode
    print('Build successful' if ret == 0 else 'Build failed')

    if ret == 0:
        # Move all the files not in the files list to the build directory
        for file in os.listdir(os.getcwd()):
            if file not in files and file != 'build':
                os.rename(file, os.path.join('build', file))
    return ret


def clean() -> int:
    print('Cleaning...')
    ret: int = subprocess.run(['make', 'clean']).returncode
    print('Clean successful' if ret == 0 else 'Clean failed')
    if ret == 0:
        shutil.rmtree('build')
    return ret

def install() -> int:
    print('Installing module...')
    cwd = os.sep.join([os.getcwd(), 'build'])
    ret: int = subprocess.run(['kmodsign', 'sha512', '/home/swanav/MOK.priv', '/home/swanav/MOK.der', 'tcp_ml.ko'], cwd=cwd).returncode
    ret = subprocess.run(['insmod', 'tcp_ml.ko'], cwd=cwd).returncode
    print('Install successful' if ret == 0 else 'Install failed')
    return ret

def uninstall() -> int:
    print('Removing module...')
    cwd: str = os.sep.join([os.getcwd(), 'build'])
    ret: int = subprocess.run(['rmmod', 'tcp_ml'], cwd=cwd).returncode
    print('Remove successful' if ret == 0 else 'Remove failed')
    return ret

def activate() -> int:
    print('Activating module...')
    ret: int = subprocess.run(['sysctl', 'net.ipv4.tcp_congestion_control={}'.format('tcp_ml')]).returncode
    print('Activate successful' if ret == 0 else 'Activate failed')
    return ret

def deactivate() -> int:
    print('Deactivating module...')
    ret: int = subprocess.run(['sysctl', 'net.ipv4.tcp_congestion_control={}'.format('cubic')]).returncode
    print('Deactivate successful' if ret == 0 else 'Deactivate failed')
    return ret

def available() -> int:
    print('Available TCP Congestion Control:')
    ret: int = subprocess.run(['cat', '/proc/sys/net/ipv4/tcp_available_congestion_control']).returncode
    return ret

def query() -> int:
    print('Current TCP Congestion Control:')
    ret: int = subprocess.run(['cat', '/proc/sys/net/ipv4/tcp_congestion_control']).returncode
    return ret

switcher = {
    'build': build,
    'clean': clean,
    'install': install,
    'uninstall': uninstall,
    'activate': activate,
    'deactivate': deactivate,
    'available': available,
    'query': query
}

def main(args: argparse.Namespace) -> int:
    ret: int = 0
    fnc = switcher.get(args.action)
    if fnc:
        ret = fnc()
    else:
        print('Invalid action', file=sys.stderr)
        return 1
    return ret

if __name__ == '__main__':
    argparser: argparse.ArgumentParser = argparse.ArgumentParser()
    argparser.add_argument('action', choices=['build', 'clean', 'install', 'uninstall', 'activate', 'deactivate', 'available', 'query'], help='Action to perform')
    args: argparse.Namespace = argparser.parse_args()
    
    sys.exit(main(args))