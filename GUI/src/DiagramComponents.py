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
  
  def __init__(self, name, x, y, width, height, scene):
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
  def GetDetailsText(self, state, memory, previous_state=None, previous_memory=None):
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
    regData = state.get("registers", {})
    prevRegData = previous_state.get("registers", {}) if previous_state else {}
    
    details.append({"type": "text", "content": "", "changed": False}) # Add a blank line

    details.append({"type": "text", "content": f"IP: {regData.get('IP', 0)}", 
                    "changed": previous_state and regData.get('IP', 0) != prevRegData.get('IP', 0)})
    
    details.append({"type": "text", "content": f"Stack Base: {regData.get('stackBase', 0)}", 
                    "changed": previous_state and regData.get('stackBase', 0) != prevRegData.get('stackBase', 0)})
    
    details.append({"type": "text", "content": f"Stack Size: {regData.get('stackSize', 0)} bytes",
                    "changed": previous_state and regData.get('stackSize', 0) != prevRegData.get('stackSize', 0)})
    
    details.append({"type": "text", "content": f"Stack Pointer: {regData.get('stackPointer', 0)}", 
                    "changed": previous_state and regData.get('stackPointer', 0) != prevRegData.get('stackPointer', 0)})
    details.append({"type": "text", "content": "", "changed": False}) # Add a blank line

    # Add flags as a table
    flagsNames = ["ZERO", "EQUAL", "GREATER", "EXCEPTION"]
    flags = regData.get('flags', 0)
    allFlagsValue = int(flags, base=16)
    flagValues = [allFlagsValue & 0x8000, allFlagsValue & 0x4000, allFlagsValue & 0x2000, allFlagsValue & 0x0800]
    prevAllFlagsValue = prevRegData.get('flags', 0) if previous_state else 0
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
    
    regData = state.get("registers", {})
    prevRegData = previous_state.get("registers", {})
    
    return regData != prevRegData


# -----------------------------------------------------------------------------------------------------------------------------
class StackComponent(DiagramComponent):
  """Stack component"""
  
  def GetDetailsText(self, state, memory, previous_state=None, previous_memory=None):
    details = []
    stack = state.get("stack", [])
    prevStack = previous_state.get("stack", []) if previous_state else []
    
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
    
    stack = state.get("stack", [])
    prevStack = previous_state.get("stack", [])
    
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
          memoryTableRows.append(["...", "00", "No"])
        prevAddr = int(addr, base=16)

        changed = "Yes" if (previous_memory and (addr not in previous_memory or memory[addr] != previous_memory[addr])) else "No"
        memoryTableRows.append([f"#{addr}", memory[addr].zfill(2), changed])

      if prevAddr < 0xFFFF:
        memoryTableRows.append(["...", "00", "No"])

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
    componentData = state.get(self.name, {})
    prevComponentData = previous_state.get(self.name, {}) if previous_state else {}
    
    details.append({"type": "text", "content": "", "changed": False}) # Add a blank line

    details.append({"type": "text", "content": f"State: {componentData.get('state', 'Unknown')}", 
                    "changed": previous_state and componentData.get('state') != prevComponentData.get('state')})
    details.append({"type": "text", "content": "", "changed": False}) # Add a blank line
    
    if self.name == "IC":
      internalIP = componentData.get('internalIP', 0)
      prevInternalIP = prevComponentData.get('internalIP', 0)
      details.append({"type": "text", "content": f"InternalIP: {internalIP}", 
                      "changed": previous_state and internalIP != prevInternalIP})
    
    elif self.name == "DE":
      fwStorage = componentData.get("fwTempStorage", {})
      prevFwStorage = prevComponentData.get("fwTempStorage", {})
      
      details.append({"type": "text", "content": f"Last Decoded Instruction: {componentData.get('lastDecodedInstr', 'N/A')}",
                      "changed": previous_state and componentData.get('lastDecodedInstr') != prevComponentData.get('lastDecodedInstr')})
      details.append({"type": "text", "content": f"Cache Start Address: {fwStorage.get('cacheStartAddr', 0)}", 
                      "changed": previous_state and fwStorage.get('cacheStartAddr', 0) != prevFwStorage.get('cacheStartAddr', 0)})
      
      details.append({"type": "text", "content": f"Stored Words Count: {fwStorage.get('storedWordsCount', 0)}", 
                      "changed": previous_state and fwStorage.get('storedWordsCount', 0) != prevFwStorage.get('storedWordsCount', 0)})
      
      storedFWs = fwStorage.get("storedFWs", [])
      prevStoredFWs = prevFwStorage.get("storedFWs", [])
      
      if storedFWs:
          changed = "Yes" if previous_state and storedFWs != prevStoredFWs else "No"
          details.append({"type": "table",
                          "headers": ["Fetch Windows Temp Storage", "Changed"],
                          "rows": [[storedFWs, changed]]})
    
    elif self.name == "EX":
      substate = componentData.get('substate', 'N/A')
      prevSubstate = prevComponentData.get('substate', 'N/A')
      if substate != "":
        details.append({"type": "text", "content": f"Substate: {substate}",
                        "changed": previous_state and substate != prevSubstate})

    if componentData.get('extra') != "":
      details.append({"type": "text", "content": f"Extra info: {componentData.get('extra', 'N/A')}",
                    "changed": previous_state and componentData.get('extra') != prevComponentData.get('extra')})

    return details
  
  # ---------------------------------------------------------------------------------------------------------------------------
  def CompareStates(self, state, previous_state, memory, previous_memory):
    if not previous_state:
      return False
    
    componentData = state.get(self.name, {})
    prevComponentData = previous_state.get(self.name, {})
    
    return componentData != prevComponentData


# -----------------------------------------------------------------------------------------------------------------------------
class CacheComponent(DiagramComponent):
  """Cache component (LS_Cache, IC_Cache)"""
  
  def GetDetailsText(self, state, memory, previous_state=None, previous_memory=None):
    details = []
    
    if self.name == "LS_Cache":
      cacheData = state.get("LS", {}).get("cache", {})
      prevCacheData = previous_state.get("LS", {}).get("cache", {}) if previous_state else {}
    else:  # IC_Cache
      cacheData = state.get("IC", {}).get("cache", {})
      prevCacheData = previous_state.get("IC", {}).get("cache", {}) if previous_state else {}
    
    details.append({"type": "text", "content": "", "changed": False}) # Add a blank line
    
    storage = cacheData.get("storage", [])
    prevStorage = prevCacheData.get("storage", [])
    if storage:
      storageTableHeaders = ["Entry Index", "Data", "Tag", "Valid"]
      storageTableRows = []

      if self.name == "LS_Cache":
        storageTableHeaders.extend(["Last Hit Time", "Modified"])

        for i, entry in enumerate(storage):
          for j in range(2):  # Two entries per cache line
            changed = "Yes" if (previous_state and (i >= len(prevStorage) or entry[j] != prevStorage[i][j])) else "No"
            storageTableRows.append([
              str(i) + f".{j}",
              entry[j].get("data", "N/A"),
              entry[j].get("tag", "N/A"),
              entry[j].get("valid", False),
              entry[j].get("lastHitTime", "N/A"),
              entry[j].get("modified", False),
              changed
            ])
      else: # IC_Cache
        for i, entry in enumerate(storage):
          changed = "Yes" if (previous_state and (i >= len(prevStorage) or entry != prevStorage[i])) else "No"
          storageTableRows.append([
              str(i),
              entry.get("data", "N/A"),
              entry.get("tag", "N/A"),
              entry.get("valid", False),
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
      cacheData = state.get("LS", {}).get("cache", {})
      prevCacheData = previous_state.get("LS", {}).get("cache", {})
    else:  # IC_Cache
      cacheData = state.get("IC", {}).get("cache", {})
      prevCacheData = previous_state.get("IC", {}).get("cache", {})
    
    return cacheData != prevCacheData


# -----------------------------------------------------------------------------------------------------------------------------
class PipelineComponent(DiagramComponent):
  """Pipeline component (EX_to_DE, DE_to_EX, etc.)"""
  
  def GetDetailsText(self, state, memory, previous_state=None, previous_memory=None):
    details = []
    
    pipes = state.get("pipes", {})
    prevPipes = previous_state.get("pipes", {}) if previous_state else {}
    pipeName = self.name.replace("_", "")
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
          exception_triggered = entry.get("exceptionTriggered", "N/A")
          if not exception_triggered:
            data_val = entry.get("data", "N/A")
          else:
            data_val = f"Exception:\n{entry.get('excpData', 'Unknown')}\nHandler addr:\n{entry.get('handlerAddr', 'N/A')}"
          sent_at = entry.get("sentAt", "N/A")
          associated_ip = entry.get("associatedIP", "N/A")
          
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
    
    pipes = state.get("pipes", {})
    prevPipes = previous_state.get("pipes", {})
    pipeName = self.name.replace("_", "")
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
    self.details.append({"type": "text", "content": "", "changed": False})  # Add a blank line
    self.details.append({"type": "text", "content": f"Clock Period: {config.clock_period_millis} ms", "changed": False})
    self.details.append({"type": "text", "content": f"IC Cycles per Op: {config.ic_cycles_per_op}", "changed": False})
    self.details.append({"type": "text", "content": f"IC Cycles per Op with Cache Hit: {config.ic_cycles_per_op_with_cache_hit}", "changed": False})
    self.details.append({"type": "text", "content": f"LS Cycles per Op: {config.ls_cycles_per_op}", "changed": False})
    self.details.append({"type": "text", "content": f"LS Cycles per Op with Cache Hit: {config.ls_cycles_per_op_with_cache_hit}", "changed": False})
    self.details.append({"type": "text", "content": f"DE Cycles per Op: {config.de_cycles_per_op}", "changed": False})
    self.details.append({"type": "text", "content": f"EX Cycles per Op: {config.ex_cycles_per_op}", "changed": False})

  # ---------------------------------------------------------------------------------------------------------------------------
  def GetDetailsText(self, _state, _memory, _previous_state=None, _previous_memory=None):
    return self.details


# -----------------------------------------------------------------------------------------------------------------------------
def CreateComponent(name, x, y, width, height, scene):
  """Factory function to create the appropriate component type"""
  if name == "Registers":
    return RegistersComponent(name, x, y, width, height, scene)
  elif name == "Stack":
    return StackComponent(name, x, y, width, height, scene)
  elif name == "Memory":
    return MemoryComponent(name, x, y, width, height, scene)
  elif name.endswith("Cache"):
    return CacheComponent(name, x, y, width, height, scene)
  elif "to" in name:
    return PipelineComponent(name, x, y, width, height, scene)
  elif name in ["EX", "DE", "LS", "IC"]:
    return ModuleComponent(name, x, y, width, height, scene)
  else:
    return DiagramComponent(name, x, y, width, height, scene)


# -----------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
  raise Exception("This module is not meant to be run directly.")