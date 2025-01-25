import multiprocessing
import subprocess
import sys
from pathlib import Path

def install(package):
    print(f"Installing {package} (this may take a few minutes)")
    try:
        subprocess.run(
            [sys.executable, "-m", "pip", "install", package, "--no-warn-script-location", "--user"],
        )

        print(f"Installed {package}")

    except subprocess.CalledProcessError:
        print(f"Failed to install {package}")

def find_cuda():
    try:
        import torch
    except ImportError:
        print("Torch is not installed")
        return

    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    print('Using device:', device)
    print()

    #Additional Info when using cuda
    if device.type == 'cuda':
        print(torch.cuda.get_device_name(0))
        print('Memory Usage:')
        print('Allocated:', round(torch.cuda.memory_allocated(0)/1024**3,1), 'GB')
        print('Cached:   ', round(torch.cuda.memory_reserved(0)/1024**3,1), 'GB')

def ensure_pip():
    try:
        subprocess.run([sys.executable, "-m", "pip", "--version"], check=True)

        print("Found Pip")

    except subprocess.CalledProcessError:
        print("Installing Pip")

        python_path = Path(sys.executable)
        get_pip_path = str(python_path.parent / "get-pip.py")

        subprocess.run([sys.executable, get_pip_path, "--no-warn-script-location", "--user"])

        print("Installed Pip")

def setup(python_path):
    """ makes sure everything works smoothly in this enviroment """
    python_exe = 'pythonw.exe' # 'python.exe'
    sys.argv = [python_path]
    multiprocessing.set_executable(str(Path(python_path) / python_exe))
    sys.executable = str(Path(python_path) / python_exe)
