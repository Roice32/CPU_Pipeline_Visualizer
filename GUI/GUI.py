import sys
import os
import json
import shutil
from PyQt5.QtWidgets import (QApplication, QMainWindow, QTabWidget, QWidget, QVBoxLayout, 
                            QHBoxLayout, QLabel, QTextEdit, QPushButton, QFileDialog, 
                            QSpinBox, QFormLayout, QSplitter, QMessageBox, QSlider,
                            QGraphicsScene, QGraphicsView, QGraphicsRectItem, QGraphicsTextItem)
from PyQt5.QtCore import Qt, QProcess, QRectF, QEvent
from PyQt5.QtGui import QFont, QPainter, QPen, QColor, QBrush

class CPUSimulator(QMainWindow):
    def __init__(self):
        super().__init__()
        
        # Create temp directory
        self.temp_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "CPU_PV.temp")
        if not os.path.exists(self.temp_dir):
            os.makedirs(self.temp_dir)
        
        # Initialize UI
        self.setWindowTitle("CPU Simulator")
        self.setGeometry(100, 100, 1000, 600)
        
        # Initialize config values
        self.clock_period = 1
        self.de_work_memory = 2
        self.ic_cache_size = 64
        self.ls_cache_size = 64
        self.ex_cycles_per_op = 5
        
        # Create tab widget
        self.tabs = QTabWidget()
        self.setCentralWidget(self.tabs)
        
        # Create tabs
        self.input_tab = QWidget()
        self.config_tab = QWidget()
        self.simulation_tab = QWidget()
        self.help_tab = QWidget()
        
        # Add tabs to widget
        self.tabs.addTab(self.input_tab, "Input")
        self.tabs.addTab(self.config_tab, "Config")
        self.tabs.addTab(self.simulation_tab, "Simulation")
        self.tabs.addTab(self.help_tab, "Help")
        
        # Set up each tab
        self.setup_input_tab()
        self.setup_config_tab()
        self.setup_simulation_tab()
        self.setup_help_tab()
    
    def setup_input_tab(self):
        # Main layout
        main_layout = QVBoxLayout()
        
        # Create splitter for resizing
        splitter = QSplitter(Qt.Horizontal)
        
        # Left side - ASM input
        left_widget = QWidget()
        left_layout = QVBoxLayout()
        left_layout.addWidget(QLabel("input.asm"))
        
        self.asm_text = QTextEdit()
        font = QFont("Courier New", 10)
        self.asm_text.setFont(font)
        left_layout.addWidget(self.asm_text)
        
        open_asm_btn = QPushButton("Open")
        open_asm_btn.clicked.connect(self.open_asm_file)
        left_layout.addWidget(open_asm_btn)
        
        left_widget.setLayout(left_layout)
        splitter.addWidget(left_widget)
        
        # Middle buttons
        middle_widget = QWidget()
        middle_layout = QVBoxLayout()
        middle_layout.addStretch()
        
        convert_btn = QPushButton("→")
        convert_btn.setFixedWidth(50)
        convert_btn.clicked.connect(self.convert_asm_to_hex)
        middle_layout.addWidget(convert_btn)
        
        execute_btn = QPushButton("Execute")
        execute_btn.clicked.connect(self.execute_simulation)
        middle_layout.addWidget(execute_btn)
        
        middle_layout.addStretch()
        middle_widget.setLayout(middle_layout)
        splitter.addWidget(middle_widget)
        
        # Right side - HEX input
        right_widget = QWidget()
        right_layout = QVBoxLayout()
        right_layout.addWidget(QLabel("input.hex"))
        
        self.hex_text = QTextEdit()
        self.hex_text.setFont(font)
        right_layout.addWidget(self.hex_text)
        
        open_hex_btn = QPushButton("Open")
        open_hex_btn.clicked.connect(self.open_hex_file)
        right_layout.addWidget(open_hex_btn)
        
        right_widget.setLayout(right_layout)
        splitter.addWidget(right_widget)
        
        # Set stretch factors for splitter
        splitter.setStretchFactor(0, 1)
        splitter.setStretchFactor(1, 0)
        splitter.setStretchFactor(2, 1)
        
        main_layout.addWidget(splitter)
        self.input_tab.setLayout(main_layout)
    
    def setup_config_tab(self):
        layout = QFormLayout()
        
        # CLOCK_PERIOD_MILLIS
        self.clock_period_input = QSpinBox()
        self.clock_period_input.setRange(1, 100)
        self.clock_period_input.setValue(self.clock_period)
        self.clock_period_input.valueChanged.connect(self.update_clock_period)
        layout.addRow("CLOCK_PERIOD_MILLIS:", self.clock_period_input)
        
        # DE_WORK_MEMORY_FW_SIZE
        self.de_work_memory_input = QSpinBox()
        self.de_work_memory_input.setRange(2, 5)
        self.de_work_memory_input.setValue(self.de_work_memory)
        self.de_work_memory_input.valueChanged.connect(self.update_de_work_memory)
        layout.addRow("DE_WORK_MEMORY_FW_SIZE:", self.de_work_memory_input)
        
        # IC_CACHE_WORDS_SIZE
        self.ic_cache_input = QSpinBox()
        self.ic_cache_input.setRange(16, 1024)
        self.ic_cache_input.setValue(self.ic_cache_size)
        self.ic_cache_input.valueChanged.connect(self.update_ic_cache_size)
        layout.addRow("IC_CACHE_WORDS_SIZE:", self.ic_cache_input)
        
        # LS_CACHE_WORDS_SIZE
        self.ls_cache_input = QSpinBox()
        self.ls_cache_input.setRange(16, 1024)
        self.ls_cache_input.setValue(self.ls_cache_size)
        self.ls_cache_input.valueChanged.connect(self.update_ls_cache_size)
        layout.addRow("LS_CACHE_WORDS_SIZE:", self.ls_cache_input)
        
        # EX_CYCLES_PER_OP
        self.ex_cycles_input = QSpinBox()
        self.ex_cycles_input.setRange(4, 100)
        self.ex_cycles_input.setValue(self.ex_cycles_per_op)
        self.ex_cycles_input.valueChanged.connect(self.update_ex_cycles_per_op)
        layout.addRow("EX_CYCLES_PER_OP:", self.ex_cycles_input)
        
        self.config_tab.setLayout(layout)
    
    def setup_simulation_tab(self):
        layout = QVBoxLayout()
        
        # Add slider for cycle selection
        slider_layout = QHBoxLayout()
        slider_layout.addWidget(QLabel("Cycle:"))
        self.cycle_slider = QSlider(Qt.Horizontal)
        self.cycle_slider.setMinimum(1)
        self.cycle_slider.setMaximum(1)  # Will be updated when data is loaded
        self.cycle_slider.valueChanged.connect(self.update_simulation_view)
        slider_layout.addWidget(self.cycle_slider)
        self.cycle_label = QLabel("1")
        slider_layout.addWidget(self.cycle_label)
        layout.addLayout(slider_layout)
        
        # CPU diagram view
        self.diagram_scene = QGraphicsScene()
        self.diagram_view = QGraphicsView(self.diagram_scene)
        self.diagram_view.setRenderHint(QPainter.Antialiasing)
        self.diagram_view.setMinimumHeight(500)
        self.diagram_view.viewport().installEventFilter(self)
        layout.addWidget(self.diagram_view)
        
        # Details text box
        layout.addWidget(QLabel("Component Details:"))
        self.details_text = QTextEdit()
        self.details_text.setReadOnly(True)
        layout.addWidget(self.details_text)
        
        # Create the CPU diagram components
        self.create_cpu_diagram()
        
        self.simulation_tab.setLayout(layout)
        
        # Storage for execution states
        self.execution_states = []
        self.selected_component = None
    
    def setup_help_tab(self):
        layout = QVBoxLayout()
        help_text = QTextEdit()
        help_text.setReadOnly(True)
        help_text.setText("""Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed euismod, sapien vel bibendum 
lacinia, nisl nunc ultrices nunc, vitae aliquam nisl nunc eget nunc. Donec auctor, nisl eget
aliquam tincidunt, nisl nunc ultrices nunc, vitae aliquam nisl nunc eget nunc. Donec auctor,
nisl eget aliquam tincidunt, nisl nunc ultrices nunc, vitae aliquam nisl nunc eget nunc.

Nulla facilisi. Donec auctor, nisl eget aliquam tincidunt, nisl nunc ultrices nunc, vitae 
aliquam nisl nunc eget nunc. Donec auctor, nisl eget aliquam tincidunt, nisl nunc ultrices 
nunc, vitae aliquam nisl nunc eget nunc.

Donec auctor, nisl eget aliquam tincidunt, nisl nunc ultrices nunc, vitae aliquam nisl nunc 
eget nunc. Donec auctor, nisl eget aliquam tincidunt, nisl nunc ultrices nunc, vitae aliquam
nisl nunc eget nunc.""")
        layout.addWidget(help_text)
        self.help_tab.setLayout(layout)
    
    def open_asm_file(self):
        options = QFileDialog.Options()
        filename, _ = QFileDialog.getOpenFileName(self, "Open ASM File", "", "Assembly Files (*.asm);;All Files (*)", options=options)
        if filename:
            with open(filename, 'r') as file:
                self.asm_text.setText(file.read())
                
    def open_hex_file(self):
        options = QFileDialog.Options()
        filename, _ = QFileDialog.getOpenFileName(self, "Open HEX File", "", "Hex Files (*.hex);;All Files (*)", options=options)
        if filename:
            with open(filename, 'r') as file:
                self.hex_text.setText(file.read())
    
    def convert_asm_to_hex(self):
        # Save ASM to temp file
        asm_path = os.path.join(self.temp_dir, "input.asm")
        with open(asm_path, 'w') as file:
            file.write(self.asm_text.toPlainText())

        hex_path = os.path.join(self.temp_dir, "input.hex")
        parse_script = "./pyasm.py"
        
        try:
            # Run parsing script
            process = QProcess()
            process.start("python", [parse_script, "--infile", asm_path, "--outfile", hex_path])
            process.waitForFinished()
            
            # Load the generated hex file
            if os.path.exists(hex_path):
                with open(hex_path, 'r') as file:
                    self.hex_text.setText(file.read())
            else:
                QMessageBox.warning(self, "Warning", "Conversion did not produce input.hex file.")
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Error converting ASM to HEX: {str(e)}")
    
    def execute_simulation(self):
        # Save HEX to temp file
        hex_path = os.path.join(self.temp_dir, "input.hex")
        with open(hex_path, 'w') as file:
            file.write(self.hex_text.toPlainText())
        
        # Try to load states.json file
        states_path = os.path.join(self.temp_dir, "states.json")
        if os.path.exists(states_path):
            try:
                with open(states_path, 'r') as file:
                    self.execution_states = json.load(file)
                
                # Update slider maximum value based on the last cycle
                if self.execution_states:
                    max_cycle = int(self.execution_states[-1].get("cycle", 1))
                    self.cycle_slider.setMaximum(max_cycle)
                    self.cycle_slider.setValue(1)  # Start at the first cycle
                    self.cycle_label.setText(str(1))
                    self.update_simulation_view()
            except Exception as e:
                QMessageBox.warning(self, "Warning", f"Error loading simulation states: {str(e)}")
        else:
            QMessageBox.information(self, "Information", "No simulation states file found. Please make sure 'states.json' exists in the CPU_PV.temp directory.")
        
        # Switch to simulation tab
        self.tabs.setCurrentIndex(2)  # Index 2 is the Simulation tab
    
    def update_clock_period(self, value):
        self.clock_period = value
        
    def update_de_work_memory(self, value):
        self.de_work_memory = value
    
    def update_ic_cache_size(self, value):
        self.ic_cache_size = value
    
    def update_ls_cache_size(self, value):
        self.ls_cache_size = value
    
    def update_ex_cycles_per_op(self, value):
        self.ex_cycles_per_op = value
        
    def create_cpu_diagram(self):
        # Clear previous scene
        self.diagram_scene.clear()
        self.component_items = {}
        
        # Define colors
        border_color = QColor(0, 0, 0)
        fill_color = QColor(255, 255, 255)
        
        # Create components based on the diagram
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
            # Connection pipes
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
            rect.setPen(QPen(border_color, 2))
            rect.setBrush(QBrush(fill_color))
            rect.setData(0, name)  # Store the component name
            
            # Add text label
            text = QGraphicsTextItem(name.replace("_", " "))
            text.setPos(x + w/2 - text.boundingRect().width()/2, y + h/2 - text.boundingRect().height()/2)
            
            # Store the reference to the rectangle
            self.component_items[name] = rect
            
            # Add to scene
            self.diagram_scene.addItem(rect)
            self.diagram_scene.addItem(text)
        
        # Set up hover and click events for components
        for item in self.diagram_scene.items():
            if isinstance(item, QGraphicsRectItem):
                item.setAcceptHoverEvents(True)
        
        # Set the scene rect to fit all components
        self.diagram_scene.setSceneRect(self.diagram_scene.itemsBoundingRect())
        
    def update_simulation_view(self):
        current_cycle = self.cycle_slider.value()
        self.cycle_label.setText(str(current_cycle))
        
        # Reset all component highlighting
        for item in self.component_items.values():
            item.setPen(QPen(QColor(0, 0, 0), 2))
        
        # If a component is selected, update its highlighting and show details
        if self.selected_component and self.execution_states:
            # Find the state that matches current cycle
            state = None
            for s in self.execution_states:
                if s.get("cycle", 0) == current_cycle:
                    state = s
                    break
                    
            if state:
                self.component_items[self.selected_component].setPen(QPen(QColor(0, 0, 255), 3))
                details = self.get_component_details(self.selected_component, state)
                self.details_text.setText(details)
    
    def get_component_details(self, component, state):
        details = f"Component: {component}\nCycle: {state.get('cycle', 0)}\n\n"
        
        # Extract and format details based on component type
        if component == "Registers":
            reg_data = state.get("registers", {})
            details += f"IP: {hex(reg_data.get('IP', 0))}\n"
            details += f"Flags: {hex(reg_data.get('flags', 0))}\n"
            details += f"Stack Base: {hex(reg_data.get('stackBase', 0))}\n"
            details += f"Stack Size: {reg_data.get('stackSize', 0)}\n"
            details += f"Stack Pointer: {reg_data.get('stackPointer', 0)}\n\n"
            
            # Add R registers
            r_regs = reg_data.get("R", [])
            for i, val in enumerate(r_regs):
                details += f"R{i}: {hex(val)}\n"
            
            # Add Z registers if available
            z_regs = reg_data.get("Z", [])
            for i, z_reg in enumerate(z_regs):
                details += f"Z{i}: {[hex(v) for v in z_reg]}\n"
                
        elif component == "Stack":
            # Get stack data
            stack = state.get("stack", [])
            if stack:
                details += "Stack contents (top to bottom):\n"
                for i, val in enumerate(stack):
                    details += f"{i}: {hex(val)}\n"
            else:
                details += "Stack is empty\n"
                
        elif component == "Memory":
            # Get memory data
            memory = state.get("memory", {})
            if memory:
                details += "Memory contents:\n"
                for addr, val in memory.items():
                    details += f"Address {hex(int(addr))}: {hex(val)}\n"
            else:
                details += "Memory is empty\n"
                
        elif component in ["EX", "DE", "LS", "IC"]:
            component_data = state.get(component, {})
            details += f"State: {component_data.get('state', 'Unknown')}\n\n"
            
            # Add component-specific details
            if component == "LS" or component == "IC":
                cache_data = component_data.get("cache", {})
                details += f"Cache Size: {cache_data.get('size', 0)}\n"
                details += f"Current Request Index: {cache_data.get('currReqIndex', 0)}\n"
                details += f"Current Request Tag: {hex(cache_data.get('currReqTag', 0))}\n"
                
                if component == "LS":
                    details += f"Physical Memory Access: {'Yes' if component_data.get('physicalMemoryAccessHappened', False) else 'No'}\n"
                    details += f"Found Index: {cache_data.get('foundIndex', 0)}\n"
                    
            elif component == "DE":
                fw_storage = component_data.get("fwTempStorage", {})
                details += f"Cache Start Address: {hex(fw_storage.get('cacheStartAddr', 0))}\n"
                details += f"Stored Words Count: {fw_storage.get('storedWordsCount', 0)}\n"
                
                # Show stored fetch windows if available
                stored_fws = fw_storage.get("storedFWs", [])
                for i, fw in enumerate(stored_fws):
                    details += f"FW {i}: {fw}\n"
                    
        elif component.endswith("Cache"):
            if component == "LS_Cache":
                cache_data = state.get("LS", {}).get("cache", {})
            else:  # IC_Cache
                cache_data = state.get("IC", {}).get("cache", {})
                
            details += f"Cache Size: {cache_data.get('size', 0)}\n"
            
            # Add storage data if available
            storage = cache_data.get("storage", [])
            if storage:
                details += f"Storage entries: {len(storage)}\n"
                # Just show a sample to avoid overwhelming the display
                for i, entry in enumerate(storage[:5]):
                    details += f"Entry {i}: {entry}\n"
                if len(storage) > 5:
                    details += "...\n"
                    
        elif "to" in component:
            # For pipeline components
            pipes = state.get("pipes", {})
            pipe_name = component.replace("_", "")
            pipe_data = pipes.get(pipe_name, [])
            
            if pipe_data:
                details += f"Pipeline entries: {len(pipe_data)}\n"
                for i, entry in enumerate(pipe_data):
                    details += f"Entry {i}: {entry}\n"
            else:
                details += "Pipeline is empty\n"
        
        return details
        
    def eventFilter(self, obj, event):
        if obj == self.diagram_view.viewport():
            if event.type() == QEvent.MouseMove:
                pos = self.diagram_view.mapToScene(event.pos())
                item = self.diagram_scene.itemAt(pos, self.diagram_view.transform())
                
                # Check if it's a component rectangle
                if isinstance(item, QGraphicsRectItem):
                    # Add hover highlighting
                    component_name = item.data(0)
                    if component_name:
                        for name, rect in self.component_items.items():
                            if name != self.selected_component:
                                if name == component_name:
                                    rect.setBrush(QBrush(QColor(220, 220, 220)))  # Light gray
                                else:
                                    rect.setBrush(QBrush(QColor(255, 255, 255)))  # White
                
            elif event.type() == QEvent.MouseButtonPress:
                pos = self.diagram_view.mapToScene(event.pos())
                item = self.diagram_scene.itemAt(pos, self.diagram_view.transform())
                
                # Check if it's a component rectangle
                if isinstance(item, QGraphicsRectItem):
                    component_name = item.data(0)

                    if component_name:
                        self.selected_component = component_name
                        self.update_simulation_view()
        
        return super().eventFilter(obj, event)
    
    def closeEvent(self, event):
        # Clean up temp directory on exit
        if os.path.exists(self.temp_dir):
            try:
                shutil.rmtree(self.temp_dir)
            except Exception as e:
                print(f"Error removing temp directory: {str(e)}")
        event.accept()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = CPUSimulator()
    window.show()
    sys.exit(app.exec_())