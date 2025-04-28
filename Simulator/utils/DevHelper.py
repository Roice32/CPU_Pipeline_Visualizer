#!/usr/bin/env python3

'''
This script is a tool to run various commands related to the CPU Pipeline Visualizer project.
Written by ClaudeAI based on previously-used .sh scripts within this project.
'''

import os
import sys
import subprocess
import argparse
import shlex
import shutil
import readline


# -----------------------------------------------------------------------------------------------------------
class DevHelper:

  def __init__(self, projectRoot):
    self.projectRoot = os.path.abspath(projectRoot)
    self.buildDir = os.path.join(self.projectRoot, "build")
    self.utilsDir = os.path.join(self.projectRoot, "utils")
    self.testsDir = os.path.join(self.projectRoot, "tests")
    self.outputsDir = os.path.join(self.testsDir, "outputs")
    self.batchDir = os.path.join(self.testsDir, "batch")
    self.executable = "CPU_Pipeline_Simulator"  # Just the executable name, as we'll run it from buildDir

    # Ensure necessary directories exist
    os.makedirs(self.outputsDir, exist_ok=True)
    os.makedirs(self.batchDir, exist_ok=True)

  # ---------------------------------------------------------------------------------------------------------
  def RunCommand(self, command, cwd=None):
    """Run a shell command and return its status code"""
    if cwd is None:
      cwd = self.projectRoot

    print(f"Executing: {command} (in {cwd})")
    process = subprocess.Popen(
      command,
      shell=True,
      cwd=cwd,
      stdout=sys.stdout,
      stderr=sys.stderr
    )
    return process.wait()

  # ---------------------------------------------------------------------------------------------------------
  def Clean(self):
    """Remove the previously built executable"""
    print("    > Removing old executable")
    executablePath = os.path.join(self.buildDir, self.executable)
    if os.path.exists(executablePath):
      try:
        os.remove(executablePath)
        print(f"Removed {executablePath}")
        return True
      except Exception as e:
        print(f"Error removing executable: {e}")
        return False
    else:
      print("No executable found to remove.")
      return True

  # ---------------------------------------------------------------------------------------------------------
  def Build(self):
    """Build the project"""
    print("    > Generating build files with CMake")
    status = self.RunCommand("cmake build . -DCMAKE_BUILD_TYPE=Debug", cwd=self.buildDir)
    if status != 0:
      print(f"CMake failed with status code {status}")
      return False

    print("    > Building project executable with Makefile")
    status = self.RunCommand("make", cwd=self.buildDir)
    if status != 0:
      print(f"Make failed with status code {status}")
      return False

    return True

  # ---------------------------------------------------------------------------------------------------------
  def Parse(self, asm_file, hex_file=None):
    """Parse an assembly file to hex"""
    # Get absolute paths
    asmPath = os.path.join(self.projectRoot, asm_file)

    # If no hex_file specified, use the same path but with .hex extension
    if not hex_file:
      asmBase, asmExt = os.path.splitext(asm_file)
      hex_file = f"{asmBase}.hex"

    hexPath = os.path.join(self.projectRoot, hex_file)

    # Get relative paths for pyasm.py
    relAsmPath = os.path.relpath(asmPath, self.utilsDir)
    relHexPath = os.path.relpath(hexPath, self.utilsDir)

    command = f"python3 ./pyasm.py --infile {relAsmPath} --outfile {relHexPath}"
    status = self.RunCommand(command, cwd=self.utilsDir)

    if status != 0:
      print(f"Parsing failed with status code {status}")
      return False
    return True

  # ---------------------------------------------------------------------------------------------------------
  def Run(self, args):
    """Run the program with arguments"""
    executablePath = os.path.join(self.buildDir, self.executable)
    if not os.path.exists(executablePath):
      print("Error: Executable not found. Build the project first.")
      return False

    print("    > Running Program")

    if len(args) >= 1:
      hexFile = os.path.join(self.projectRoot, args[0])
      hexFileRel = os.path.relpath(hexFile, self.buildDir)

      inputName = os.path.splitext(os.path.basename(hexFile))[0]
      outputDir = os.path.join(self.outputsDir, inputName)
      os.makedirs(outputDir, exist_ok=True)

      if len(args) >= 2 and args[1]:
        outputFile = os.path.join(self.projectRoot, args[1])
        outputFileRel = os.path.relpath(outputFile, self.buildDir)
      else:
        outputFile = os.path.join(outputDir, f"{inputName}.log")
        outputFileRel = os.path.relpath(outputFile, self.buildDir)

      command = f"./{self.executable} {hexFileRel} {outputFileRel}"
      status = self.RunCommand(command.strip(), cwd=self.buildDir)

      if status != 0:
        print(f"Program execution failed with status code {status}")
        return False
      return True
    else:
      print("Usage: --run <.hex source code file> [output file]")
      return False

  # ---------------------------------------------------------------------------------------------------------
  def Test(self, test_name):
    """Run test case with memory dumping"""
    executablePath = os.path.join(self.buildDir, self.executable)
    if not os.path.exists(executablePath):
      print("Error: Executable not found. Build the project first.")
      return False

    print("    > Running test")

    baseName = os.path.basename(os.path.splitext(test_name)[0])
    outputDir = os.path.join(self.outputsDir, baseName)
    os.makedirs(outputDir, exist_ok=True)

    testPath = os.path.splitext(test_name)[0]
    hexFile = os.path.join(self.projectRoot, f"{testPath}.hex")
    hexFileRel = os.path.relpath(hexFile, self.buildDir)

    logFile = os.path.join(outputDir, f"{baseName}.log")
    logFileRel = os.path.relpath(logFile, self.buildDir)

    memDumpFile = os.path.join(outputDir, "mem_dump.log")
    memDumpFileRel = os.path.relpath(memDumpFile, self.buildDir)

    command = f"./{self.executable} {hexFileRel} {logFileRel} {memDumpFileRel}"
    status = self.RunCommand(command, cwd=self.buildDir)

    if status != 0:
      print(f"Test execution failed with status code {status}")
      return False
    return True

  # ---------------------------------------------------------------------------------------------------------
  def BatchRun(self, hex_file, num_runs):
    """Run the program multiple times without memory dumping"""
    executablePath = os.path.join(self.buildDir, self.executable)
    if not os.path.exists(executablePath):
      print("Error: Executable not found. Build the project first.")
      return False

    print("    > Running Batch")

    for file in os.listdir(self.batchDir):
      filePath = os.path.join(self.batchDir, file)
      try:
        if os.path.isfile(filePath):
          os.unlink(filePath)
        elif os.path.isdir(filePath):
          shutil.rmtree(filePath)
      except Exception as e:
        print(f"Error cleaning batch directory: {e}")

    hexFileAbs = os.path.join(self.projectRoot, hex_file)
    hexFileRel = os.path.relpath(hexFileAbs, self.buildDir)

    for i in range(1, int(num_runs) + 1):
      logFile = os.path.join(self.batchDir, f"{i}.log")
      logFileRel = os.path.relpath(logFile, self.buildDir)

      command = f"./{self.executable} {hexFileRel} {logFileRel}"
      status = self.RunCommand(command, cwd=self.buildDir)

      if status != 0:
        print(f"Run #{i} failed with status code {status}")
        return False

      print(f"Run #{i}:")
      self.RunCommand(f"tail -1 {logFileRel}", cwd=self.buildDir)

    return True


# -----------------------------------------------------------------------------------------------------------
def SetupHistory():
  """Setup readline history for command recall"""
  historyFile = os.path.expanduser('~/.cpu_pipeline_history')

  readline.set_history_length(16)

  try:
    readline.read_history_file(historyFile)
  except FileNotFoundError:
    pass

  import atexit
  atexit.register(readline.write_history_file, historyFile)


# -----------------------------------------------------------------------------------------------------------
def ParseArgs():
  parser = argparse.ArgumentParser(description="CPU Pipeline Visualizer Development Helper")
  parser.add_argument("project_root", help="Path to the project root directory")
  return parser.parse_args()


# -----------------------------------------------------------------------------------------------------------
def PrintHelp():
  """Print available commands"""
  print("\nAvailable commands:")
  print("  --build, -b                       - Build the project")
  print("  --clean, -c                       - Remove the executable")
  print("  --parse, -p <asm_file> [hex_file] - Parse assembly to hex")
  print("  --run, -r <hex_file> [out_file]   - Run the program (no memory dump)")
  print("  --test, -t <test_name>            - Run test (with memory dump)")
  print("  --batch, -bt <hex_file> <runs>    - Run the program multiple times")
  print("  --help, -h                        - Show this help")
  print("  --quit, -q                        - Exit the script")
  print("\nUse Up/Down arrow keys to navigate command history")


# -----------------------------------------------------------------------------------------------------------
def Main():
  args = ParseArgs()

  if not os.path.isdir(args.project_root):
    print(f"Error: Project root '{args.project_root}' is not a directory.")
    sys.exit(1)

  buildDir = os.path.join(args.project_root, "build")
  if not os.path.isdir(buildDir):
    print(f"Error: Build directory '{buildDir}' not found.")
    sys.exit(1)

  helper = DevHelper(args.project_root)

  SetupHistory()

  print(f"CPU Pipeline Visualizer Development Helper")
  print(f"Project root: {helper.projectRoot}")
  print("Type --help or -h for available commands")
  print("Use Up/Down arrow keys to navigate command history")

  while True:
    try:
      userInput = input("\n> ").strip()
      if not userInput:
        continue

      if userInput.strip():
        readline.add_history(userInput)

      parts = shlex.split(userInput)
      command = parts[0].lower()
      commandArgs = parts[1:] if len(parts) > 1 else []

      if command in ("--quit", "-q"):
        print("Exiting dev helper.")
        break

      elif command in ("--help", "-h"):
        PrintHelp()

      elif command in ("--clean", "-c"):
        helper.Clean()

      elif command in ("--build", "-b"):
        helper.Build()

      elif command in ("--parse", "-p"):
        if not commandArgs:
          print("Error: Missing assembly file path.")
          print("Usage: --parse <asm_file> [hex_file]")
        else:
          asmFile = commandArgs[0]
          hexFile = commandArgs[1] if len(commandArgs) > 1 else None
          helper.Parse(asmFile, hexFile)

      elif command in ("--run", "-r"):
        helper.Run(commandArgs)

      elif command in ("--test", "-t"):
        if not commandArgs:
          print("Error: Missing test name.")
          print("Usage: --test <test_name>")
        else:
          helper.Test(commandArgs[0])

      elif command in ("--batch", "-bt"):
        if len(commandArgs) < 2:
          print("Error: Missing arguments.")
          print("Usage: --batch <hex_file> <number_of_runs>")
        else:
          helper.BatchRun(commandArgs[0], commandArgs[1])

      else:
        print(f"Unknown command: {command}")
        PrintHelp()

    except KeyboardInterrupt:
      print("\nInterrupted. Use --quit or -q to exit.")
    except Exception as e:
      print(f"Error: {e}")


# -----------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
  Main()
