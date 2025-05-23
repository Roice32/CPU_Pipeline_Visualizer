import os
import json
from PyQt5.QtWidgets import (
  QWidget, QVBoxLayout, QHBoxLayout, QLabel, QTextEdit, QSlider,
  QGraphicsScene, QGraphicsView, QGraphicsRectItem, QGraphicsTextItem, QMessageBox,
  QPushButton
)
from PyQt5.QtCore import Qt, QEvent, QRectF, QTimer
from PyQt5.QtGui import QPainter, QPen, QColor, QBrush, QIcon


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
  detailsText = None
  playPauseButton = None
  autoPlayTimer = None
  isPlaying = False
  memoryMapping = None  # Maps cycle numbers to the last cycle where memory changed

  # ---------------------------------------------------------------------------------------------------------------------------
  def __init__(self, parent):
    super().__init__()
    self.parent = parent
    self.totalCycles = 0
    self.currentCycleIndex = 1
    self.selectedComponent = None
    self.componentItems = {}
    self.memoryMapping = {}

    layout = QVBoxLayout()

    # Slider and control buttons layout
    sliderLayout = QVBoxLayout()
    sliderLayout.setContentsMargins(int(self.width() * 0.2), 0, int(self.width() * 0.2), 0)

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
    toStartIcon  = QIcon("resources/icons/toStart.png")
    previousIcon = QIcon("resources/icons/previous.png")
    playIcon     = QIcon("resources/icons/play.png")
    pauseIcon    = QIcon("resources/icons/pause.png")
    nextIcon     = QIcon("resources/icons/next.png")
    toEndIcon    = QIcon("resources/icons/toEnd.png")

    # Create buttons
    toStartButton        = QPushButton(toStartIcon, "")
    previousButton       = QPushButton(previousIcon, "")
    self.playPauseButton = QPushButton(playIcon, "")  # Initially play icon
    nextButton           = QPushButton(nextIcon, "")
    toEndButton          = QPushButton(toEndIcon, "")

    # Connect buttons to functions
    toStartButton.clicked.connect(self.GoToFirstCycle)
    previousButton.clicked.connect(self.DecreaseCycle)
    self.playPauseButton.clicked.connect(self.TogglePlayPause)
    nextButton.clicked.connect(self.IncreaseCycle)
    toEndButton.clicked.connect(self.GoToLastCycle)

    # Add buttons to layout
    buttonLayout.addWidget(toStartButton)
    buttonLayout.addWidget(previousButton)
    buttonLayout.addWidget(self.playPauseButton)
    buttonLayout.addWidget(nextButton)
    buttonLayout.addWidget(toEndButton)

    sliderLayout.addLayout(buttonLayout)
    layout.addLayout(sliderLayout)

    # Diagram and Details Layout
    diagramDetailsLayout = QHBoxLayout()
    diagramDetailsLayout.setContentsMargins(0, 10, 0, 0)  # Add some top margin

    # CPU diagram view
    diagramLayout = QVBoxLayout()
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
    self.diagramView.setMinimumWidth(300)  # Ensure it has some width
    self.diagramView.viewport().installEventFilter(self)
    self.diagramView.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff) # Remove horizontal scrollbar
    self.diagramView.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)   # Remove vertical scrollbar
    diagramLayout.addWidget(self.diagramView, 1)  # Stretch vertically
    diagramDetailsLayout.addLayout(diagramLayout, 1)  # Stretch horizontally

    # Details text box
    detailsLayout = QVBoxLayout()
    detailsLabel = QLabel("Component Details:")
    detailsLabel.setAlignment(Qt.AlignCenter)
    font = detailsLabel.font()
    font.setBold(True)
    detailsLabel.setFont(font)
    detailsLayout.addWidget(detailsLabel)
    self.detailsText = QTextEdit()
    self.detailsText.setAcceptRichText(True)
    self.detailsText.setStyleSheet("border: 3px solid black; border-radius: 8px; background-color: #f0f0f0;")
    self.detailsText.setReadOnly(True)
    detailsLayout.addWidget(self.detailsText, 1)  # Stretch vertically
    diagramDetailsLayout.addLayout(detailsLayout, 1)  # Stretch horizontally

    layout.addLayout(diagramDetailsLayout, 1)  # Use all available space

    # Create the CPU diagram components
    self.CreateCpuDiagram()

    self.setLayout(layout)

    # Timer for auto-play
    self.autoPlayTimer = QTimer(self)
    self.autoPlayTimer.timeout.connect(self.IncreaseCycle)
    self.autoPlayTimer.setInterval(1000)  # 1 second

  # ---------------------------------------------------------------------------------------------------------------------------
  def resizeEvent(self, event):
    # Fit the scene into the view, keeping aspect ratio
    self.diagramView.fitInView(self.diagramScene.sceneRect(), Qt.AspectRatioMode.KeepAspectRatio)
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
        self.TogglePlayPause()  # Stop playing at the end

  # ---------------------------------------------------------------------------------------------------------------------------
  def GoToLastCycle(self):
    self.cycleSlider.setValue(self.cycleSlider.maximum())

  # ---------------------------------------------------------------------------------------------------------------------------
  def CreateCpuDiagram(self):
    # Clear previous scene
    self.diagramScene.clear()
    self.componentItems = {}

    # Define colors
    borderColor = QColor(0, 0, 0)
    fillColor = QColor(255, 255, 255)

    MODULE_SIDE = 120
    PIPE_WIDTH = 40
    SPACING = int((MODULE_SIDE - 2 * PIPE_WIDTH) / 3)
    MEM_COMP_WIDTH = 60
    MODULES_X_OFFSET = MEM_COMP_WIDTH + SPACING
    MODULES_Y_OFFSET = MODULE_SIDE

    EX_X, EX_Y = MODULES_X_OFFSET,           MODULES_Y_OFFSET
    DE_X, DE_Y = MODULES_X_OFFSET + 2 * MODULE_SIDE, MODULES_Y_OFFSET
    IC_X, IC_Y = MODULES_X_OFFSET + 2 * MODULE_SIDE, MODULES_Y_OFFSET + 2 * MODULE_SIDE
    LS_X, LS_Y = MODULES_X_OFFSET,           MODULES_Y_OFFSET + 2 * MODULE_SIDE

    # Define component geometries (x, y, width, height)
    components = {
      "Registers": (int(EX_X + MODULE_SIDE / 2),   0,                 2 * MODULE_SIDE, MEM_COMP_WIDTH),
      "Stack":   (0,                 0,                 MEM_COMP_WIDTH,  2 * MODULE_SIDE),
      "EX":    (EX_X,              EX_Y,              MODULE_SIDE,   MODULE_SIDE),
      "DE":    (DE_X,              DE_Y,              MODULE_SIDE,   MODULE_SIDE),
      "IC":    (IC_X,              IC_Y,              MODULE_SIDE,   MODULE_SIDE),
      "LS":    (LS_X,              LS_Y,              MODULE_SIDE,   MODULE_SIDE),
      "LS_Cache":  (LS_X - MEM_COMP_WIDTH,       LS_Y,              MEM_COMP_WIDTH,  MODULE_SIDE),
      "IC_Cache":  (IC_X + MODULE_SIDE,        IC_Y,              MEM_COMP_WIDTH,  MODULE_SIDE),
      "Memory":  (EX_X,              LS_Y + MODULE_SIDE + SPACING,  3 * MODULE_SIDE, MEM_COMP_WIDTH),
      "EX_to_DE":  (EX_X + MODULE_SIDE,        EX_Y + SPACING,          MODULE_SIDE,   PIPE_WIDTH),
      "DE_to_EX":  (EX_X + MODULE_SIDE,        EX_Y + 2 * SPACING + PIPE_WIDTH, MODULE_SIDE,   PIPE_WIDTH),
      "DE_to_IC":  (DE_X + SPACING,          DE_Y + MODULE_SIDE,        PIPE_WIDTH,    MODULE_SIDE),
      "IC_to_DE":  (DE_X + 2 * SPACING + PIPE_WIDTH, DE_Y + MODULE_SIDE,        PIPE_WIDTH,    MODULE_SIDE),
      "IC_to_LS":  (LS_X + MODULE_SIDE,        LS_Y + 2 * SPACING + PIPE_WIDTH, MODULE_SIDE,   PIPE_WIDTH),
      "LS_to_IC":  (LS_X + MODULE_SIDE,        LS_Y + SPACING,          MODULE_SIDE,   PIPE_WIDTH),
      "LS_to_EX":  (EX_X + SPACING,          EX_Y + MODULE_SIDE,        PIPE_WIDTH,    MODULE_SIDE),
      "EX_to_LS":  (EX_X + 2 * SPACING + PIPE_WIDTH, EX_Y + MODULE_SIDE,        PIPE_WIDTH,    MODULE_SIDE)
    }

    # Create rectangular components with labels
    for name, (x, y, w, h) in components.items():
      rect = QGraphicsRectItem(x, y, w, h)
      rect.setPen(QPen(borderColor, 2))
      rect.setBrush(QBrush(fillColor))
      rect.setData(0, name)  # Store the component name

      # Add text label
      text = QGraphicsTextItem(name.replace("_", " " if w > h else "\n"))
      scale = 1.25
      text.setScale(scale)
      text.setPos(x + w / 2 - text.boundingRect().width() / 2 * scale, y + h / 2 - text.boundingRect().height() / 2 * scale)

      # Store the reference to the rectangle
      self.componentItems[name] = rect

      # Add to scene
      self.diagramScene.addItem(rect)
      self.diagramScene.addItem(text)

    # Set the scene rect to fit all components
    self.diagramScene.setSceneRect(self.diagramScene.itemsBoundingRect())

    # Fit the view to the scene
    self.diagramView.fitInView(self.diagramScene.sceneRect(), Qt.AspectRatioMode.KeepAspectRatio)

  # ---------------------------------------------------------------------------------------------------------------------------
  def GetMaxCycleNumber(self, directory):
    """Find the highest cycle number from files in the directory"""
    maxCycle = 0
    if os.path.exists(directory):
      for file in os.listdir(directory):
        if file.endswith('.json'):
          try:
            cycle = int(file.split('.')[0])
            maxCycle = max(maxCycle, cycle)
          except ValueError:
            pass  # Ignore files that don't have a number as filename
    return maxCycle

  # ---------------------------------------------------------------------------------------------------------------------------
  def BuildMemoryMapping(self):
    """Build a mapping of cycle numbers to the memory state they should use"""
    tempDir = self.parent.GetTempDir()
    cpuStatesDir = os.path.join(tempDir, "simulation", "cpu_states")
    memoryDir = os.path.join(tempDir, "simulation", "memory")
    
    # Reset the memory mapping
    self.memoryMapping = {}
    
    # Get all available memory states
    memoryStates = set()
    if os.path.exists(memoryDir):
      for file in os.listdir(memoryDir):
        if file.endswith('.json'):
          try:
            cycle = int(file.split('.')[0])
            memoryStates.add(cycle)
          except ValueError:
            pass
    
    # Now go through each CPU state and determine which memory state to use
    lastMemoryState = 1  # Default to first cycle if nothing else is found
    
    for cycle in range(1, self.totalCycles + 1):
      # Check if this cycle has a memory state
      if cycle in memoryStates:
        lastMemoryState = cycle
      
      # Alternative: check if CPU state says memory hasn't changed
      stateFilePath = os.path.join(cpuStatesDir, f"{cycle}.json")
      if os.path.exists(stateFilePath):
        try:
          with open(stateFilePath, 'r') as file:
            state = json.load(file)
            memoryUnchangedSince = state.get("memoryUnchangedSinceCyle", cycle)
            if memoryUnchangedSince in memoryStates:
              lastMemoryState = memoryUnchangedSince
        except Exception:
          pass  # If there's any error reading the file, just use the last known memory state
      
      # Store the mapping
      self.memoryMapping[cycle] = lastMemoryState

  # ---------------------------------------------------------------------------------------------------------------------------
  def LoadStateFile(self, cycle):
    """Load a specific CPU state file"""
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
    """Load a specific memory state file"""
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
    """Get the memory state for a specific cycle"""
    if cycle in self.memoryMapping:
      memoryCycle = self.memoryMapping[cycle]
      return self.LoadMemoryFile(memoryCycle)
    return {}  # Return empty dict if no memory state found

  # ---------------------------------------------------------------------------------------------------------------------------
  def LoadSimulationData(self):
    # Check if simulation directories exist
    tempDir = self.parent.GetTempDir()
    cpuStatesDir = self.parent.GetSimulationCpuStatesDir()
    memoryDir = self.parent.GetSimulationMemoryDir()
    
    if not os.path.exists(cpuStatesDir) or not os.path.exists(memoryDir):
      QMessageBox.information(self, "Information", 
                              "Simulation directories not found. Please make sure that the directories exist:\n"
                              f"- {cpuStatesDir}\n"
                              f"- {memoryDir}")
      return False

    # Find the total number of simulation cycles
    self.totalCycles = self.GetMaxCycleNumber(cpuStatesDir)
    if self.totalCycles == 0:
      QMessageBox.information(self, "Information", 
                              "No simulation states found. Please make sure cycle JSON files exist in the directories.")
      return False

    # Update slider range
    self.cycleSlider.setMaximum(self.totalCycles)
    self.cycleSlider.setValue(1)  # Start at the first cycle
    self.cycleLabel.setText("1")
    self.currentCycleIndex = 1
    
    # Build the memory mapping
    self.BuildMemoryMapping()
    
    # Update the view
    self.UpdateSimulationView()
    return True

  # ---------------------------------------------------------------------------------------------------------------------------
  def UpdateSimulationView(self):
    currentCycle = self.cycleSlider.value()
    self.cycleLabel.setText(str(currentCycle))
    
    # Load the current state
    state = self.LoadStateFile(currentCycle)

    # Reset all component highlighting
    for item in self.componentItems.values():
      item.setPen(QPen(QColor(0, 0, 0), 2))
      item.setBrush(QBrush(QColor(255, 255, 255)))

    # If a component is selected, update its highlighting and show details
    if self.selectedComponent and state:
      memory = self.GetMemoryForCycle(currentCycle)
      
      self.componentItems[self.selectedComponent].setPen(QPen(QColor(0, 0, 255), 3))
      details = self.GetComponentDetails(self.selectedComponent, state, memory)
      self.detailsText.setText(details)

  # ---------------------------------------------------------------------------------------------------------------------------
  def GetComponentDetails(self, component, state, memory):
    details = f"Component: {component}\nCycle: {state.get('cycle', 0)}\n\n"

    # Extract and format details based on component type
    if component == "Registers":
      regData = state.get("registers", {})
      details += f"IP: {hex(regData.get('IP', 0))}\n"
      details += f"Flags: {hex(regData.get('flags', 0))}\n"
      details += f"Stack Base: {hex(regData.get('stackBase', 0))}\n"
      details += f"Stack Size: {regData.get('stackSize', 0)}\n"
      details += f"Stack Pointer: {regData.get('stackPointer', 0)}\n\n"

      # Add R registers
      rRegs = regData.get("R", [])
      for i, val in enumerate(rRegs):
        details += f"R{i}: {hex(val)}\n"

      # Add Z registers if available
      zRegs = regData.get("Z", [])
      for i, zReg in enumerate(zRegs):
        details += f"Z{i}: {[hex(v) for v in zReg]}\n"

    elif component == "Stack":
      stack = state.get("stack", [])
      if stack:
        details += "Stack contents (top to bottom):\n"
        for i, val in enumerate(stack):
          details += f"{i}: {hex(val)}\n"
      else:
        details += "Stack is empty\n"

    elif component == "Memory":
      if memory:
        details += "Memory contents:\n"
        # Sort addresses for a more consistent display
        sorted_addresses = sorted(memory.keys(), key=lambda x: int(x, 16) if isinstance(x, str) else int(x))
        for addr in sorted_addresses:
          val = memory[addr].zfill(4)
          details += f"#{addr}: {val}\n"
      else:
        details += "Memory data not available for this cycle\n"

    elif component in ["EX", "DE", "LS", "IC"]:
      componentData = state.get(component, {})
      details += f"State: {componentData.get('state', 'Unknown')}\n\n"

      if component == "LS" or component == "IC":
        cacheData = componentData.get("cache", {})
        details += f"Cache Size: {cacheData.get('size', 0)}\n"

        if component == "LS":
          details += f"Physical Memory Access: {'Yes' if componentData.get('physicalMemoryAccessHappened', False) else 'No'}\n"
          details += f"Found Index: {cacheData.get('foundIndex', 0)}\n"
        else:
          details += f"InternalIP: {hex(componentData.get('internalIP', 0))}\n"

      elif component == "DE":
        fwStorage = componentData.get("fwTempStorage", {})
        details += f"Cache Start Address: {hex(fwStorage.get('cacheStartAddr', 0))}\n"
        details += f"Stored Words Count: {fwStorage.get('storedWordsCount', 0)}\n"

        storedFWs = fwStorage.get("storedFWs", [])
        for i, fw in enumerate(storedFWs):
          details += f"FW {i}: {fw}\n"

    elif component.endswith("Cache"):
      if component == "LS_Cache":
        cacheData = state.get("LS", {}).get("cache", {})
      else:
        cacheData = state.get("IC", {}).get("cache", {})

      details += f"Cache Size: {cacheData.get('size', 0)}\n"

      storage = cacheData.get("storage", [])
      if storage:
        details += f"Storage entries: {len(storage)}\n"
        for i, entry in enumerate(storage[:5]):
          details += f"Entry {i}: {entry}\n"
        if len(storage) > 5:
          details += "...\n"

    elif "to" in component:
      pipes = state.get("pipes", {})
      pipeName = component.replace("_", "")
      pipeData = pipes.get(pipeName, [])

      if pipeData:
        details += f"Pipeline entries: {len(pipeData)}\n"
        for i, entry in enumerate(pipeData):
          details += f"Entry {i}: {entry}\n"
      else:
        details += "Pipeline is empty\n"

    return details

  # ---------------------------------------------------------------------------------------------------------------------------
  def eventFilter(self, obj, event):
    if obj == self.diagramView.viewport():
      if event.type() == QEvent.MouseMove:
        pos = self.diagramView.mapToScene(event.pos())
        item = self.diagramScene.itemAt(pos, self.diagramView.transform())

        if isinstance(item, QGraphicsRectItem):
          componentName = item.data(0)
          if componentName:
            for name, rect in self.componentItems.items():
              if name != self.selectedComponent:
                if name == componentName:
                  rect.setBrush(QBrush(QColor(220, 220, 220)))
                else:
                  rect.setBrush(QBrush(QColor(255, 255, 255)))

      elif event.type() == QEvent.MouseButtonPress:
        pos = self.diagramView.mapToScene(event.pos())
        item = self.diagramScene.itemAt(pos, self.diagramView.transform())

        if isinstance(item, QGraphicsRectItem):
          componentName = item.data(0)

          if componentName:
            self.selectedComponent = componentName
            self.UpdateSimulationView()

    return super().eventFilter(obj, event)


# -----------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
  raise Exception("This module is not meant to be run directly.")
