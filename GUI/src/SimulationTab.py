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
  executionStates = None
  memoryStates = None
  memoryRanges = None
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

  # ---------------------------------------------------------------------------------------------------------------------------
  def __init__(self, parent):
    super().__init__()
    self.parent = parent
    self.executionStates = {}    # Dict to store loaded states by cycle number
    self.memoryStates = {}       # Dict to store memory states
    self.memoryRanges = {}       # Dict mapping ranges of cycles to memory states
    self.totalCycles = 0
    self.currentCycleIndex = 1
    self.selectedComponent = None
    self.componentItems = {}

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
    # Load first 6 states or as many as available
    self.LoadStateRange(1, min(6, self.totalCycles))

  # ---------------------------------------------------------------------------------------------------------------------------
  def DecreaseCycle(self):
    if self.cycleSlider.value() > self.cycleSlider.minimum():
      newCycle = self.cycleSlider.value() - 1
      self.cycleSlider.setValue(newCycle)
      self.currentCycleIndex = newCycle
      
      # Check if we need to load more states (when current is at the lower edge of our window)
      if newCycle % 11 == 1 and newCycle > 1:
        self.LoadStateRange(max(1, newCycle - 5), min(newCycle + 5, self.totalCycles))

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
      
      # Check if we need to load more states (when current is at the upper edge of our window)
      if newCycle % 11 == 0 and newCycle < self.totalCycles:
        self.LoadStateRange(max(1, newCycle - 5), min(newCycle + 5, self.totalCycles))
    else:
      if self.isPlaying:
        self.TogglePlayPause()  # Stop playing at the end

  # ---------------------------------------------------------------------------------------------------------------------------
  def GoToLastCycle(self):
    self.cycleSlider.setValue(self.cycleSlider.maximum())
    # Load last 6 states or as many as available
    self.LoadStateRange(max(1, self.totalCycles - 5), self.totalCycles)

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
  def LoadStateRange(self, start, end):
    """Load CPU states and memory states for a range of cycles"""
    # Clear old states outside our new window
    cycles_to_keep = set(range(start, end + 1))
    cycles_to_remove = [c for c in self.executionStates.keys() if c not in cycles_to_keep]
    for cycle in cycles_to_remove:
      self.executionStates.pop(cycle, None)
    
    # Load new states
    for cycle in range(start, end + 1):
      if cycle not in self.executionStates:
        state = self.LoadStateFile(cycle)
        if state:
          self.executionStates[cycle] = state
          
          # Check if we need to update memory state
          memoryUnchangedSince = state.get("memoryUnchangedSinceCyle", cycle)
          if memoryUnchangedSince not in self.memoryStates:
            # Try to load this memory state
            memory = self.LoadMemoryFile(memoryUnchangedSince)
            if memory:
              self.memoryStates[memoryUnchangedSince] = memory
          
          # Update memory ranges
          for memRange in list(self.memoryRanges.keys()):
            if cycle in memRange:
              # Remove the old range
              memState = self.memoryRanges[memRange]
              self.memoryRanges.pop(memRange)
              # Determine new ranges
              newRange = frozenset(range(memoryUnchangedSince, cycle + 1))
              self.memoryRanges[newRange] = memState
              break
          else:
            # If not in any existing range, create a new one
            newRange = frozenset(range(memoryUnchangedSince, cycle + 1))
            if memoryUnchangedSince in self.memoryStates:
              self.memoryRanges[newRange] = self.memoryStates[memoryUnchangedSince]

  # ---------------------------------------------------------------------------------------------------------------------------
  def GetMemoryForCycle(self, cycle):
    """Get the memory state for a specific cycle"""
    for memRange, memState in self.memoryRanges.items():
      if cycle in memRange:
        return memState
    
    # If not found in our cached ranges, try to find from which cycle memory is unchanged
    if cycle in self.executionStates:
      memoryUnchangedSince = self.executionStates[cycle].get("memoryUnchangedSinceCyle", cycle)
      memory = self.LoadMemoryFile(memoryUnchangedSince)
      if memory:
        # Create a new range
        newRange = frozenset(range(memoryUnchangedSince, cycle + 1))
        self.memoryStates[memoryUnchangedSince] = memory
        self.memoryRanges[newRange] = memory
        return memory
    
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
    
    # Load first 6 states or all states if less than 6
    self.LoadStateRange(1, min(6, self.totalCycles))
    
    # Update the view
    self.UpdateSimulationView()
    return True

  # ---------------------------------------------------------------------------------------------------------------------------
  def UpdateSimulationView(self):
    currentCycle = self.cycleSlider.value()
    self.cycleLabel.setText(str(currentCycle))
    
    # Make sure we have the current state loaded
    if currentCycle not in self.executionStates:
      # Need to load this state and potentially surrounding states
      minCycle = max(1, currentCycle - 5)
      maxCycle = min(self.totalCycles, currentCycle + 5)
      self.LoadStateRange(minCycle, maxCycle)

    # Reset all component highlighting
    for item in self.componentItems.values():
      item.setPen(QPen(QColor(0, 0, 0), 2))
      item.setBrush(QBrush(QColor(255, 255, 255)))

    # If a component is selected, update its highlighting and show details
    if self.selectedComponent and currentCycle in self.executionStates:
      state = self.executionStates[currentCycle]
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
        details += f"Current Request Index: {cacheData.get('currReqIndex', 0)}\n"
        details += f"Current Request Tag: {hex(cacheData.get('currReqTag', 0))}\n"

        if component == "LS":
          details += f"Physical Memory Access: {'Yes' if componentData.get('physicalMemoryAccessHappened', False) else 'No'}\n"
          details += f"Found Index: {cacheData.get('foundIndex', 0)}\n"

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