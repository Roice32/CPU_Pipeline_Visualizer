from PyQt5.QtWidgets import QGraphicsRectItem, QGraphicsTextItem
from PyQt5.QtGui import QPen, QColor, QBrush
from ConfigTab import Config

# -----------------------------------------------------------------------------------------------------------------------------
class ComponentColors:
  """Color definitions for components"""
  BORDER_DEFAULT = QColor(0, 0, 0)
  BORDER_SELECTED = QColor(0, 0, 255)
  FILL_DEFAULT = QColor(255, 255, 255)
  FILL_HOVER = QColor(220, 220, 220)
  FILL_CHANGED = QColor(144, 238, 144)  # Light green
  FILL_CHANGED_HOVER = QColor(102, 205, 102)  # Darker green


# -----------------------------------------------------------------------------------------------------------------------------
class DiagramComponent:
  """Base class for all CPU diagram components"""

  def __init__(self, top_parent, name, x, y, width, height, scene):
    self.topParent = top_parent
    self.name = name
    self.x = x
    self.y = y
    self.width = width
    self.height = height
    self.scene = scene
    self.isSelected = False
    self.isHovered = False
    self.hasChanged = False

    # Create graphics items
    self.rect = QGraphicsRectItem(x, y, width, height)
    self.rect.setPen(QPen(ComponentColors.BORDER_DEFAULT, 2))
    self.rect.setBrush(QBrush(ComponentColors.FILL_DEFAULT))
    self.rect.setData(0, name)  # Store the component name

    # Create text label
    displayName = name.replace("_", " " if width > height else "\n")
    self.text = QGraphicsTextItem(displayName)
    scale = 1.25
    self.text.setScale(scale)
    textX = x + width / 2 - self.text.boundingRect().width() / 2 * scale
    textY = y + height / 2 - self.text.boundingRect().height() / 2 * scale
    self.text.setPos(textX, textY)

    # Add to scene
    scene.addItem(self.rect)
    scene.addItem(self.text)

  # ---------------------------------------------------------------------------------------------------------------------------
  def SetSelected(self, selected):
    """Set the selection state of the component"""
    self.isSelected = selected
    self.UpdateVisualState()

  # ---------------------------------------------------------------------------------------------------------------------------
  def SetHovered(self, hovered):
    """Set the hover state of the component"""
    self.isHovered = hovered
    self.UpdateVisualState()

  # ---------------------------------------------------------------------------------------------------------------------------
  def SetChanged(self, changed):
    """Set whether the component has changed from previous cycle"""
    self.hasChanged = changed
    self.UpdateVisualState()

  # ---------------------------------------------------------------------------------------------------------------------------
  def UpdateVisualState(self):
    """Update the visual appearance based on current state"""
    # Border color
    if self.isSelected:
      borderColor = ComponentColors.BORDER_SELECTED
      borderWidth = 3
    else:
      borderColor = ComponentColors.BORDER_DEFAULT
      borderWidth = 2

    # Fill color
    if self.hasChanged:
      if self.isHovered and not self.isSelected:
        fillColor = ComponentColors.FILL_CHANGED_HOVER
      else:
        fillColor = ComponentColors.FILL_CHANGED
    else:
      if self.isHovered and not self.isSelected:
        fillColor = ComponentColors.FILL_HOVER
      else:
        fillColor = ComponentColors.FILL_DEFAULT

    self.rect.setPen(QPen(borderColor, borderWidth))
    self.rect.setBrush(QBrush(fillColor))

  # ---------------------------------------------------------------------------------------------------------------------------
  def GetDetailsText(self, state, memory, previous_state=None, previous_memory=None, garbage_memory=False):
    """Get detailed text for this component. Override in subclasses.
    Returns a list of dicts:
    - {"type": "text", "content": "...", "changed": False}
    - {"type": "table", "headers": [...], "rows": [[...], ...]}
    """
    # Remove "Component:" and "Cycle:"
    return []

  # ---------------------------------------------------------------------------------------------------------------------------
  def CompareStates(self, state, previous_state, memory, previous_memory):
    """Compare current state with previous state. Override in subclasses."""
    return False


# -----------------------------------------------------------------------------------------------------------------------------
class RegistersComponent(DiagramComponent):
  """Registers component"""

  def GetDetailsText(self, state, memory, previous_state=None, previous_memory=None):
    details = []
    regData = state.get("r", {})
    prevRegData = previous_state.get("r", {}) if previous_state else {}

    details.append({"type": "text", "content": "", "changed": False}) # Add a blank line

    details.append({"type": "text", "content": f"IP: {regData.get('i', 0)}",
                    "changed": previous_state and regData.get('i', 0) != prevRegData.get('i', 0)})

    details.append({"type": "text", "content": f"Stack Base: {regData.get('b', 0)}",
                    "changed": previous_state and regData.get('b', 0) != prevRegData.get('b', 0)})

    details.append({"type": "text", "content": f"Stack Size: {regData.get('s', 0)} bytes",
                    "changed": previous_state and regData.get('s', 0) != prevRegData.get('s', 0)})

    details.append({"type": "text", "content": f"Stack Pointer: {regData.get('p', 0)}",
                    "changed": previous_state and regData.get('p', 0) != prevRegData.get('p', 0)})
    details.append({"type": "text", "content": "", "changed": False}) # Add a blank line

    # Add flags as a table
    flagsNames = ["ZERO", "EQUAL", "GREATER", "EXCEPTION"]
    flags = regData.get('f', 0)
    allFlagsValue = int(flags, base=16)
    flagValues = [allFlagsValue & 0x8000, allFlagsValue & 0x4000, allFlagsValue & 0x2000, allFlagsValue & 0x0800]
    prevAllFlagsValue = prevRegData.get('f', 0) if previous_state else 0
    flagTableHeaders = ["Flag", "Value", "Changed"] # Keep "Changed" for internal logic
    flagTableRows = []
    changed = "Yes" if previous_state and flags != prevAllFlagsValue else "No"
    for i, flag in enumerate(flagsNames):
      flagTableRows.append([flag, "1" if flagValues[i] else "0", changed])

    details.append({"type": "text", "content": "Flags table:", "changed": False})
    details.append({"type": "table", "headers": flagTableHeaders, "rows": flagTableRows})

    # Add R registers as a table
    rRegs = regData.get("R", [])
    prevRRegs = prevRegData.get("R", []) if previous_state else []

    r_table_headers = ["Register", "Value", "Changed"] # Keep "Changed" for internal logic
    r_table_rows = []
    for i, val in enumerate(rRegs):
      changed = "Yes" if (previous_state and (i >= len(prevRRegs) or val != prevRRegs[i])) else "No"
      r_table_rows.append([f"R{i}", val, changed])

    if r_table_rows:
        details.append({"type": "table", "headers": r_table_headers, "rows": r_table_rows})
        details.append({"type": "text", "content": "", "changed": False}) # Add a blank line

    # Add Z registers if available as a table
    zRegs = regData.get("Z", [])
    prevZRegs = prevRegData.get("Z", []) if previous_state else []

    z_table_headers = ["Register"] + [f"Word 0x{i}" for i in range(4)] + ["Changed"] # Keep "Changed" for internal logic
    z_table_rows = []
    for i, zReg in enumerate(zRegs):
      changed = "Yes" if (previous_state and (i >= len(prevZRegs) or zReg != prevZRegs[i])) else "No"
      z_table_rows.append([f"Z{i}"] + [zReg[i] for i in range(4)] + [changed])

    if z_table_rows:
        details.append({"type": "table", "headers": z_table_headers, "rows": z_table_rows})

    return details

  # ---------------------------------------------------------------------------------------------------------------------------
  def CompareStates(self, state, previous_state, memory, previous_memory):
    if not previous_state:
      return False

    regData = state.get("r", {})
    prevRegData = previous_state.get("r", {})

    return regData != prevRegData


# -----------------------------------------------------------------------------------------------------------------------------
class StackComponent(DiagramComponent):
  """Stack component"""

  def GetDetailsText(self, state, memory, previous_state=None, previous_memory=None):
    details = []
    stack = state.get("k", [])
    prevStack = previous_state.get("k", []) if previous_state else []

    details.append({"type": "text", "content": "", "changed": False}) # Add a blank line

    if stack:
      details.append({"type": "text", "content": "Stack contents (top to bottom):", "changed": False})
      # Check if stack size changed
      if previous_state and len(stack) != len(prevStack):
        details.append({"type": "text", "content": f"Stack contents count changed from {len(prevStack)} to {len(stack)}", "changed": True})

      stack_table_headers = ["Value", "Changed"] # Keep "Changed" for internal logic
      stack_table_rows = []
      for i, val in enumerate(stack):
        changed = "Yes" if (previous_state and (i >= len(prevStack) or val != prevStack[i])) else "No"
        stack_table_rows.append([val, changed])
      stack_table_rows.reverse()
      details.append({"type": "table", "headers": stack_table_headers, "rows": stack_table_rows})

    else:
      details.append({"type": "text", "content": "Stack is empty", "changed": False})
      if previous_state and prevStack:
        details.append({"type": "text", "content": "Previously had content", "changed": True})

    return details

  # ---------------------------------------------------------------------------------------------------------------------------
  def CompareStates(self, state, previous_state, memory, previous_memory):
    if not previous_state:
      return False

    stack = state.get("k", [])
    prevStack = previous_state.get("k", [])

    return stack != prevStack


# -----------------------------------------------------------------------------------------------------------------------------
class MemoryComponent(DiagramComponent):
  """Memory component"""

  def GetDetailsText(self, state, memory, previous_state=None, previous_memory=None):
    details = []

    details.append({"type": "text", "content": "", "changed": False}) # Add a blank line

    if memory:
      details.append({"type": "text", "content": "Memory contents:", "changed": False})
      memoryTableHeaders = ["Address", "Value", "Changed"] # Keep "Changed" for internal logic
      memoryTableRows = []

      prevAddr = 0

      for addr in memory.keys():
        if int(addr, base=16) != prevAddr + 1:
          memoryTableRows.append(["...", "??" if self.topParent.GetConfig().garbage_memory else "00", "No"])
        prevAddr = int(addr, base=16)

        changed = "Yes" if (previous_memory and (addr not in previous_memory or memory[addr] != previous_memory[addr])) else "No"
        memoryTableRows.append([f"#{addr}", memory[addr].zfill(2), changed])

      if prevAddr < 0xFFFF:
        memoryTableRows.append(["...", "??" if self.topParent.GetConfig().garbage_memory else "00", "No"])

      if memoryTableRows:
          details.append({"type": "table", "headers": memoryTableHeaders, "rows": memoryTableRows})
    else:
      details.append({"type": "text", "content": "Memory data not available for this cycle", "changed": False})

    return details

  # ---------------------------------------------------------------------------------------------------------------------------
  def CompareStates(self, state, previous_state, memory, previous_memory):
    return memory != previous_memory


# -----------------------------------------------------------------------------------------------------------------------------
class ModuleComponent(DiagramComponent):
  """Generic module component (EX, DE, LS, IC)"""

  def GetDetailsText(self, state, memory, previous_state=None, previous_memory=None):
    details = []
    componentData = state.get(self.name[0], {})
    prevComponentData = previous_state.get(self.name[0], {}) if previous_state else {}

    details.append({"type": "text", "content": "", "changed": False}) # Add a blank line

    details.append({"type": "text", "content": f"State: {componentData.get('s', 'Unknown')}",
                    "changed": previous_state and componentData.get('s') != prevComponentData.get('s')})
    details.append({"type": "text", "content": "", "changed": False}) # Add a blank line

    if self.name == "IC":
      internalIP = componentData.get('i', "#0000")
      prevInternalIP = prevComponentData.get('i', "#0000")
      details.append({"type": "text", "content": f"InternalIP: {internalIP}",
                      "changed": previous_state and internalIP != prevInternalIP})

    elif self.name == "DE":
      fwStorage = componentData.get("t", {})
      prevFwStorage = prevComponentData.get("t", {})

      details.append({"type": "text", "content": f"Last Decoded Instruction: {componentData.get('l', 'N/A')}",
                      "changed": previous_state and componentData.get('l') != prevComponentData.get('l')})
      details.append({"type": "text", "content": f"Cache Start Address: {fwStorage.get('a', 0)}",
                      "changed": previous_state and fwStorage.get('a', 0) != prevFwStorage.get('a', 0)})

      details.append({"type": "text", "content": f"Stored Words Count: {fwStorage.get('c', 0)}",
                      "changed": previous_state and fwStorage.get('c', 0) != prevFwStorage.get('c', 0)})

      storedFWs = fwStorage.get("e", [])
      prevStoredFWs = prevFwStorage.get("e", [])

      if storedFWs:
          changed = "Yes" if previous_state and storedFWs != prevStoredFWs else "No"
          details.append({"type": "table",
                          "headers": ["Fetch Windows Temp Storage", "Changed"],
                          "rows": [[storedFWs, changed]]})

    elif self.name == "EX":
      substate = componentData.get('u', 'N/A')
      prevSubstate = prevComponentData.get('u', 'N/A')
      if substate != "":
        details.append({"type": "text", "content": f"Substate: {substate}",
                        "changed": previous_state and substate != prevSubstate})

      activeExcp = componentData.get('e', {})
      prevActiveExcp = prevComponentData.get('e', {})
      if activeExcp:
        details.append({"type": "text", "content": f"Active Exception: {activeExcp}",
                        "changed": previous_state and activeExcp != prevActiveExcp})

    if componentData.get('x') != "":
      details.append({"type": "text", "content": f"Extra info: {componentData.get('x', 'N/A')}",
                    "changed": previous_state and componentData.get('x') != prevComponentData.get('x')})

    return details

  # ---------------------------------------------------------------------------------------------------------------------------
  def CompareStates(self, state, previous_state, memory, previous_memory):
    if not previous_state:
      return False

    componentData = state.get(self.name[0], {})
    prevComponentData = previous_state.get(self.name[0], {})

    return componentData != prevComponentData


# -----------------------------------------------------------------------------------------------------------------------------
class CacheComponent(DiagramComponent):
  """Cache component (LS_Cache, IC_Cache)"""

  def GetDetailsText(self, state, memory, previous_state=None, previous_memory=None):
    details = []

    if self.name == "LS_Cache":
      cacheData = state.get("L", {}).get("c", {})
      prevCacheData = previous_state.get("L", {}).get("c", {}) if previous_state else {}
    else:  # IC_Cache
      cacheData = state.get("I", {}).get("c", {})
      prevCacheData = previous_state.get("I", {}).get("c", {}) if previous_state else {}

    details.append({"type": "text", "content": "", "changed": False}) # Add a blank line

    storage = cacheData.get("e", [])
    prevStorage = prevCacheData.get("e", [])
    if storage:
      storageTableHeaders = ["Entry Index", "Data", "Tag", "Valid"]
      storageTableRows = []

      if self.name == "LS_Cache":
        storageTableHeaders.extend(["Last Hit Time", "Modified"])

        for i, entry in enumerate(storage):
          for j in range(self.topParent.GetConfig().ls_cache_set_entries_count):
            changed = "Yes" if (previous_state and (i >= len(prevStorage) or entry[j] != prevStorage[i][j])) else "No"
            storageTableRows.append([
              str(i) + f".{j}",
              entry[j].get("d", "N/A"),
              entry[j].get("t", "N/A"),
              entry[j].get("v", False),
              entry[j].get("l", "N/A"),
              entry[j].get("m", False),
              changed
            ])
      else: # IC_Cache
        for i, entry in enumerate(storage):
          changed = "Yes" if (previous_state and (i >= len(prevStorage) or entry != prevStorage[i])) else "No"
          storageTableRows.append([
              str(i),
              entry.get("d", "N/A"),
              entry.get("t", "N/A"),
              entry.get("v", False),
              changed
          ])

      storageTableHeaders.append("Changed")  # Keep "Changed" for internal logic
      details.append({"type": "table", "headers": storageTableHeaders, "rows": storageTableRows})

    return details

  # ---------------------------------------------------------------------------------------------------------------------------
  def CompareStates(self, state, previous_state, memory, previous_memory):
    if not previous_state:
      return False

    if self.name == "LS_Cache":
      cacheData = state.get("L", {}).get("c", {})
      prevCacheData = previous_state.get("L", {}).get("c", {})
    else:  # IC_Cache
      cacheData = state.get("I", {}).get("c", {})
      prevCacheData = previous_state.get("I", {}).get("c", {})

    return cacheData != prevCacheData


# -----------------------------------------------------------------------------------------------------------------------------
class PipelineComponent(DiagramComponent):
  """Pipeline component (EX_to_DE, DE_to_EX, etc.)"""

  def GetDetailsText(self, state, memory, previous_state=None, previous_memory=None):
    details = []

    pipes = state.get("P", {})
    prevPipes = previous_state.get("P", {}) if previous_state else {}
    pipeName = self.name[0] + self.name[6]
    pipeData = pipes.get(pipeName, [])
    prevPipeData = prevPipes.get(pipeName, [])

    details.append({"type": "text", "content": "", "changed": False}) # Add a blank line

    if pipeData:
      details.append({"type": "text", "content": f"Pipeline entries: {len(pipeData)}",
                      "changed": previous_state and len(pipeData) != len(prevPipeData)})

      pipeline_table_headers = ["Data", "Sent At Cycle", "Associated IP", "Exception Triggered", "Changed"] # Keep "Changed" for internal logic
      pipeline_table_rows = []

      for i, entry in enumerate(pipeData):
          changed = "No"
          if previous_state and (i >= len(prevPipeData) or entry != prevPipeData[i]):
              changed = "Yes"

          # Assuming the entry dictionary has these keys
          exception_triggered = entry.get("e", "N/A")
          if not exception_triggered:
            data_val = entry.get("d", "N/A")
          else:
            data_val = f"Exception:\n{entry.get('x', 'Unknown')}\nHandler addr:\n{entry.get('h', 'N/A')}"
          sent_at = entry.get("s", "N/A")
          associated_ip = entry.get("a", "N/A")

          pipeline_table_rows.append([data_val, sent_at, associated_ip, exception_triggered, changed])

      if pipeline_table_rows:
          details.append({"type": "table", "headers": pipeline_table_headers, "rows": pipeline_table_rows})
    else:
      details.append({"type": "text", "content": "Pipeline is empty", "changed": False})
      if previous_state and prevPipeData:
        details.append({"type": "text", "content": "Previously had content", "changed": True})

    return details

  # ---------------------------------------------------------------------------------------------------------------------------
  def CompareStates(self, state, previous_state, memory, previous_memory):
    if not previous_state:
      return False

    pipes = state.get("P", {})
    prevPipes = previous_state.get("P", {})
    pipeName = self.name[0] + self.name[6]
    pipeData = pipes.get(pipeName, [])
    prevPipeData = prevPipes.get(pipeName, [])

    return pipeData != prevPipeData


# -----------------------------------------------------------------------------------------------------------------------------
class SpecComponent(DiagramComponent):
  details = []

  # ---------------------------------------------------------------------------------------------------------------------------
  def SetDetails(self, config: Config):
    self.details = []
    self.details.append({"type": "text", "content": "", "changed": False})  # Add a blank line
    self.details.append({"type": "text", "content": "CPU Configuration used for this simulation", "changed": False})
    self.details.append({"type": "text", "content": "", "changed": False})
    self.details.append({"type": "text", "content": f"Clock Period: {config.clock_period_millis} ms", "changed": False})
    self.details.append({"type": "text", "content": "", "changed": False})
    self.details.append({"type": "text", "content": f"Ignore Uninitialized Memory: {'Yes' if config.ignore_uninitialized_mem else 'No'}", "changed": False})
    self.details.append({"type": "text", "content": f"Garbage Memory: {'Yes' if config.garbage_memory else 'No'}", "changed": False})
    self.details.append({"type": "text", "content": "", "changed": False})
    self.details.append({"type": "text", "content": f"IC Cycles per Op: {config.ic_cycles_per_op}", "changed": False})
    self.details.append({"type": "text", "content": f"IC Cycles per Op with Cache Hit: {config.ic_cycles_per_op_with_cache_hit}", "changed": False})
    self.details.append({"type": "text", "content": f"LS Cycles per Op: {config.ls_cycles_per_op}", "changed": False})
    self.details.append({"type": "text", "content": f"LS Cycles per Op with Cache Hit: {config.ls_cycles_per_op_with_cache_hit}", "changed": False})
    self.details.append({"type": "text", "content": f"DE Cycles per Op: {config.de_cycles_per_op}", "changed": False})
    self.details.append({"type": "text", "content": f"EX Cycles per Op: {config.ex_cycles_per_op}", "changed": False})
    self.details.append({"type": "text", "content": "", "changed": False})
    self.details.append({"type": "text", "content": f"IC Cache Size: {config.ic_cache_words_size} words", "changed": False})
    self.details.append({"type": "text", "content": f"LS Cache Size: {config.ls_cache_words_size} words", "changed": False})
    self.details.append({"type": "text", "content": f"Entries per LS Cache Set: {config.ls_cache_set_entries_count}", "changed": False})

  # ---------------------------------------------------------------------------------------------------------------------------
  def GetDetailsText(self, _state, _memory, _previous_state=None, _previous_memory=None):
    return self.details


# -----------------------------------------------------------------------------------------------------------------------------
def CreateComponent(top_parent, name, x, y, width, height, scene):
  """Factory function to create the appropriate component type"""
  if name == "Registers":
    return RegistersComponent(top_parent, name, x, y, width, height, scene)
  elif name == "Stack":
    return StackComponent(top_parent, name, x, y, width, height, scene)
  elif name == "Memory":
    return MemoryComponent(top_parent, name, x, y, width, height, scene)
  elif name.endswith("Cache"):
    return CacheComponent(top_parent, name, x, y, width, height, scene)
  elif "to" in name:
    return PipelineComponent(top_parent, name, x, y, width, height, scene)
  elif name in ["EX", "DE", "LS", "IC"]:
    return ModuleComponent(top_parent, name, x, y, width, height, scene)
  else:
    return DiagramComponent(top_parent, name, x, y, width, height, scene)


# -----------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
  raise Exception("This module is not meant to be run directly.")