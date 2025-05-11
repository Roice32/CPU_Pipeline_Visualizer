import os
import json
from PyQt5.QtWidgets import (
  QWidget, QVBoxLayout, QHBoxLayout, QLabel, QTextEdit, QSlider, 
  QGraphicsScene, QGraphicsView, QGraphicsRectItem, QGraphicsTextItem, QMessageBox
)
from PyQt5.QtCore import Qt, QEvent, QRectF
from PyQt5.QtGui import QPainter, QPen, QColor, QBrush


# -----------------------------------------------------------------------------------------------------------------------------
class SimulationTab(QWidget):
  parent = None
  executionStates = None
  selectedComponent = None
  componentItems = None
  cycleSlider = None
  cycleLabel = None
  diagramScene = None
  diagramView = None
  detailsText = None

  # ---------------------------------------------------------------------------------------------------------------------------
  def __init__(self, parent):
    super().__init__()
    self.parent = parent
    self.executionStates = []
    self.selectedComponent = None
    self.componentItems = {}

    layout = QVBoxLayout()

    # Add slider for cycle selection
    sliderLayout = QHBoxLayout()
    sliderLayout.addWidget(QLabel("Cycle:"))
    self.cycleSlider = QSlider(Qt.Horizontal)
    self.cycleSlider.setMinimum(1)
    self.cycleSlider.setMaximum(1)  # Will be updated when data is loaded
    self.cycleSlider.valueChanged.connect(self.UpdateSimulationView)
    sliderLayout.addWidget(self.cycleSlider)
    self.cycleLabel = QLabel("1")
    sliderLayout.addWidget(self.cycleLabel)
    layout.addLayout(sliderLayout)

    # CPU diagram view
    self.diagramScene = QGraphicsScene()
    self.diagramView = QGraphicsView(self.diagramScene)
    self.diagramView.setRenderHint(QPainter.Antialiasing)
    self.diagramView.setMinimumHeight(500)
    self.diagramView.viewport().installEventFilter(self)
    layout.addWidget(self.diagramView)

    # Details text box
    layout.addWidget(QLabel("Component Details:"))
    self.detailsText = QTextEdit()
    self.detailsText.setReadOnly(True)
    layout.addWidget(self.detailsText)

    # Create the CPU diagram components
    self.CreateCpuDiagram()

    self.setLayout(layout)

  # ---------------------------------------------------------------------------------------------------------------------------
  def CreateCpuDiagram(self):
    # Clear previous scene
    self.diagramScene.clear()
    self.componentItems = {}

    # Define colors
    borderColor = QColor(0, 0, 0)
    fillColor = QColor(255, 255, 255)

    # Define component geometries (x, y, width, height)
    components = {
      "Registers": (550, 30, 400, 80),
      "Stack": (50, 140, 80, 210),
      "EX": (330, 196, 130, 160),
      "DE": (910, 196, 130, 160),
      "LS": (330, 550, 130, 150),
      "IC": (910, 550, 130, 150),
      "LS_Cache": (90, 550, 120, 150),
      "IC_Cache": (1150, 550, 120, 150),
      "Memory": (590, 770, 400, 150),
      "EX_to_DE": (620, 233, 130, 40),
      "DE_to_EX": (620, 330, 130, 40),
      "LS_to_EX": (153, 380, 180, 40),
      "EX_to_LS": (444, 456, 130, 40),
      "LS_to_IC": (556, 569, 130, 40),
      "IC_to_LS": (556, 644, 130, 40),
      "IC_to_DE": (840, 456, 130, 40),
      "DE_to_IC": (1027, 380, 130, 40)
    }

    # Create rectangular components with labels
    for name, (x, y, w, h) in components.items():
      rect = QGraphicsRectItem(x, y, w, h)
      rect.setPen(QPen(borderColor, 2))
      rect.setBrush(QBrush(fillColor))
      rect.setData(0, name)  # Store the component name

      # Add text label
      text = QGraphicsTextItem(name.replace("_", " "))
      text.setPos(x + w / 2 - text.boundingRect().width() / 2, y + h / 2 - text.boundingRect().height() / 2)

      # Store the reference to the rectangle
      self.componentItems[name] = rect

      # Add to scene
      self.diagramScene.addItem(rect)
      self.diagramScene.addItem(text)

    # Set the scene rect to fit all components
    self.diagramScene.setSceneRect(self.diagramScene.itemsBoundingRect())

  # ---------------------------------------------------------------------------------------------------------------------------
  def LoadSimulationData(self):
    # Try to load states.json file
    tempDir = self.parent.GetTempDir()
    statesPath = os.path.join(tempDir, "states.json")

    if os.path.exists(statesPath):
      try:
        with open(statesPath, 'r') as file:
          self.executionStates = json.load(file)

        # Update slider maximum value based on the last cycle
        if self.executionStates:
          maxCycle = int(self.executionStates[-1].get("cycle", 1))
          self.cycleSlider.setMaximum(maxCycle)
          self.cycleSlider.setValue(1)  # Start at the first cycle
          self.cycleLabel.setText(str(1))
          self.UpdateSimulationView()
          return True
      except Exception as e:
        QMessageBox.warning(self, "Warning", f"Error loading simulation states: {str(e)}")
    else:
      QMessageBox.information(self, "Information", "No simulation states file found. Please make sure 'states.json' exists in the CPU_PV.temp directory.")

    return False

  # ---------------------------------------------------------------------------------------------------------------------------
  def UpdateSimulationView(self):
    currentCycle = self.cycleSlider.value()
    self.cycleLabel.setText(str(currentCycle))

    # Reset all component highlighting
    for item in self.componentItems.values():
      item.setPen(QPen(QColor(0, 0, 0), 2))
      item.setBrush(QBrush(QColor(255, 255, 255)))

    # If a component is selected, update its highlighting and show details
    if self.selectedComponent and self.executionStates:
      # Find the state that matches current cycle
      state = None
      for s in self.executionStates:
        if s.get("cycle", 0) == currentCycle:
          state = s
          break

      if state:
        self.componentItems[self.selectedComponent].setPen(QPen(QColor(0, 0, 255), 3))
        details = self.GetComponentDetails(self.selectedComponent, state)
        self.detailsText.setText(details)

  # ---------------------------------------------------------------------------------------------------------------------------
  def GetComponentDetails(self, component, state):
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
      memory = state.get("memory", {})
      if memory:
        details += "Memory contents:\n"
        for addr, val in memory.items():
          details += f"Address {hex(int(addr))}: {hex(val)}\n"
      else:
        details += "Memory is empty\n"

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
