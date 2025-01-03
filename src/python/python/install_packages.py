import multiprocessing
import subprocess
import sys
import os

def install(package):
    print(f"Installing {package}")
    subprocess.run([sys.executable, "-m", "pip", "install", package], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    print(f"Installed {package}")

def get_pip():
    subprocess.run([sys.executable, "get-pip.py"], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

def setup(python_path):
    sys.argv = [python_path]
    multiprocessing.set_executable(os.path.join(python_path, 'python.exe'))
    sys.executable = os.path.join(python_path, 'python.exe')
