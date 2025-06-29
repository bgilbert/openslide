import os
from pathlib import Path
import shutil
import subprocess
import sys
import venv

requirements = Path(sys.argv[1])
python = Path(sys.argv[2])
env_dir = python.parent.parent

if not python.exists():
    # recreate env from scratch after "ninja clean"
    shutil.rmtree(env_dir)
if not env_dir.exists():
    venv.create(env_dir, with_pip=True)

subprocess.run(
    [
        python.with_stem('pip'),
        'install',
        '--disable-pip-version-check',
        '-q',
        '-r',
        requirements,
    ],
    check=True,
)

os.utime(python)
