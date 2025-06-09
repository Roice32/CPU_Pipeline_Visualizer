import os
from time import sleep
from PyQt5.QtWidgets import (
  QWidget, QVBoxLayout, QHBoxLayout, QLabel, QTextEdit, QPushButton, QFileDialog, QSplitter, QMessageBox
)
from PyQt5.QtCore import Qt, QProcess, QRegExp
from PyQt5.QtGui import QFont, QTextCharFormat, QColor, QSyntaxHighlighter


# -----------------------------------------------------------------------------------------------------------------------------
class SyntaxHighlighter(QSyntaxHighlighter):
  highlightingRules = []

  # ---------------------------------------------------------------------------------------------------------------------------
  def __init__(self, parent=None):
    super(SyntaxHighlighter, self).__init__(parent)
    
    # Define highlighting rules
    self.highlightingRules = []
    
    # Segment format (lines starting with .)
    segmentFormat = QTextCharFormat()
    segmentFormat.setForeground(QColor(255, 0, 0))  # Red
    self.highlightingRules.append((QRegExp("^\\.[^\n]*"), segmentFormat))

    # Instruction format (first word of line)
    instructionFormat = QTextCharFormat()
    instructionFormat.setForeground(QColor(0, 0, 255))  # Blue
    self.highlightingRules.append((QRegExp("^\\s*\\w+"), instructionFormat))

    # Declaration format (lines starting with dw or dblock)
    declarationFormat = QTextCharFormat()
    declarationFormat.setForeground(QColor(255, 165, 0))  # Orange
    self.highlightingRules.append((QRegExp("^(dw|dblock)\\s+"), declarationFormat))

    # Label format (word followed by colon)
    labelFormat = QTextCharFormat()
    labelFormat.setForeground(QColor(128, 0, 128))  # Purple
    labelFormat.setFontItalic(True)
    self.highlightingRules.append((QRegExp("^\\w+:"), labelFormat))

    # Comment format (semicolon and everything after)
    commentFormat = QTextCharFormat()
    commentFormat.setForeground(QColor(0, 100, 0))  # Dark Green
    self.highlightingRules.append((QRegExp(";.*$"), commentFormat))

    # Address format (in .hex textbox)
    addressFormat = QTextCharFormat()
    addressFormat.setForeground(QColor(128, 0, 128)) # Purple
    self.highlightingRules.append((QRegExp("^#[0-9a-f]{4}$"), addressFormat))

  # ---------------------------------------------------------------------------------------------------------------------------
  def highlightBlock(self, text):
    for pattern, format in self.highlightingRules:
      expression = QRegExp(pattern)
      index = expression.indexIn(text)
      while index >= 0:
        length = expression.matchedLength()
        self.setFormat(index, length, format)
        index = expression.indexIn(text, index + length)


# -----------------------------------------------------------------------------------------------------------------------------
class InputTab(QWidget):
  buttonStyleSheet = """
    QPushButton {
        border: 1px solid black;
        border-radius: 5px;
        height: 30%;
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
  statusStyle = "font-weight: bold; color: #333; margin: 5px; padding: 5px; border-radius: 4px; background-color: #f8f8f8;"
  parent = None
  asmText = None
  hexText = None
  syntaxHighlighter = None
  statusLabel = None

  # ---------------------------------------------------------------------------------------------------------------------------
  def __init__(self, parent):
    super().__init__()
    self.parent = parent

    # Main layout
    mainLayout = QVBoxLayout()
    
    # Status label (centered at the top)
    self.statusLabel = QLabel()
    self.statusLabel.setWordWrap(True)
    self.statusLabel.setAlignment(Qt.AlignCenter)
    self.statusLabel.setStyleSheet(InputTab.statusStyle)
    self.statusLabel.setVisible(False)  # Hidden by default

    # Set height to at most 10% of screen height
    screenHeight = self.parent.screen().size().height()
    self.statusLabel.setMaximumHeight(int(screenHeight * 0.05))

    mainLayout.addWidget(self.statusLabel)

    # Create splitter for resizing
    splitter = QSplitter(Qt.Horizontal)

    # Left side - ASM input
    leftWidget = QWidget()
    leftLayout = QVBoxLayout()
    leftLayout.addWidget(QLabel("input.asm"))

    self.asmText = QTextEdit()
    font = QFont("Courier New", 10)
    self.asmText.setFont(font)
    leftLayout.addWidget(self.asmText)
    
    # Apply syntax highlighter to ASM text
    self.syntaxHighlighter = SyntaxHighlighter(self.asmText.document())

    # ASM Buttons layout
    asmBtnLayout = QHBoxLayout()
    
    openAsmBtn = QPushButton("Open")
    openAsmBtn.clicked.connect(self.OpenAsmFile)
    openAsmBtn.setStyleSheet(InputTab.buttonStyleSheet)
    
    saveAsmBtn = QPushButton("Save")
    saveAsmBtn.clicked.connect(self.SaveAsmFile)
    saveAsmBtn.setStyleSheet(InputTab.buttonStyleSheet)
    
    asmBtnLayout.addWidget(openAsmBtn)
    asmBtnLayout.addWidget(saveAsmBtn)
    leftLayout.addLayout(asmBtnLayout)

    leftWidget.setLayout(leftLayout)
    splitter.addWidget(leftWidget)

    # Middle buttons
    middleWidget = QWidget()
    middleLayout = QVBoxLayout()
    middleLayout.addStretch()

    # Center-align the buttons
    buttonLayout = QHBoxLayout()
    buttonLayout.setAlignment(Qt.AlignCenter)
    
    convertBtn = QPushButton("→")
    convertBtn.setFixedWidth(50)
    convertBtn.setFixedHeight(30)
    convertBtn.setStyleSheet(InputTab.buttonStyleSheet)
    convertBtn.clicked.connect(self.ConvertAsmToHex)
    buttonLayout.addWidget(convertBtn)
    
    middleLayout.addLayout(buttonLayout)

    executeBtn = QPushButton("Execute")
    executeBtn.setFixedWidth(80)
    executeBtn.setFixedHeight(30)
    executeBtn.setStyleSheet(InputTab.buttonStyleSheet)
    executeBtn.clicked.connect(self.ExecuteSimulation)
    middleLayout.addWidget(executeBtn, 0, Qt.AlignCenter)

    middleLayout.addStretch()
    middleWidget.setLayout(middleLayout)
    splitter.addWidget(middleWidget)

    # Right side - HEX input
    rightWidget = QWidget()
    rightLayout = QVBoxLayout()
    rightLayout.addWidget(QLabel("input.hex"))

    self.hexText = QTextEdit()
    self.hexText.setFont(font)
    rightLayout.addWidget(self.hexText)

    # HEX Buttons layout
    hexBtnLayout = QHBoxLayout()
    
    openHexBtn = QPushButton("Open")
    openHexBtn.clicked.connect(self.OpenHexFile)
    openHexBtn.setStyleSheet(InputTab.buttonStyleSheet)
    
    saveHexBtn = QPushButton("Save")
    saveHexBtn.clicked.connect(self.SaveHexFile)
    saveHexBtn.setStyleSheet(InputTab.buttonStyleSheet)
    
    hexBtnLayout.addWidget(openHexBtn)
    hexBtnLayout.addWidget(saveHexBtn)
    rightLayout.addLayout(hexBtnLayout)

    rightWidget.setLayout(rightLayout)
    splitter.addWidget(rightWidget)

    # Set stretch factors for splitter
    splitter.setStretchFactor(0, 1)
    splitter.setStretchFactor(1, 0)
    splitter.setStretchFactor(2, 1)

    mainLayout.addWidget(splitter)
    self.setLayout(mainLayout)

    with open("./dependencies/example.asm", 'r') as defaultAsm:
      self.asmText.setText(defaultAsm.read())

  # ---------------------------------------------------------------------------------------------------------------------------
  def SetStatusText(self, text, error=False):
    """Set the status text and make it visible with appropriate styling."""
    if error:
      self.statusLabel.setStyleSheet(InputTab.statusStyle + "color: #d32f2f; background-color: #ffebee;")
    else:
      self.statusLabel.setStyleSheet(InputTab.statusStyle + "color: #1b5e20; background-color: #e8f5e9;")
    
    self.statusLabel.setText(text)
    self.statusLabel.setVisible(True)
    self.statusLabel.repaint()  # Force a redraw of the status label

  # ---------------------------------------------------------------------------------------------------------------------------
  def ClearStatusText(self):
    """Clear the status text and hide the label."""
    self.statusLabel.setText("")
    self.statusLabel.setVisible(False)

  # ---------------------------------------------------------------------------------------------------------------------------
  def OpenAsmFile(self):
    options = QFileDialog.Options()
    filename, _ = QFileDialog.getOpenFileName(self, "Open ASM File", "", "Assembly Files (*.asm);;All Files (*)", options=options)
    if filename:
      with open(filename, 'r') as file:
        self.asmText.setText(file.read())
  
  # ---------------------------------------------------------------------------------------------------------------------------
  def SaveAsmFile(self):
    options = QFileDialog.Options()
    filename, _ = QFileDialog.getSaveFileName(self, "Save ASM File", "", "Assembly Files (*.asm);;All Files (*)", options=options)
    if filename:
      # Add .asm extension if not present
      if not filename.lower().endswith('.asm'):
        filename += '.asm'
      
      with open(filename, 'w') as file:
        file.write(self.asmText.toPlainText())
        QMessageBox.information(self, "Success", f"File saved successfully: {filename}")

  # ---------------------------------------------------------------------------------------------------------------------------
  def OpenHexFile(self):
    options = QFileDialog.Options()
    filename, _ = QFileDialog.getOpenFileName(self, "Open HEX File", "", "Hex Files (*.hex);;All Files (*)", options=options)
    if filename:
      with open(filename, 'r') as file:
        self.hexText.setText(file.read())
  
  # ---------------------------------------------------------------------------------------------------------------------------
  def SaveHexFile(self):
    options = QFileDialog.Options()
    filename, _ = QFileDialog.getSaveFileName(self, "Save HEX File", "", "Hex Files (*.hex);;All Files (*)", options=options)
    if filename:
      # Add .hex extension if not present
      if not filename.lower().endswith('.hex'):
        filename += '.hex'
      
      with open(filename, 'w') as file:
        file.write(self.hexText.toPlainText())
        QMessageBox.information(self, "Success", f"File saved successfully: {filename}")

  # ---------------------------------------------------------------------------------------------------------------------------
  def ConvertAsmToHex(self):
    # Save ASM to temp file
    self.SetStatusText("Converting .asm to .hex...", error=False)

    tempDir = self.parent.GetTempDir()
    asmPath = os.path.join(tempDir, "input.asm")
    with open(asmPath, 'w') as file:
      file.write(self.asmText.toPlainText())

    hexPath = os.path.join(tempDir, "input.hex")
    parseScript = os.path.join(".", "dependencies", "pyasm.py")
    configFile = os.path.join(".", "dependencies", "asm_cfg.yml")

    try:
      # Run parsing script
      process = QProcess()
      process.start("python.exe", [parseScript, "--config-file", configFile, "--in-file", asmPath, "--out-file", hexPath])
      process.waitForFinished(-1)

      processOutput = process.readAllStandardOutput().data().decode()
      if processOutput != "":
        raise Exception(processOutput)

      # Load the generated hex file
      if os.path.exists(hexPath):
        with open(hexPath, 'r') as file:
          self.hexText.setText(file.read())
        self.SetStatusText(".asm to .hex conversion successful.", error=False)

    except Exception as e:
      self.SetStatusText(f"Error converting .asm to .hex", error=True)
      QMessageBox.critical(self, "Error", str(e))

  # ---------------------------------------------------------------------------------------------------------------------------
  def ExecuteSimulation(self):
    # Save HEX to temp file
    self.SetStatusText("Saving hex source file...", error=False)
    hexPath = os.path.join(self.parent.GetTempDir(), "input.hex")
    with open(hexPath, 'w') as file:
      file.write(self.hexText.toPlainText())

    simulationPath = self.parent.GetSimulationDir()
    cpuStatesPath = self.parent.GetSimulationCpuStatesDir()
    memoryPath = self.parent.GetSimulationMemoryDir()

    self.parent.SetSimulationTabEnabled(False)

    # Delete all files in cpuStatesPath and memoryPath directories
    for path in [cpuStatesPath, memoryPath]:
      if os.path.exists(path):
        for file in os.listdir(path):
          file_path = os.path.join(path, file)
          if os.path.isfile(file_path):
            os.remove(file_path)

    self.parent.configTab.Save()
    self.SetStatusText("Executing simulation...", error=False)
    process = QProcess(self)
    process.finished.connect(lambda _: self.PostExecuteSimulation(process))
    process.start("dependencies/CPU_Pipeline_Simulator.exe", [hexPath, simulationPath] + self.GenerateConfigOverloadParams())

  # ---------------------------------------------------------------------------------------------------------------------------
  def PostExecuteSimulation(self, process: QProcess):
    # Check if the execution was successful
    if process.exitCode() != 0:
      processError = process.readAllStandardError().data().decode()
      self.SetStatusText(f"Simulation failed", error=True)
      QMessageBox.critical(self, "Error", f"{processError}")
      return

    processOutput = process.readAllStandardOutput().data().decode()
    print(processOutput)

    self.SetStatusText("Processing simulation results...", error=False)
    self.parent.simulationTab.LoadSimulationData()

    # Switch to simulation tab
    self.SetStatusText("Simulation complete.", error=False)
    self.parent.SetSimulationTabEnabled(True)
    self.parent.SwitchToSimulationTab()

  # ---------------------------------------------------------------------------------------------------------------------------
  def GenerateConfigOverloadParams(self):
    config = self.parent.GetConfig()
    params = []
    for arg in vars(config):
      params.append(f"--{arg.replace('_', '-')}")
      params.append(str(getattr(config, arg)))
    return params

  # ---------------------------------------------------------------------------------------------------------------------------
  def GetAsmText(self):
    return self.asmText.toPlainText()

  # ---------------------------------------------------------------------------------------------------------------------------
  def GetHexText(self):
    return self.hexText.toPlainText()

  # ---------------------------------------------------------------------------------------------------------------------------
  def SetAsmText(self, text):
    self.asmText.setText(text)

  # ---------------------------------------------------------------------------------------------------------------------------
  def SetHexText(self, text):
    self.hexText.setText(text)


# -----------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
  raise Exception("This module is not meant to be run directly.")
