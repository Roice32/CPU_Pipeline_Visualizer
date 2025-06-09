from PyQt5.QtWidgets import QWidget, QFormLayout, QSpinBox, QPushButton
from PyQt5.QtWidgets import QHBoxLayout, QGridLayout
from PyQt5.QtWidgets import QGroupBox, QVBoxLayout, QLabel
from PyQt5.QtWidgets import QScrollArea, QWidget
from PyQt5.QtWidgets import QCheckBox


# -----------------------------------------------------------------------------------------------------------------------------
class Config:
  autoplay_speed_millis:           int

  clock_period_millis:             int
  garbage_memory:                  bool
  ls_cycles_per_op:                int
  ls_cycles_per_op_with_cache_hit: int
  ic_cycles_per_op:                int
  ic_cycles_per_op_with_cache_hit: int
  de_cycles_per_op:                int
  ex_cycles_per_op:                int

  # ---------------------------------------------------------------------------------------------------------------------------
  def __init__(self) -> None:
    self.autoplay_speed_millis           = 200
    self.clock_period_millis             = 1
    self.garbage_memory                  = False
    self.ls_cycles_per_op                = 15
    self.ls_cycles_per_op_with_cache_hit = 7
    self.ic_cycles_per_op                = 3
    self.ic_cycles_per_op_with_cache_hit = 2
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

  parent     = None
  currConfig = None

  autoPlaySpeedInput             = None
  clockPeriodInput               = None
  garbageMemory                  = None
  icCyclesPerOpInput             = None
  icCyclesPerOpWithCacheHitInput = None
  lsCyclesPerOpInput             = None
  lsCyclesPerOpWithCacheHitInput = None
  deCyclesPerOpInput             = None
  exCyclesPerOpInput             = None

  prevSimHadGarbageMemory        = False

  saveButton                     = None
  resetButton                    = None

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
    autoPlaySpeedGridElem, self.autoPlaySpeedInput = self.GenNumericConfigElement(
      "Autoplay Speed",
      (10, 10_000),
      "Sets the speed of the autoplay in Simulation tab, in milliseconds."
      + "\nCan be changed at any time, without affecting the current simulation."
    )
    scrollLayout.addRow(autoPlaySpeedGridElem)

    # Add a horizontal line
    line = QLabel()
    line.setFrameShape(QLabel.HLine)
    line.setFrameShadow(QLabel.Sunken)
    scrollLayout.addRow(line)

    gridWidget = QWidget()
    gridLayout = QGridLayout()
    gridLayout.setContentsMargins(0, 0, 0, 0)
    gridLayout.setSpacing(10)

    # Add a solid 1-pixel black border to the cells of the grid
    gridWidget.setStyleSheet("""
      QGroupBox {
        border: 1px solid black;
        border-radius: 4px;
        margin-top: 4px;
        padding: 2px;
        font-weight: bold;
        font-size: 12pt;
      }
    """)

    # GARBAGE_MEMORY
    garbageMemoryGridElem, self.garbageMemory = self.GenBooleanConfigElement(
      "Garbage Memory",
      "If enabled, when fetching data from a memory location not written to, the value will be random,"
      + " as opposed to 0x00."
    )

    # CLOCK_PERIOD_MILLIS
    clockPeriodGridElem, self.clockPeriodInput = self.GenNumericConfigElement(
      "Clock Period",
      (1, 100),
      "Sets the CPU's clock to tick every X milliseconds."
    )

    # LS_CYCLES_PER_OP
    lsCyclesPerOpGridElem, self.lsCyclesPerOpInput = self.GenNumericConfigElement(
      "LS Cycles per Op",
      (1, 100),
      "The number of cycles the Load Store unit takes to execute an operation"
      + " (deliver a fetch window to DE; deliver data to EX; store data from EX)."
    )

    # LS_CYCLES_PER_OP_WITH_CACHE_HIT
    lsCyclesPerOpWithCacheHitGridElem, self.lsCyclesPerOpWithCacheHitInput = self.GenNumericConfigElement(
      "LS Cycles per Op with Cache Hit",
      (1, 100),
      "The number of cycles the Load Store unit takes to execute an"
      + " operation when the data is in the cache & valid (no access to physical memory)."
    )

    # IC_CYCLES_PER_OP
    icCyclesPerOpGridElem, self.icCyclesPerOpInput = self.GenNumericConfigElement(
      "IC Cycles per Op",
      (1, 100),
      "The number of cycles the Instruction Cache takes to execute an operation"
      + " (deliver a fetch window to DE)."
      + "\nDoes not include cycles when awaiting data from LS."
    )

    # IC_CYCLES_PER_OP_WITH_CACHE_HIT
    icCyclesPerOpWithCacheHitGridElem, self.icCyclesPerOpWithCacheHitInput = self.GenNumericConfigElement(
      "IC Cycles per Op with Cache Hit",
      (1, 100),
      "The number of cycles the Instruction Cache takes to execute an"
      + " operation when the data is in the cache & valid (no request to LS)."
    )

    # DE_CYCLES_PER_OP
    deCyclesPerOpGridElem, self.deCyclesPerOpInput = self.GenNumericConfigElement(
      "DE Cycles per Op",
      (1, 100),
      "The number of cycles the Decode unit takes to execute an operation"
      + " (decode an instruction & send it to EX)."
      + "\nDoes not include cycles when awaiting a fetch window from IC."
    )

    # EX_CYCLES_PER_OP
    exCyclesPerOpGridElem, self.exCyclesPerOpInput = self.GenNumericConfigElement(
      "EX Cycles per Op",
      (1, 100),
      "The number of cycles the Execute unit takes to execute an operation"
      + " (execute an instruction)."
      + "\nDoes not include cycles when awaiting data from LS."
    )

    for ind, elem in enumerate([clockPeriodGridElem, garbageMemoryGridElem,
                                lsCyclesPerOpGridElem, lsCyclesPerOpWithCacheHitGridElem,
                                icCyclesPerOpGridElem, icCyclesPerOpWithCacheHitGridElem,
                                deCyclesPerOpGridElem, exCyclesPerOpGridElem]):
      gridLayout.addWidget(elem, ind // 2, ind % 2)

    gridWidget.setLayout(gridLayout)
    scrollLayout.addWidget(gridWidget)

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
  def GenNumericConfigElement(self, title: str, range: tuple[int, int], description: str) -> tuple[QGroupBox, QSpinBox]:
    groupBox = QGroupBox(title)
    groupBox.setFlat(True)
    groupBox.setContentsMargins(0, 0, 0, 0)
    layout = QVBoxLayout()
    layout.setContentsMargins(0, 0, 0, 0)
    layout.setSpacing(1)

    rangeLabel = QLabel(f"[{range[0]} - {range[1]}]")
    rangeLabel.setWordWrap(True)
    rangeLabel.setStyleSheet("font-size: 9pt;")
    rangeLabel.setContentsMargins(0, 0, 0, 0)
    layout.addWidget(rangeLabel)

    descriptionLabel = QLabel(f"{description}")
    descriptionLabel.setWordWrap(True)
    descriptionLabel.setStyleSheet("font-size: 10pt;")
    descriptionLabel.setContentsMargins(0, 0, 0, 0)
    layout.addWidget(descriptionLabel)

    inputField = QSpinBox()
    inputField.setRange(range[0], range[1])
    inputField.setButtonSymbols(QSpinBox.NoButtons)
    inputField.setMaximumWidth(int(self.parent.width() * 0.05 * 1.2))
    inputField.setStyleSheet("font-size: 11pt;")
    inputField.setContentsMargins(0, 0, 0, 0)
    layout.addWidget(inputField)

    groupBox.setLayout(layout)

    return groupBox, inputField

  # ---------------------------------------------------------------------------------------------------------------------------
  def GenBooleanConfigElement(self, title: str, description: str) -> tuple[QGroupBox, QPushButton]:
    groupBox = QGroupBox(title)
    groupBox.setFlat(True)
    groupBox.setContentsMargins(0, 0, 0, 0)
    layout = QVBoxLayout()
    layout.setContentsMargins(0, 0, 0, 0)
    layout.setSpacing(1)

    descriptionLabel = QLabel(f"{description}")
    descriptionLabel.setWordWrap(True)
    descriptionLabel.setStyleSheet("font-size: 10pt;")
    descriptionLabel.setContentsMargins(0, 0, 0, 0)
    layout.addWidget(descriptionLabel)

    inputField = QCheckBox()
    inputField.setStyleSheet("QCheckBox::indicator { width: 15%; height: 15%; margin: 5px}")

    layout.addWidget(inputField)

    groupBox.setLayout(layout)

    return groupBox, inputField

  # ---------------------------------------------------------------------------------------------------------------------------
  def SetConfig(self, config: Config) -> None:
    self.currConfig = config

    self.autoPlaySpeedInput.setValue(self.currConfig.autoplay_speed_millis)
    self.clockPeriodInput.setValue(self.currConfig.clock_period_millis)
    self.garbageMemory.setChecked(self.currConfig.garbage_memory)
    self.lsCyclesPerOpInput.setValue(self.currConfig.ls_cycles_per_op)
    self.lsCyclesPerOpWithCacheHitInput.setValue(self.currConfig.ls_cycles_per_op_with_cache_hit)
    self.icCyclesPerOpInput.setValue(self.currConfig.ic_cycles_per_op)
    self.icCyclesPerOpWithCacheHitInput.setValue(self.currConfig.ic_cycles_per_op_with_cache_hit)
    self.deCyclesPerOpInput.setValue(self.currConfig.de_cycles_per_op)
    self.exCyclesPerOpInput.setValue(self.currConfig.ex_cycles_per_op)

  # ---------------------------------------------------------------------------------------------------------------------------
  def Save(self) -> None:
    self.currConfig.autoplay_speed_millis           = self.autoPlaySpeedInput.value()
    self.currConfig.clock_period_millis             = self.clockPeriodInput.value()
    self.currConfig.garbage_memory                  = self.garbageMemory.isChecked()
    self.currConfig.ls_cycles_per_op                = self.lsCyclesPerOpInput.value()
    self.currConfig.ls_cycles_per_op_with_cache_hit = self.lsCyclesPerOpWithCacheHitInput.value()
    self.currConfig.ic_cycles_per_op                = self.icCyclesPerOpInput.value()
    self.currConfig.ic_cycles_per_op_with_cache_hit = self.icCyclesPerOpWithCacheHitInput.value()
    self.currConfig.de_cycles_per_op                = self.deCyclesPerOpInput.value()
    self.currConfig.ex_cycles_per_op                = self.exCyclesPerOpInput.value()

    self.parent.simulationTab.autoPlayTimer.setInterval(self.currConfig.autoplay_speed_millis)


# -----------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
  raise Exception("This module is not meant to be run directly.")
