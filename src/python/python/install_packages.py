import multiprocessing
import subprocess
import sys
from pathlib import Path
import re

def install(package):
    print(f"Installing {package} (this may take a few minutes)")
    try:
        args = [sys.executable, "-m", "pip", "install"]
        args += package
        args += ["--no-warn-script-location", "--user"]

        result = subprocess.run(args)

        result.check_returncode()

        print(f"Installed {package}")

    except subprocess.CalledProcessError:
        print(f"Failed to install {package}")

def get_cuda_version():
    try:
        result = subprocess.run(["nvcc", "--version"],
            capture_output = True,
            text = True,
            shell = True
        )
        result.check_returncode()

        version = re.search("release (\d+)\.(\d+)", result.stdout)

        if version is None:
            print("CUDA found, could not parse version info")

            return None

        version_major = int(version[1])
        version_minor = int(version[2])

        print(f"Found CUDA version {version_major}.{version_minor}")

        return (version_major, version_minor)

    except (FileNotFoundError, subprocess.CalledProcessError):
        print("Could not find CUDA")

        return None

def install_dependencies():
    torch_index_url = None

    cuda_version = get_cuda_version()

    if cuda_version is not None:
        version_major, version_minor = cuda_version
        torch_cuda_version = None

        if version_major >= 12 and version_minor >= 4:
            torch_index_url = "https://download.pytorch.org/whl/cu124"
            torch_cuda_version = "12.4"

        elif version_major >= 12 and version_minor >= 1:
            torch_index_url = "https://download.pytorch.org/whl/cu121"
            torch_cuda_version = "12.1"

        elif version_major >= 11 and version_minor >= 8:
            torch_index_url = "https://download.pytorch.org/whl/cu118"
            torch_cuda_version = "11.8"

        if torch_cuda_version is not None:
            print(f"Installing torch for CUDA {torch_cuda_version}")

    torch_install_args = ["torch", "torchvision", "torchaudio"]

    if torch_index_url is not None:
        torch_install_args += ["--index-url", torch_index_url]

    install(torch_install_args)

    install(["ultralytics==8.3.59"])


def ensure_pip():
    try:
        result = subprocess.run([sys.executable, "-m", "pip", "--version"],
            capture_output = True,
            text = True
        )
        print(result.stdout.strip())
        result.check_returncode()
        print("Found Pip")

    except subprocess.CalledProcessError:
        print("Installing Pip")

        python_path = Path(sys.executable)
        get_pip_path = str(python_path.parent / "get-pip.py")

        result = subprocess.run([sys.executable, get_pip_path, "--no-warn-script-location", "--user"],
            capture_output = True,
            text = True
        )
        print(result.stdout)

        print("Installed Pip")

def setup(python_path):
    """ makes sure everything works smoothly in this enviroment """
    python_exe = 'pythonw.exe' # 'python.exe'
    sys.argv = [python_path]
    multiprocessing.set_executable(str(Path(python_path) / python_exe))
    sys.executable = str(Path(python_path) / python_exe)

    print(f"Using {sys.executable}")
