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

        print(f"Installed {package}")s

    except subprocess.CalledProcessError:
        print(f"Failed to install {package}")

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
