import subprocess
import os


resources = [
  ('resources', 'resources'),
  ('resources/icons', 'resources/icons'),
]

dependencies = [
  ('dependencies/pyasm.py', 'dependencies'),
  ('dependencies/asm_cfg.yml', 'dependencies'),
  ('dependencies/example.asm', 'dependencies'),
  ('dependencies/CPU_Pipeline_Simulator.exe', 'dependencies'),
  ('dependencies/libgcc_s_seh-1.dll', 'dependencies'),
  ('dependencies/libstdc++-6.dll', 'dependencies'),
  ('dependencies/libwinpthread-1.dll', 'dependencies')
]

# Define the command and working directory
pyInstallerCommand = [
  "pyinstaller",
  "GUI.py",
  '--onefile',
  '--name', 'CPU Pipeline Visualizer',
  '--icon', 'resources/CPU_PV.ico'
]

for src, dest in resources:
  pyInstallerCommand.append('--add-data')
  pyInstallerCommand.append(f'{src};{dest}')

for src, dest in dependencies:
  pyInstallerCommand.append('--add-data')
  pyInstallerCommand.append(f'{src};{dest}')

GuiSrcDir = os.path.join(os.path.dirname(__file__), "../GUI/src")

# Run the command
result = subprocess.run(pyInstallerCommand, cwd=GuiSrcDir)

# Print result
if result.returncode == 0:
  print("PyInstaller build completed successfully.")
else:
  print(f"PyInstaller build failed with exit code {result.returncode}.")
  exit(1)

# Copy the generated executable to the Release directory
releaseDir = os.path.join(os.path.dirname(__file__))

exeName = 'CPU Pipeline Visualizer.exe'
exeSrcPath = os.path.join(GuiSrcDir, 'dist', exeName)

copyExeCommand = ['copy', exeSrcPath, releaseDir]
result = subprocess.run(copyExeCommand, shell=True)

# Print result of copy operation
if result.returncode == 0:
  print(f"Executable copied to {releaseDir}.")
else:
  print(f"Failed to copy executable with exit code {result.returncode}.")
  exit(1)