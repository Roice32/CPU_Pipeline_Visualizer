from PyQt5.QtWidgets import QWidget, QFormLayout, QSpinBox, QPushButton
from PyQt5.QtWidgets import QHBoxLayout
from PyQt5.QtWidgets import QGroupBox, QVBoxLayout, QLabel
from PyQt5.QtWidgets import QScrollArea, QWidget


# -----------------------------------------------------------------------------------------------------------------------------
class Config:
  autoplay_speed_millis:           int

  clock_period_millis:             int
  ic_cycles_per_op:                int
  ic_cycles_per_op_with_cache_hit: int
  ls_cycles_per_op:                int
  ls_cycles_per_op_with_cache_hit: int
  de_cycles_per_op:                int
  ex_cycles_per_op:                int

  # ---------------------------------------------------------------------------------------------------------------------------
  def __init__(self) -> None:
    self.autoplay_speed_millis           = 200
    self.clock_period_millis             = 1
    self.ic_cycles_per_op                = 3
    self.ic_cycles_per_op_with_cache_hit = 2
    self.ls_cycles_per_op                = 15
    self.ls_cycles_per_op_with_cache_hit = 7
    self.de_cycles_per_op                = 2
    self.ex_cycles_per_op                = 5


# -----------------------------------------------------------------------------------------------------------------------------
class ConfigTab(QWidget):
  buttonStyleSheet = """
    QPushButton {
        border: 1px solid black;
        border-radius: 5px;
        height: 30%;
        width: 40%;
        background-color: #f0f0f0;
        font-weight: bold;
    }
    QPushButton:hover {
        background-color: #d0d0d0;
    }
    QPushButton:pressed {
        border: 2px solid #007BFF;
    }
"""

  parent = None
  currConfig = None

  autoplaySpeedInput = None
  clockPeriodInput = None
  icCyclesPerOpInput = None
  icCyclesPerOpWithCacheHitInput = None
  lsCyclesPerOpInput = None
  lsCyclesPerOpWithCacheHitInput = None
  deCyclesPerOpInput = None
  exCyclesPerOpInput = None

  # ---------------------------------------------------------------------------------------------------------------------------
  def __init__(self, parent: QWidget) -> None:
    super().__init__()
    self.parent = parent
    layout = QFormLayout()
    layout.setContentsMargins(10, 10, 10, 10)
    layout.setSpacing(10)

    # Set font for the widget and its children
    font = self.font()
    font.setPointSize(10)
    font.setBold(True)
    self.setFont(font)

    self.currConfig = Config()

    # Create a scrollable area for the config elements
    scrollArea = QScrollArea()
    scrollArea.setWidgetResizable(True)
    scrollWidget = QWidget()
    scrollLayout = QFormLayout()
    scrollLayout.setContentsMargins(0, 0, 0, 0)
    scrollLayout.setSpacing(10)

    # Autoplay Speed
    self.autoplaySpeedInput = self.GenConfigElement(scrollLayout,
                            "Autoplay Speed",
                            (10, 10_000),
                            "Sets the speed of the autoplay in Simulation tab, in milliseconds."
                            + "\nCan be changed at any time, without affecting the current simulation.")

    # Add a horizontal line
    line = QLabel()
    line.setFrameShape(QLabel.HLine)
    line.setFrameShadow(QLabel.Sunken)
    scrollLayout.addRow(line)

    # CLOCK_PERIOD_MILLIS
    self.clockPeriodInput = self.GenConfigElement(scrollLayout,
                            "Clock Period",
                            (1, 100),
                            "Sets the CPU's clock to tick every X milliseconds.")
    # IC_CYCLES_PER_OP
    self.icCyclesPerOpInput = self.GenConfigElement(scrollLayout,
                            "IC Cycles per Op",
                            (1, 100),
                            "The number of cycles the Instruction Cache takes to execute an operation"
                            + " (deliver a fetch window to DE)."
                            + "\nDoes not include cycles when awaiting data from LS.")
    # IC_CYCLES_PER_OP_WITH_CACHE_HIT
    self.icCyclesPerOpWithCacheHitInput = self.GenConfigElement(scrollLayout,
                                  "IC Cycles per Op with Cache Hit",
                                  (1, 100),
                                  "The number of cycles the Instruction Cache takes to execute an"
                                   + " operation when the data is in the cache & valid.")
    # LS_CYCLES_PER_OP
    self.lsCyclesPerOpInput = self.GenConfigElement(scrollLayout,
                            "LS Cycles per Op",
                            (1, 100),
                            "The number of cycles the Load Store unit takes to execute an operation"
                            + " (deliver a fetch window to DE; deliver data to EX; store data from EX).")
    # LS_CYCLES_PER_OP_WITH_CACHE_HIT
    self.lsCyclesPerOpWithCacheHitInput = self.GenConfigElement(scrollLayout,
                                  "LS Cycles per Op with Cache Hit",
                                  (1, 100),
                                  "The number of cycles the Load Store unit takes to execute an"
                                   + " operation when the data is in the cache & valid.")
    # DE_CYCLES_PER_OP
    self.deCyclesPerOpInput = self.GenConfigElement(scrollLayout,
                            "DE Cycles per Op",
                            (1, 100),
                            "The number of cycles the Decode unit takes to execute an operation"
                            + " (decode an instruction & send it to EX)."
                            + "\nDoes not include cycles when awaiting a fetch window from IC.")
    # EX_CYCLES_PER_OP
    self.exCyclesPerOpInput = self.GenConfigElement(scrollLayout,
                            "EX Cycles per Op",
                            (1, 100),
                            "The number of cycles the Execute unit takes to execute an operation"
                            + " (execute an instruction)."
                            + "\nDoes not include cycles when awaiting data from LS.")

    scrollWidget.setLayout(scrollLayout)
    scrollArea.setWidget(scrollWidget)
    layout.addRow(scrollArea)

    # Buttons layout
    buttonsLayout = QHBoxLayout()
    buttonsLayout.addStretch(1)  # Add stretchable space before buttons

    self.saveButton = QPushButton("Save")
    self.saveButton.clicked.connect(self.Save)
    self.saveButton.setStyleSheet(ConfigTab.buttonStyleSheet)
    buttonsLayout.addWidget(self.saveButton)

    self.resetButton = QPushButton("Reset")
    self.resetButton.clicked.connect(lambda: self.SetConfig(Config()))
    self.resetButton.setStyleSheet(ConfigTab.buttonStyleSheet)
    buttonsLayout.addWidget(self.resetButton)

    buttonsLayout.addStretch(1)  # Add stretchable space after buttons

    layout.addRow(buttonsLayout)

    self.SetConfig(self.currConfig)
    self.setLayout(layout)

  # ---------------------------------------------------------------------------------------------------------------------------
  def GenConfigElement(self, top_layout: QFormLayout, title: str, range: tuple[int, int], description: str) -> QSpinBox:
    groupBox = QGroupBox(title)
    groupBox.setFlat(True)
    layout = QVBoxLayout()

    rangeLabel = QLabel(f"[{range[0]} - {range[1]}]")
    rangeLabel.setWordWrap(True)
    rangeLabel.setStyleSheet("font-size: 9pt;")
    layout.addWidget(rangeLabel)

    descriptionLabel = QLabel(f"{description}")
    descriptionLabel.setWordWrap(True)
    descriptionLabel.setStyleSheet("font-size: 9pt;")
    layout.addWidget(descriptionLabel)

    inputField = QSpinBox()
    inputField.setRange(range[0], range[1])
    inputField.setButtonSymbols(QSpinBox.NoButtons)
    inputField.setMaximumWidth(int(self.parent.width() * 0.05))
    inputField.setStyleSheet("font-size: 10pt;")
    layout.addWidget(inputField)

    groupBox.setLayout(layout)
    top_layout.addRow(groupBox)

    return inputField

  # ---------------------------------------------------------------------------------------------------------------------------
  def SetConfig(self, config: Config) -> None:
    self.currConfig = config

    self.autoplaySpeedInput.setValue(self.currConfig.autoplay_speed_millis)
    self.clockPeriodInput.setValue(self.currConfig.clock_period_millis)
    self.icCyclesPerOpInput.setValue(self.currConfig.ic_cycles_per_op)
    self.icCyclesPerOpWithCacheHitInput.setValue(self.currConfig.ic_cycles_per_op_with_cache_hit)
    self.lsCyclesPerOpInput.setValue(self.currConfig.ls_cycles_per_op)
    self.lsCyclesPerOpWithCacheHitInput.setValue(self.currConfig.ls_cycles_per_op_with_cache_hit)
    self.deCyclesPerOpInput.setValue(self.currConfig.de_cycles_per_op)
    self.exCyclesPerOpInput.setValue(self.currConfig.ex_cycles_per_op)

  # ---------------------------------------------------------------------------------------------------------------------------
  def Save(self) -> None:
    self.currConfig.autoplay_speed_millis           = self.autoplaySpeedInput.value()
    self.currConfig.clock_period_millis             = self.clockPeriodInput.value()
    self.currConfig.ic_cycles_per_op                = self.icCyclesPerOpInput.value()
    self.currConfig.ic_cycles_per_op_with_cache_hit = self.icCyclesPerOpWithCacheHitInput.value()
    self.currConfig.ls_cycles_per_op                = self.lsCyclesPerOpInput.value()
    self.currConfig.ls_cycles_per_op_with_cache_hit = self.lsCyclesPerOpWithCacheHitInput.value()
    self.currConfig.de_cycles_per_op                = self.deCyclesPerOpInput.value()
    self.currConfig.ex_cycles_per_op                = self.exCyclesPerOpInput.value()

    self.parent.simulationTab.autoPlayTimer.setInterval(self.currConfig.autoplay_speed_millis)


# -----------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
  raise Exception("This module is not meant to be run directly.")
