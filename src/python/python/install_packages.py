import multiprocessing
import subprocess
import sys
import os

def install(package):
    print(f"Installing {package}")
    subprocess.run([sys.executable, "-m", "pip", "install", package], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    print(f"Installed {package}")

def ls():
    # run `pip list` to list all installed packages
    print("Listing installed packages")
    # convert to multiprocessing:
    subprocess.run(['ls'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

def main():
    # print(sys.argv)
    # print(sys.path)
    sys.argv = [sys.path[-1]]
    multiprocessing.set_executable(os.path.join(sys.exec_prefix, 'bin', 'python3'))
    sys.executable = os.path.join(sys.exec_prefix, 'bin', 'python3')
    # print(sys.executable)
    # print(sys.exec_prefix)
    # print(os.
    # ls()
    install("ultralytics")