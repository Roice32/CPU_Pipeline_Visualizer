from PyQt5.QtWidgets import QWidget, QFormLayout, QSpinBox, QPushButton
from PyQt5.QtWidgets import QHBoxLayout
from PyQt5.QtWidgets import QGroupBox, QVBoxLayout, QLabel


# -----------------------------------------------------------------------------------------------------------------------------
class Config:
  clock_period_millis: int

  # ---------------------------------------------------------------------------------------------------------------------------
  def __init__(self):
    self.clock_period_millis = 1


# -----------------------------------------------------------------------------------------------------------------------------
class ConfigTab(QWidget):
  buttonStyle = "border: 1px solid black; border-radius: 5px; height: 30%; width: 50%; background-color: #f0f0f0; font: bold;"

  parent = None
  currConfig = None
  prevConfig = None

  clockPeriodInput = None

  # ---------------------------------------------------------------------------------------------------------------------------
  def __init__(self, parent):
    super().__init__()
    self.parent = parent
    layout = QFormLayout()

    # Set font for the widget and its children
    font = self.font()
    font.setPointSize(10)
    font.setBold(True)
    self.setFont(font)

    self.currConfig = Config()
    self.prevConfig = Config()

    # CLOCK_PERIOD_MILLIS
    self.clockPeriodInput = self.GenConfigElement(layout,
                                                  "Clock Period",
                                                  (1, 100),
                                                  "Sets the CPU's clock to tick every X milliseconds.")

    # Buttons layout
    buttonsLayout = QHBoxLayout()
    buttonsLayout.addStretch(1)  # Add stretchable space before buttons

    self.saveButton = QPushButton("Save")
    self.saveButton.clicked.connect(self.Save)
    self.saveButton.setStyleSheet(ConfigTab.buttonStyle)
    buttonsLayout.addWidget(self.saveButton)

    self.resetButton = QPushButton("Reset")
    self.resetButton.clicked.connect(lambda: self.SetConfig(self.prevConfig))
    self.resetButton.setStyleSheet(ConfigTab.buttonStyle)
    buttonsLayout.addWidget(self.resetButton)

    buttonsLayout.addStretch(1)  # Add stretchable space after buttons

    layout.addRow(buttonsLayout)

    self.SetConfig(self.currConfig)
    self.setLayout(layout)

  # ---------------------------------------------------------------------------------------------------------------------------
  def GenConfigElement(self, top_layout, title, range, description):
    groupBox = QGroupBox(title)
    layout = QVBoxLayout()

    descriptionLabel = QLabel(description)
    descriptionLabel.setWordWrap(True)
    descriptionLabel.setStyleSheet("font-size: 9pt;")
    layout.addWidget(descriptionLabel)

    inputField = QSpinBox()
    inputField.setRange(range[0], range[1])
    inputField.setButtonSymbols(QSpinBox.NoButtons)
    inputField.setMaximumWidth(int(self.parent.width() * 0.1))
    inputField.setStyleSheet("font-size: 10pt;")
    layout.addWidget(inputField)

    groupBox.setLayout(layout)
    top_layout.addRow(groupBox)

    return inputField

  # ---------------------------------------------------------------------------------------------------------------------------
  def SetConfig(self, config):
    self.currConfig = config
    self.clockPeriodInput.setValue(self.currConfig.clock_period_millis)

  # ---------------------------------------------------------------------------------------------------------------------------
  def Save(self):
    self.currConfig.clock_period_millis = self.clockPeriodInput.value()


# -----------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
  raise Exception("This module is not meant to be run directly.")
