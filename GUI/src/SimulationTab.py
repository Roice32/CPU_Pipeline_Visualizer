import os
import json
from PyQt5.QtWidgets import (
  QWidget, QVBoxLayout, QHBoxLayout, QLabel, QSlider,
  QGraphicsScene, QGraphicsView, QGraphicsRectItem, QMessageBox,
  QPushButton, QTableWidget, QTableWidgetItem, QHeaderView, QSplitter,
  QSpacerItem, QSizePolicy, QScrollArea
)
from PyQt5.QtCore import Qt, QEvent, QTimer
from PyQt5.QtGui import QPainter, QIcon, QColor, QFont, QKeySequence
from PyQt5.QtWidgets import QShortcut
from DiagramComponents import CreateComponent, ComponentColors


# -----------------------------------------------------------------------------------------------------------------------------
class SimulationTab(QWidget):
  parent = None
  totalCycles = None
  currentCycleIndex = None
  selectedComponent = None
  componentItems = None
  cycleSlider = None
  cycleLabel = None
  diagramScene = None
  diagramView = None
  detailsLayout = None
  playPauseButton = None
  autoPlayTimer = None
  isPlaying = False
  memoryMapping = None
  currentHoveredComponent = None
  currentState = None
  previousState = None
  currentMemory = None
  previousMemory = None

  # ---------------------------------------------------------------------------------------------------------------------------
  def __init__(self, parent):
    super().__init__()
    self.parent = parent
    self.totalCycles = 0
    self.currentCycleIndex = 1
    self.selectedComponent = None
    self.componentItems = {}
    self.memoryMapping = {}
    self.currentHoveredComponent = None

    layout = QVBoxLayout()

    # Slider and control buttons layout
    sliderLayout = QVBoxLayout()
    sliderLayout.setContentsMargins(0, 0, 0, 0)

    # Slider
    sliderBarLayout = QHBoxLayout()
    sliderBarLayout.addWidget(QLabel("Cycle:"))
    self.cycleSlider = QSlider(Qt.Horizontal)
    self.cycleSlider.setMinimum(1)
    self.cycleSlider.setMaximum(1)
    self.cycleSlider.valueChanged.connect(self.UpdateSimulationView)
    sliderBarLayout.addWidget(self.cycleSlider)
    self.cycleLabel = QLabel("1")
    sliderBarLayout.addWidget(self.cycleLabel)
    sliderLayout.addLayout(sliderBarLayout)

    # Control buttons
    buttonLayout = QHBoxLayout()
    buttonLayout.setAlignment(Qt.AlignCenter)

    # Button icons
    toStartIcon = QIcon("resources/icons/toStart.png")
    previousIcon = QIcon("resources/icons/previous.png")
    playIcon = QIcon("resources/icons/play.png")
    pauseIcon = QIcon("resources/icons/pause.png")
    nextIcon = QIcon("resources/icons/next.png")
    toEndIcon = QIcon("resources/icons/toEnd.png")

    # Create buttons
    toStartButton = QPushButton(toStartIcon, "")
    self.previousButton = QPushButton(previousIcon, "")
    self.playPauseButton = QPushButton(playIcon, "")
    self.nextButton = QPushButton(nextIcon, "")
    toEndButton = QPushButton(toEndIcon, "")

    # Connect buttons to functions
    toStartButton.clicked.connect(self.GoToFirstCycle)
    self.previousButton.clicked.connect(self.DecreaseCycle)
    self.playPauseButton.clicked.connect(self.TogglePlayPause)
    self.nextButton.clicked.connect(self.IncreaseCycle)
    toEndButton.clicked.connect(self.GoToLastCycle)

    # Add buttons to layout
    buttonLayout.addWidget(toStartButton)
    buttonLayout.addWidget(self.previousButton)
    buttonLayout.addWidget(self.playPauseButton)
    buttonLayout.addWidget(self.nextButton)
    buttonLayout.addWidget(toEndButton)

    sliderLayout.addLayout(buttonLayout)
    layout.addLayout(sliderLayout)

    # Create splitter for diagram and details
    splitter = QSplitter(Qt.Horizontal)
    splitter.setContentsMargins(0, 10, 0, 0)

    # CPU diagram view
    diagramWidget = QWidget()
    diagramLayout = QVBoxLayout(diagramWidget)
    label = QLabel("CPU Diagram:")
    label.setAlignment(Qt.AlignCenter)
    font = label.font()
    font.setBold(True)
    label.setFont(font)
    diagramLayout.addWidget(label)
    self.diagramScene = QGraphicsScene()
    self.diagramView = QGraphicsView(self.diagramScene)
    self.diagramView.setStyleSheet("border: 3px solid black; border-radius: 8px; background-color: #f0f0f0;")
    self.diagramView.setRenderHint(QPainter.Antialiasing)
    self.diagramView.setMinimumHeight(300)
    self.diagramView.setMinimumWidth(300)
    self.diagramView.viewport().installEventFilter(self)
    self.diagramView.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
    self.diagramView.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
    diagramLayout.addWidget(self.diagramView, 1)
    splitter.addWidget(diagramWidget)

    # Details widget
    detailsContainerWidget = QWidget()
    detailsContainerLayout = QVBoxLayout(detailsContainerWidget)
    detailsLabel = QLabel("Component Details:")
    detailsLabel.setAlignment(Qt.AlignCenter)
    font = detailsLabel.font()
    font.setBold(True)
    detailsLabel.setFont(font)
    detailsContainerLayout.addWidget(detailsLabel)

    # Create a scroll area for the details content
    self.detailsScrollArea = QScrollArea()
    self.detailsScrollArea.setWidgetResizable(True)
    self.detailsScrollArea.setStyleSheet("QScrollArea { border: 3px solid black; border-radius: 8px; background-color: #f0f0f0; }")
    
    detailsContentWidget = QWidget()
    self.detailsLayout = QVBoxLayout(detailsContentWidget)
    self.detailsLayout.setContentsMargins(10, 10, 10, 10)
    self.detailsLayout.addStretch(1)

    self.detailsScrollArea.setWidget(detailsContentWidget)
    detailsContainerLayout.addWidget(self.detailsScrollArea)
    
    splitter.addWidget(detailsContainerWidget)

    # Set splitter proportions (1:1)
    splitter.setSizes([400, 400])
    layout.addWidget(splitter, 1)

    # Create the CPU diagram components
    self.CreateCpuDiagram()

    self.setLayout(layout)

    # Timer for auto-play
    self.autoPlayTimer = QTimer(self)
    self.autoPlayTimer.timeout.connect(self.IncreaseCycle)
    self.autoPlayTimer.setInterval(1000)

    # Setup keyboard shortcuts
    self.SetupKeyboardShortcuts()

  # ---------------------------------------------------------------------------------------------------------------------------
  def SetupKeyboardShortcuts(self):
    # Left arrow for previous cycle
    leftShortcut = QShortcut(QKeySequence(Qt.Key_Left), self)
    leftShortcut.activated.connect(self.DecreaseCycle)
    
    # Right arrow for next cycle
    rightShortcut = QShortcut(QKeySequence(Qt.Key_Right), self)
    rightShortcut.activated.connect(self.IncreaseCycle)
    
    # Space bar for play/pause toggle
    spaceShortcut = QShortcut(QKeySequence(Qt.Key_Space), self)
    spaceShortcut.activated.connect(self.TogglePlayPause)

  # ---------------------------------------------------------------------------------------------------------------------------
  def ResizeEvent(self, event):
    self.diagramView.fitInView(self.diagramScene.sceneRect(), Qt.AspectRatioMode.KeepAspectRatio)
    marginH = int(self.width() * 0.2)
    self.layout().itemAt(0).setContentsMargins(marginH, 0, marginH, 0)
    super().resizeEvent(event)

  # ---------------------------------------------------------------------------------------------------------------------------
  def GoToFirstCycle(self):
    self.cycleSlider.setValue(self.cycleSlider.minimum())

  # ---------------------------------------------------------------------------------------------------------------------------
  def DecreaseCycle(self):
    if self.cycleSlider.value() > self.cycleSlider.minimum():
      newCycle = self.cycleSlider.value() - 1
      self.cycleSlider.setValue(newCycle)
      self.currentCycleIndex = newCycle

  # ---------------------------------------------------------------------------------------------------------------------------
  def TogglePlayPause(self):
    if self.isPlaying:
      self.autoPlayTimer.stop()
      self.playPauseButton.setIcon(QIcon("resources/icons/play.png"))
    else:
      self.autoPlayTimer.start()
      self.playPauseButton.setIcon(QIcon("resources/icons/pause.png"))
    self.isPlaying = not self.isPlaying

  # ---------------------------------------------------------------------------------------------------------------------------
  def IncreaseCycle(self):
    if self.cycleSlider.value() < self.cycleSlider.maximum():
      newCycle = self.cycleSlider.value() + 1
      self.cycleSlider.setValue(newCycle)
      self.currentCycleIndex = newCycle
    else:
      if self.isPlaying:
        self.TogglePlayPause()

  # ---------------------------------------------------------------------------------------------------------------------------
  def GoToLastCycle(self):
    self.cycleSlider.setValue(self.cycleSlider.maximum())

  # ---------------------------------------------------------------------------------------------------------------------------
  def CreateCpuDiagram(self):
    self.diagramScene.clear()
    self.componentItems = {}

    MODULE_SIDE = 120
    PIPE_WIDTH = 40
    SPACING = int((MODULE_SIDE - 2 * PIPE_WIDTH) / 3)
    MEM_COMP_WIDTH = 60
    MODULES_X_OFFSET = MEM_COMP_WIDTH + SPACING
    MODULES_Y_OFFSET = MODULE_SIDE

    exX, exY = MODULES_X_OFFSET, MODULES_Y_OFFSET
    deX, deY = MODULES_X_OFFSET + 2 * MODULE_SIDE, MODULES_Y_OFFSET
    icX, icY = MODULES_X_OFFSET + 2 * MODULE_SIDE, MODULES_Y_OFFSET + 2 * MODULE_SIDE
    lsX, lsY = MODULES_X_OFFSET, MODULES_Y_OFFSET + 2 * MODULE_SIDE

    components = {
      "Registers": (int(exX + MODULE_SIDE / 2), 0, 2 * MODULE_SIDE, MEM_COMP_WIDTH),
      "Stack":     (0, 0, MEM_COMP_WIDTH, 2 * MODULE_SIDE),
      "EX":        (exX, exY, MODULE_SIDE, MODULE_SIDE),
      "DE":        (deX, deY, MODULE_SIDE, MODULE_SIDE),
      "IC":        (icX, icY, MODULE_SIDE, MODULE_SIDE),
      "LS":        (lsX, lsY, MODULE_SIDE, MODULE_SIDE),
      "LS_Cache":  (lsX - MEM_COMP_WIDTH, lsY, MEM_COMP_WIDTH, MODULE_SIDE),
      "IC_Cache":  (icX + MODULE_SIDE, icY, MEM_COMP_WIDTH, MODULE_SIDE),
      "Memory":    (exX, lsY + MODULE_SIDE + SPACING, 3 * MODULE_SIDE, MEM_COMP_WIDTH),
      "EX_to_DE":  (exX + MODULE_SIDE, exY + SPACING, MODULE_SIDE, PIPE_WIDTH),
      "DE_to_EX":  (exX + MODULE_SIDE, exY + 2 * SPACING + PIPE_WIDTH, MODULE_SIDE, PIPE_WIDTH),
      "DE_to_IC":  (deX + SPACING, deY + MODULE_SIDE, PIPE_WIDTH, MODULE_SIDE),
      "IC_to_DE":  (deX + 2 * SPACING + PIPE_WIDTH, deY + MODULE_SIDE, PIPE_WIDTH, MODULE_SIDE),
      "IC_to_LS":  (lsX + MODULE_SIDE, lsY + 2 * SPACING + PIPE_WIDTH, MODULE_SIDE, PIPE_WIDTH),
      "LS_to_IC":  (lsX + MODULE_SIDE, lsY + SPACING, MODULE_SIDE, PIPE_WIDTH),
      "LS_to_EX":  (exX + SPACING, exY + MODULE_SIDE, PIPE_WIDTH, MODULE_SIDE),
      "EX_to_LS":  (exX + 2 * SPACING + PIPE_WIDTH, exY + MODULE_SIDE, PIPE_WIDTH, MODULE_SIDE)
    }

    for name, (x, y, w, h) in components.items():
      component = CreateComponent(name, x, y, w, h, self.diagramScene)
      self.componentItems[name] = component

    self.diagramScene.setSceneRect(self.diagramScene.itemsBoundingRect())
    self.diagramView.fitInView(self.diagramScene.sceneRect(), Qt.AspectRatioMode.KeepAspectRatio)

  # ---------------------------------------------------------------------------------------------------------------------------
  def GetMaxCycleNumber(self, directory):
    maxCycle = 0
    if os.path.exists(directory):
      for file in os.listdir(directory):
        if file.endswith('.json'):
          try:
            cycle = int(file.split('.')[0])
            maxCycle = max(maxCycle, cycle)
          except ValueError:
            pass
    return maxCycle

  # ---------------------------------------------------------------------------------------------------------------------------
  def BuildMemoryMapping(self):
    tempDir = self.parent.GetTempDir()
    cpuStatesDir = os.path.join(tempDir, "simulation", "cpu_states")
    memoryDir = os.path.join(tempDir, "simulation", "memory")

    self.memoryMapping = {}

    memoryStates = set()
    if os.path.exists(memoryDir):
      for file in os.listdir(memoryDir):
        if file.endswith('.json'):
          try:
            cycle = int(file.split('.')[0])
            memoryStates.add(cycle)
          except ValueError:
            pass

    lastMemoryState = 1

    for cycle in range(1, self.totalCycles + 1):
      if cycle in memoryStates:
        lastMemoryState = cycle

      stateFilePath = os.path.join(cpuStatesDir, f"{cycle}.json")
      if os.path.exists(stateFilePath):
        try:
          with open(stateFilePath, 'r') as file:
            state = json.load(file)
            memoryUnchangedSince = state.get("memoryUnchangedSinceCyle", cycle)
            if memoryUnchangedSince in memoryStates:
              lastMemoryState = memoryUnchangedSince
        except Exception:
          pass

      self.memoryMapping[cycle] = lastMemoryState

  # ---------------------------------------------------------------------------------------------------------------------------
  def LoadStateFile(self, cycle):
    tempDir = self.parent.GetTempDir()
    stateFilePath = os.path.join(tempDir, "simulation", "cpu_states", f"{cycle}.json")

    if os.path.exists(stateFilePath):
      try:
        with open(stateFilePath, 'r') as file:
          state = json.load(file)
          return state
      except Exception as e:
        QMessageBox.warning(self, "Warning", f"Error loading state file {cycle}.json: {str(e)}")

    return None

  # ---------------------------------------------------------------------------------------------------------------------------
  def LoadMemoryFile(self, cycle):
    tempDir = self.parent.GetTempDir()
    memoryFilePath = os.path.join(tempDir, "simulation", "memory", f"{cycle}.json")

    if os.path.exists(memoryFilePath):
      try:
        with open(memoryFilePath, 'r') as file:
          memory = json.load(file)
          return memory
      except Exception as e:
        QMessageBox.warning(self, "Warning", f"Error loading memory file {cycle}.json: {str(e)}")

    return None

  # ---------------------------------------------------------------------------------------------------------------------------
  def GetMemoryForCycle(self, cycle):
    if cycle in self.memoryMapping:
      memoryCycle = self.memoryMapping[cycle]
      return self.LoadMemoryFile(memoryCycle)
    return {}

  # ---------------------------------------------------------------------------------------------------------------------------
  def LoadSimulationData(self):
    tempDir = self.parent.GetTempDir()
    cpuStatesDir = self.parent.GetSimulationCpuStatesDir()
    memoryDir = self.parent.GetSimulationMemoryDir()

    if not os.path.exists(cpuStatesDir) or not os.path.exists(memoryDir):
      QMessageBox.information(self, "Information",
                              "Simulation directories not found. Please make sure that the directories exist:\n"
                              f"- {cpuStatesDir}\n"
                              f"- {memoryDir}")
      return False

    self.totalCycles = self.GetMaxCycleNumber(cpuStatesDir)
    if self.totalCycles == 0:
      QMessageBox.information(self, "Information",
                              "No simulation states found. Please make sure cycle JSON files exist in the directories.")
      return False

    self.cycleSlider.setMaximum(self.totalCycles)
    self.cycleSlider.setValue(1)
    self.cycleLabel.setText("1")
    self.currentCycleIndex = 1

    self.BuildMemoryMapping()
    self.UpdateSimulationView()
    return True

  # ---------------------------------------------------------------------------------------------------------------------------
  def UpdateSimulationView(self):
    currentCycle = self.cycleSlider.value()
    self.cycleLabel.setText(str(currentCycle))

    self.currentState = self.LoadStateFile(currentCycle)
    self.previousState = self.LoadStateFile(currentCycle - 1) if currentCycle > 1 else None
    self.currentMemory = self.GetMemoryForCycle(currentCycle)
    self.previousMemory = self.GetMemoryForCycle(currentCycle - 1) if currentCycle > 1 else None

    self.UpdateComponentStates()

    if self.selectedComponent and self.currentState:
      self.UpdateDetailsDisplay()

  # ---------------------------------------------------------------------------------------------------------------------------
  def UpdateComponentStates(self):
    for name, component in self.componentItems.items():
      component.SetSelected(name == self.selectedComponent)

      hasChanged = False
      if self.currentCycleIndex == 1:
        hasChanged = name in ["IC", "Memory"]
      elif self.previousState:
        if name == "Memory":
          currentMemoryCycle = self.memoryMapping.get(self.currentCycleIndex, self.currentCycleIndex)
          previousMemoryCycle = self.memoryMapping.get(self.currentCycleIndex - 1, self.currentCycleIndex - 1)
          hasChanged = currentMemoryCycle != previousMemoryCycle
        else:
          hasChanged = component.CompareStates(self.currentState, self.previousState,
                                               self.currentMemory, self.previousMemory)

      component.SetChanged(hasChanged)
      component.SetHovered(name == self.currentHoveredComponent)

  # ---------------------------------------------------------------------------------------------------------------------------
  def UpdateDetailsDisplay(self):
    while self.detailsLayout.count() > 0:
      item = self.detailsLayout.takeAt(0)
      if item.widget():
        item.widget().deleteLater()
      elif item.layout():
        self.ClearLayout(item.layout())
        item.layout().deleteLater()

    if not self.selectedComponent or not self.currentState:
      self.detailsLayout.addStretch(1)
      return

    component = self.componentItems[self.selectedComponent]
    detailsData = component.GetDetailsText(self.currentState, self.currentMemory,
                                           self.previousState, self.previousMemory)

    if detailsData:
      detailsFont = QFont()
      detailsFont.setPointSize(12)

      for itemData in detailsData:
        if itemData.get("type") == "text":
          textLabel = QLabel(itemData.get("content"))
          textLabel.setWordWrap(True)
          textLabel.setFont(detailsFont)

          if itemData.get("changed", False):
            changedColor = ComponentColors.FILL_CHANGED.name()
            textLabel.setStyleSheet(f"QLabel {{ background-color: {changedColor}; padding: 2px; border-radius: 3px; }}")
          else:
            textLabel.setStyleSheet("QLabel { background-color: transparent; }")
          
          self.detailsLayout.addWidget(textLabel)
        elif itemData.get("type") == "table":
          tableWidget = QTableWidget()
          headers = itemData.get("headers", [])
          rows = itemData.get("rows", [])

          # Process the Changed column
          changedColIdx = -1
          if "Changed" in headers:
            changedColIdx = headers.index("Changed")
            headers.pop(changedColIdx)  # Remove Changed column from headers
            
          processedRows = []
          for rowData in rows:
            rowChanged = rowData[changedColIdx]=="Yes" if changedColIdx != -1 else False
            if changedColIdx != -1:
              newRow = rowData[:changedColIdx]
            else:
              newRow = rowData
            processedRows.append({"data": newRow, "changed": rowChanged})
          
          tableWidget.setColumnCount(len(headers))
          tableWidget.setHorizontalHeaderLabels(headers)
          tableWidget.setRowCount(len(processedRows))

          headerFont = QFont()
          headerFont.setPointSize(12)
          for i in range(tableWidget.columnCount()):
            headerItem = tableWidget.horizontalHeaderItem(i)
            if headerItem:
              headerItem.setFont(headerFont)

          for rIdx, rowItem in enumerate(processedRows):
            rowData = rowItem["data"]
            for cIdx, cellData in enumerate(rowData):
              tableItem = QTableWidgetItem(str(cellData))
              tableItem.setFlags(Qt.ItemIsEnabled)
              tableItem.setFont(detailsFont)
              tableItem.setTextAlignment(Qt.AlignCenter)
              if rowItem["changed"]:
                tableItem.setBackground(ComponentColors.FILL_CHANGED)
              tableWidget.setItem(rIdx, cIdx, tableItem)

          tableWidget.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
          tableWidget.verticalHeader().setVisible(False)
          tableWidget.setStyleSheet(
            "QTableWidget {"
            "   border: 1px solid black;"
            "   background-color: white;"
            "}"
            "QHeaderView::section {"
            "   background-color: #dcdcdc;"
            "   color: black;"
            "   padding: 4px;"
            "   border: 1px solid black;"
            "   border-bottom-color: black;"
            "   font-weight: bold;"
            "}"
            "QTableWidget::horizontalHeader { border-bottom: 1px solid black; }"
          )
          tableWidget.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
          tableWidget.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
          tableWidget.resizeRowsToContents()
          tableWidget.resizeColumnsToContents()
          tableHeight = tableWidget.horizontalHeader().height()
          for r in range(tableWidget.rowCount()):
            tableHeight += tableWidget.rowHeight(r)
          tableWidget.setMinimumHeight(tableHeight)
          tableWidget.setMaximumHeight(tableHeight)

          self.detailsLayout.addWidget(tableWidget)
      self.detailsLayout.addStretch(1)
    else:
      self.detailsLayout.addStretch(1)

  # ---------------------------------------------------------------------------------------------------------------------------
  def ClearLayout(self, layout):
    if layout is not None:
      while layout.count():
        item = layout.takeAt(0)
        if item.widget():
          item.widget().deleteLater()
        elif item.layout():
          self.ClearLayout(item.layout())
          item.layout().deleteLater()

  # ---------------------------------------------------------------------------------------------------------------------------
  def eventFilter(self, obj, event):
    if obj == self.diagramView.viewport():
      if event.type() == QEvent.MouseMove:
        pos = self.diagramView.mapToScene(event.pos())
        item = self.diagramScene.itemAt(pos, self.diagramView.transform())

        newHoveredComponent = None
        if isinstance(item, QGraphicsRectItem):
          componentName = item.data(0)
          if componentName and componentName in self.componentItems:
            newHoveredComponent = componentName

        if newHoveredComponent != self.currentHoveredComponent:
          if self.currentHoveredComponent and self.currentHoveredComponent in self.componentItems:
            self.componentItems[self.currentHoveredComponent].SetHovered(False)
          self.currentHoveredComponent = newHoveredComponent
          if self.currentHoveredComponent:
            self.componentItems[self.currentHoveredComponent].SetHovered(True)

      elif event.type() == QEvent.MouseButtonPress:
        pos = self.diagramView.mapToScene(event.pos())
        item = self.diagramScene.itemAt(pos, self.diagramView.transform())

        if isinstance(item, QGraphicsRectItem):
          componentName = item.data(0)
          if componentName and componentName in self.componentItems:
            if self.selectedComponent:
              self.componentItems[self.selectedComponent].SetSelected(False)
            self.selectedComponent = componentName
            self.componentItems[self.selectedComponent].SetSelected(True)
            self.UpdateDetailsDisplay()

      elif event.type() == QEvent.Leave:
        if self.currentHoveredComponent and self.currentHoveredComponent in self.componentItems:
          self.componentItems[self.currentHoveredComponent].SetHovered(False)
        self.currentHoveredComponent = None

    return super().eventFilter(obj, event)


# -----------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
  raise Exception("This module is not meant to be run directly.")