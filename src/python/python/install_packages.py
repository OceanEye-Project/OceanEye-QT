import multiprocessing
import subprocess
import sys
import os

def install(package):
    print(f"Installing {package}")
    try:
        subprocess.run([sys.executable, "-m", "pip", "install", package], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print(f"Installed {package}")

    except subprocess.CalledProcessError:
        print(f"Failed to install {package}")

def ensure_pip():
    try:
        subprocess.run([sys.executable, "-m", "pip", "--version"], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print("Found Pip")

    except subprocess.CalledProcessError:
        print("Installing Pip")

        subprocess.run([sys.executable, "get-pip.py"], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        print("Installed Pip")

def setup(python_path):
    """ makes sure everything works smoothly in this enviroment """
    python_exe = 'pythonw.exe' # 'python.exe'
    sys.argv = [python_path]
    multiprocessing.set_executable(os.path.join(python_path, python_exe))
    sys.executable = os.path.join(python_path, python_exe)
