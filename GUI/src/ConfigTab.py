from PyQt5.QtWidgets import QWidget, QFormLayout, QSpinBox


# -----------------------------------------------------------------------------------------------------------------------------
class ConfigTab(QWidget):
  parent = None
  clockPeriodInput = None
  deWorkMemoryInput = None
  icCacheInput = None
  lsCacheInput = None
  exCyclesInput = None

  # ---------------------------------------------------------------------------------------------------------------------------
  def __init__(self, parent):
    super().__init__()
    self.parent = parent

    layout = QFormLayout()

    # Get initial config values
    config = parent.GetConfigValues()

    # CLOCK_PERIOD_MILLIS
    self.clockPeriodInput = QSpinBox()
    self.clockPeriodInput.setRange(1, 100)
    self.clockPeriodInput.setValue(config["clockPeriod"])
    self.clockPeriodInput.valueChanged.connect(self.UpdateClockPeriod)
    layout.addRow("CLOCK_PERIOD_MILLIS:", self.clockPeriodInput)

    # DE_WORK_MEMORY_FW_SIZE
    self.deWorkMemoryInput = QSpinBox()
    self.deWorkMemoryInput.setRange(2, 5)
    self.deWorkMemoryInput.setValue(config["deWorkMemory"])
    self.deWorkMemoryInput.valueChanged.connect(self.UpdateDeWorkMemory)
    layout.addRow("DE_WORK_MEMORY_FW_SIZE:", self.deWorkMemoryInput)

    # IC_CACHE_WORDS_SIZE
    self.icCacheInput = QSpinBox()
    self.icCacheInput.setRange(16, 1024)
    self.icCacheInput.setValue(config["icCacheSize"])
    self.icCacheInput.valueChanged.connect(self.UpdateIcCacheSize)
    layout.addRow("IC_CACHE_WORDS_SIZE:", self.icCacheInput)

    # LS_CACHE_WORDS_SIZE
    self.lsCacheInput = QSpinBox()
    self.lsCacheInput.setRange(16, 1024)
    self.lsCacheInput.setValue(config["lsCacheSize"])
    self.lsCacheInput.valueChanged.connect(self.UpdateLsCacheSize)
    layout.addRow("LS_CACHE_WORDS_SIZE:", self.lsCacheInput)

    # EX_CYCLES_PER_OP
    self.exCyclesInput = QSpinBox()
    self.exCyclesInput.setRange(4, 100)
    self.exCyclesInput.setValue(config["exCyclesPerOp"])
    self.exCyclesInput.valueChanged.connect(self.UpdateExCyclesPerOp)
    layout.addRow("EX_CYCLES_PER_OP:", self.exCyclesInput)

    self.setLayout(layout)

  # ---------------------------------------------------------------------------------------------------------------------------
  def UpdateClockPeriod(self, value):
    self.parent.UpdateClockPeriod(value)

  # ---------------------------------------------------------------------------------------------------------------------------
  def UpdateDeWorkMemory(self, value):
    self.parent.UpdateDeWorkMemory(value)

  # ---------------------------------------------------------------------------------------------------------------------------
  def UpdateIcCacheSize(self, value):
    self.parent.UpdateIcCacheSize(value)

  # ---------------------------------------------------------------------------------------------------------------------------
  def UpdateLsCacheSize(self, value):
    self.parent.UpdateLsCacheSize(value)

  # ---------------------------------------------------------------------------------------------------------------------------
  def UpdateExCyclesPerOp(self, value):
    self.parent.UpdateExCyclesPerOp(value)

  # ---------------------------------------------------------------------------------------------------------------------------
  def GetConfigValues(self):
    return {
      "clockPeriod": self.clockPeriodInput.value(),
      "deWorkMemory": self.deWorkMemoryInput.value(),
      "icCacheSize": self.icCacheInput.value(),
      "lsCacheSize": self.lsCacheInput.value(),
      "exCyclesPerOp": self.exCyclesInput.value()
    }


# -----------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
  raise Exception("This module is not meant to be run directly.")
