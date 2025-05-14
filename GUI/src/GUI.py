import sys
import os
import shutil
from PyQt5.QtWidgets import QApplication, QMainWindow, QTabWidget
from PyQt5.QtCore import QProcess

# Import tab modules
from InputTab import InputTab
from ConfigTab import ConfigTab
from SimulationTab import SimulationTab
from HelpTab import HelpTab


# -----------------------------------------------------------------------------------------------------------------------------
class CPUSimulator(QMainWindow):
  tempDir: os.path

  clockPeriod:   int
  deWorkMemory:  int
  icCacheSize:   int
  lsCacheSize:   int
  exCyclesPerOp: int

  inputTab:      InputTab
  configTab:     ConfigTab
  simulationTab: SimulationTab
  helpTab:       HelpTab

  # ---------------------------------------------------------------------------------------------------------------------------
  def __init__(self):
    super().__init__()

    # Create temp directory
    self.tempDir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "CPU_PV.temp")
    if not os.path.exists(self.tempDir):
      os.makedirs(self.tempDir)
    simulationCpuStatesDir = os.path.join(self.tempDir, "simulation", "cpu_states")
    if not os.path.exists(simulationCpuStatesDir):
      os.makedirs(simulationCpuStatesDir)
    simulationMemoryDir = os.path.join(self.tempDir, "simulation", "memory")
    if not os.path.exists(simulationMemoryDir):
      os.makedirs(simulationMemoryDir)
    
    # Initialize UI
    self.setWindowTitle("CPU Simulator")
    self.setGeometry(100, 100, 1000, 600)

    # Initialize config values
    self.clockPeriod = 1
    self.deWorkMemory = 2
    self.icCacheSize = 64
    self.lsCacheSize = 64
    self.exCyclesPerOp = 5
    
    # Create process for handling external commands
    self.process = QProcess()
    
    # Create tab widget
    self.tabs = QTabWidget()
    self.setCentralWidget(self.tabs)
    
    # Create tabs
    self.inputTab = InputTab(self)
    self.configTab = ConfigTab(self)
    self.simulationTab = SimulationTab(self)
    self.helpTab = HelpTab(self)

    # Add tabs to widget
    self.tabs.addTab(self.inputTab, "Input")
    self.tabs.addTab(self.configTab, "Config")
    self.tabs.addTab(self.simulationTab, "Simulation")
    self.tabs.setTabEnabled(self.tabs.indexOf(self.simulationTab), False)  # Disable the Simulation tab without any simulation
    self.tabs.addTab(self.helpTab, "Help")

  # ---------------------------------------------------------------------------------------------------------------------------
  def GetTempDir(self):
    return self.tempDir

  # ---------------------------------------------------------------------------------------------------------------------------
  def GetSimulationDir(self):
    return os.path.join(self.tempDir, "simulation")

  # ---------------------------------------------------------------------------------------------------------------------------
  def GetSimulationCpuStatesDir(self):
    return os.path.join(self.tempDir, "simulation", "cpu_states")
  
  # ---------------------------------------------------------------------------------------------------------------------------
  def GetSimulationMemoryDir(self):
    return os.path.join(self.tempDir, "simulation", "memory")

  # ---------------------------------------------------------------------------------------------------------------------------
  def GetConfigValues(self):
    return {
      "clockPeriod": self.clockPeriod,
      "deWorkMemory": self.deWorkMemory,
      "icCacheSize": self.icCacheSize,
      "lsCacheSize": self.lsCacheSize,
      "exCyclesPerOp": self.exCyclesPerOp
    }

  # ---------------------------------------------------------------------------------------------------------------------------
  def UpdateClockPeriod(self, value):
    self.clockPeriod = value

  # ---------------------------------------------------------------------------------------------------------------------------
  def UpdateDeWorkMemory(self, value):
    self.deWorkMemory = value

  # ---------------------------------------------------------------------------------------------------------------------------
  def UpdateIcCacheSize(self, value):
    self.icCacheSize = value

  # ---------------------------------------------------------------------------------------------------------------------------
  def UpdateLsCacheSize(self, value):
    self.lsCacheSize = value

  # ---------------------------------------------------------------------------------------------------------------------------
  def UpdateExCyclesPerOp(self, value):
    self.exCyclesPerOp = value

  # ---------------------------------------------------------------------------------------------------------------------------
  def SwitchToSimulationTab(self):
    self.tabs.setCurrentIndex(2)  # Index 2 is the Simulation tab

  # ---------------------------------------------------------------------------------------------------------------------------
  def SetSimulationTabEnabled(self, enabled):
    self.tabs.setTabEnabled(self.tabs.indexOf(self.simulationTab), enabled)

  # ---------------------------------------------------------------------------------------------------------------------------
  def closeEvent(self, event):
    # Clean up temp directory on exit
    if os.path.exists(self.tempDir):
      try:
        shutil.rmtree(self.tempDir)
      except Exception as e:
        print(f"Error removing temp directory: {str(e)}")
    event.accept()


# -----------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
  app = QApplication(sys.argv)
  window = CPUSimulator()
  window.showMaximized()
  sys.exit(app.exec_())
