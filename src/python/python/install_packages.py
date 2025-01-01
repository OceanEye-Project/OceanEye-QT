import multiprocessing
import subprocess
import sys
import os

def install(package):
    print(f"Installing {package}")
    subprocess.run([sys.executable, "-m", "pip", "install", package], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    print(f"Installed {package}")

def get_pip():
    print(sys.executable)
    subprocess.run([sys.executable, "get-pip.py"], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

def setup():
    sys.argv = [sys.path[-1]]
    python_path = os.path.join(sys.path[-1], 'python3.12-embed')
    multiprocessing.set_executable(os.path.join(python_path, 'python.exe'))
    sys.executable = os.path.join(python_path, 'python.exe')
