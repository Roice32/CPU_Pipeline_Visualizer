from PyQt5.QtWidgets import QWidget, QVBoxLayout, QLabel, QTextEdit
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QFont
from PyQt5.QtWidgets import QScrollArea, QWidget
from PyQt5.QtWidgets import QTableWidget, QTableWidgetItem, QHeaderView


# -----------------------------------------------------------------------------------------------------------------------------
class HelpTab(QWidget):
  parent = None

  # ---------------------------------------------------------------------------------------------------------------------------
  def __init__(self, parent):
    super().__init__()
    self.parent = parent

    # Create main layout for the HelpTab
    mainLayout = QVBoxLayout(self)
    
    # Create scroll area
    scrollArea = QScrollArea(self)
    scrollArea.setWidgetResizable(True)
    scrollArea.setVerticalScrollBarPolicy(Qt.ScrollBarAsNeeded)
    scrollArea.setHorizontalScrollBarPolicy(Qt.ScrollBarAsNeeded)

    # Create content widget that will contain all the content
    contentWidget = QWidget()
    layout = QVBoxLayout(contentWidget)
    layout.setContentsMargins(10, 10, 10, 10)  # Add some padding
    layout.setSpacing(10)  # Add spacing between elements

    titleFont = QFont()
    titleFont.setFamily("Arial")
    titleFont.setPointSize(16)
    titleFont.setBold(True)

    self.chapterFont = QFont()
    self.chapterFont.setFamily("Arial")
    self.chapterFont.setPointSize(14)
    self.chapterFont.setBold(True)
    self.chapterFont.setUnderline(True)

    self.contentFont = QFont()
    self.contentFont.setFamily("Arial")
    self.contentFont.setPointSize(12)

    title = QLabel("CPU Pipeline Visualizer")
    title.setFont(titleFont)
    title.setAlignment(Qt.AlignHCenter | Qt.AlignTop)
    layout.addWidget(title)

    self.GenTextSection(
      layout,
      "What is CPU Pipeline Visualizer?",
      "\tCPU Pipeline Visualizer is a tool designed to help users understand and visualize the inner workings of a CPU pipeline.\n"
      "It provides graphical representations and explanations of how instructions move through different stages of the pipeline, "
      "from raw memory to being retired, making it easier to learn about pipelining concepts and CPU architecture.\n"

      "\tFurthermore, it allows configuring the CPU's architecture, such as clock speed, caches' sizes, "
      "modules' cycles count per operation, allowing users to observe how these changes affect the CPU's performance, for either "
      "educational or practical purposes."
    )

    self.GenTextSection(
      layout,
      "What language does it use?",
      "\tThe app is able to emulate a CISC architecture using a custom assembly language. "
      "It is designed to encapsulate most of the features present everyday-use assembly languages.\n"
      "More on how the code is structured and what instructions are available, down below."
    )

    self.GenTextSection(
      layout,
      "How do I use it?",
      "\tIn the 'Input' tab, write your assembly code on the left side, click the '->' button to convert it into binary code.\n"
      "Both can be edited, if you wish to make memory changes that are not possible via parsing the assembly code "
      "(e.g., force a parameter type to a forbidden combination for an op for the purpose of triggering an exception).\n"
      "You may also save / load your asm/hex files to / from the disk.\n"

      "\tHitting 'Execute' will take your binary code and pass it to the C++ \"backend\" for execution. "
      "Check the status at the top. Once 'Simulation complete.' is shown, you will be redirected to the results.\n"

      "\tIn the 'Simulation' tab, you have the diagram of the CPU pipeline, including the memory. "
      "Select any component to see the most relevant internal data at the current cycle.\n"
      "Move the slider at the top to change the cycle. You may also use the buttons, or LEFT_ARROW / SPACE / RIGHT_ARROW keys "
      "to go back 1 cycle, pause/play, or go forward 1 cycle, respectively.\n"

      "\tIf a component is green, it means that a change has occurred since the last cycle.\n"
    )

    self.GenTextSection(
      layout,
      "CPU's Architecture",
      "\tThe CPU works with 16-bit unsigned integers & addresses, called 'words'. 4 words make a 'fetch_window' "
      "(equivalent to qword in other languages). All 3 of them must be written (in 'Input') and are represented (in 'Simulation') "
      "as BIG-ENDIAN hex (most significant bits first, \"to the left\"). Values preceded by '0x' are words; '#' marks addresses.\n"
      "The address space is 64KB, so your code can be up to 64KB in size. Addresses point to bytes, so they must always be even, "
      "otherwise a MISALIGNED_IP / MISALIGNED_ACCESS exception will be raised.\n"

      "\tThe CPU has 8 general-purpose registers (R0-R7), an Instruction Pointer (IP), a Flags register (Flags), "
      "a Stack Base (ST_BASE), a Stack Size (ST_SIZE), and a Stack Pointer (SP_REG). All of them are word-sized.\n"

      "\tThe Stack resides in the physical memory. Keep in mind that the stack grows downwards, so the SP_REG points to the top "
      "element of the stack, and gets decremented on push and incremented on pop.\n"

      "\tThere are also 4 SIMD (Single Instruction, Multiple Data) registers (Z0-Z3), which are 4 words each, which can apply the "
      "same arithmetic operation to 4 words at once\n"

      "\tThe Flags of the CPU are: ZERO (last arithmetic op lead to a 0-result, or a compare found both operands 0), "
      "EQUAL (last compare found both operands equal), GREATER (last compare found the first operand bigger), and EXCEPTION "
      "(there is an active exception that the CPU is trying to recover from)."
    )

    table_headers = ["Name", "Purpose", "Communicates with"]
    modules_data = [
      ("Load/Store (LS)", "Handles memory operations", "IC, EX"),
      ("Instruction Cache (IC)", "Caches instructions for faster access", "LS, Decode"),
      ("Decode (DE)", "Decodes instructions and prepares operands", "Fetch, Execute"),
      ("Execute (EX)", "Performs arithmetic/logic operations", "Decode, Memory, Writeback"),
    ]
    self.GenTableSection(
      layout,
      "Modules",
      table_headers,
      modules_data
    )

    chapter6 = QLabel("Code Structure")
    chapter6.setFont(self.chapterFont)
    chapter6.setAlignment(Qt.AlignLeft | Qt.AlignTop)
    layout.addWidget(chapter6)

    content6 = QLabel("Content for Code Structure section will be added here...")
    content6.setFont(self.contentFont)
    content6.setWordWrap(True)
    layout.addWidget(content6)

    line6 = QLabel()
    line6.setFrameShape(QLabel.HLine)
    line6.setFrameShadow(QLabel.Sunken)
    layout.addWidget(line6)

    chapter7 = QLabel("Available Instructions")
    chapter7.setFont(self.chapterFont)
    chapter7.setAlignment(Qt.AlignLeft | Qt.AlignTop)
    layout.addWidget(chapter7)

    content7 = QLabel("Content for Available Instructions section will be added here...")
    content7.setFont(self.contentFont)
    content7.setWordWrap(True)
    layout.addWidget(content7)

    line7 = QLabel()
    line7.setFrameShape(QLabel.HLine)
    line7.setFrameShadow(QLabel.Sunken)
    layout.addWidget(line7)

    chapter8 = QLabel("Exceptions")
    chapter8.setFont(self.chapterFont)
    chapter8.setAlignment(Qt.AlignLeft | Qt.AlignTop)
    layout.addWidget(chapter8)

    content8 = QLabel("Content for Exceptions section will be added here...")
    content8.setFont(self.contentFont)
    content8.setWordWrap(True)
    layout.addWidget(content8)

    line8 = QLabel()
    line8.setFrameShape(QLabel.HLine)
    line8.setFrameShadow(QLabel.Sunken)
    layout.addWidget(line8)

    chapter9 = QLabel("Interpreting the Output")
    chapter9.setFont(self.chapterFont)
    chapter9.setAlignment(Qt.AlignLeft | Qt.AlignTop)
    layout.addWidget(chapter9)

    content9 = QLabel("Content for Interpreting the Output section will be added here...")
    content9.setFont(self.contentFont)
    content9.setWordWrap(True)
    layout.addWidget(content9)

    line9 = QLabel()
    line9.setFrameShape(QLabel.HLine)
    line9.setFrameShadow(QLabel.Sunken)
    layout.addWidget(line9)

    chapter10 = QLabel("Configuring the CPU")
    chapter10.setFont(self.chapterFont)
    chapter10.setAlignment(Qt.AlignLeft | Qt.AlignTop)
    layout.addWidget(chapter10)

    content10 = QLabel("Content for Configuring the CPU section will be added here...")
    content10.setFont(self.contentFont)
    content10.setWordWrap(True)
    layout.addWidget(content10)

    # Add stretch to push content to top
    layout.addStretch()

    # Set the content widget to the scroll area
    scrollArea.setWidget(contentWidget)
    
    # Add scroll area to main layout
    mainLayout.addWidget(scrollArea)
    mainLayout.setContentsMargins(0, 0, 0, 0)  # Remove margins from main layout

  # ---------------------------------------------------------------------------------------------------------------------------
  def GenTextSection(self, layout: QVBoxLayout, title: str | None, content: str) -> None:
    if title is not None:
      titleLabel = QLabel(title)
      titleLabel.setFont(self.chapterFont)
      titleLabel.setAlignment(Qt.AlignLeft | Qt.AlignTop)
      layout.addWidget(titleLabel)

    contentLabel = QLabel(content)
    contentLabel.setFont(self.contentFont)
    contentLabel.setWordWrap(True)
    layout.addWidget(contentLabel)

    line = QLabel()
    line.setFrameShape(QLabel.HLine)
    line.setFrameShadow(QLabel.Sunken)
    layout.addWidget(line)

  # ---------------------------------------------------------------------------------------------------------------------------
  def GenTableSection(self, layout: QVBoxLayout, title: str | None, headers: list[str], data: list[list[str]]) -> None:
    if title is not None:
      titleLabel = QLabel(title)
      titleLabel.setFont(self.chapterFont)
      titleLabel.setAlignment(Qt.AlignLeft | Qt.AlignTop)
      layout.addWidget(titleLabel)

    table = QTableWidget(len(data), len(headers))
    table.setHorizontalHeaderLabels(headers)
    table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
    table.verticalHeader().setVisible(False)
    table.setEditTriggers(QTableWidget.NoEditTriggers)
    table.setSelectionMode(QTableWidget.NoSelection)
    table.setFocusPolicy(Qt.NoFocus)

    for row, rowData in enumerate(data):
      for col, item in enumerate(rowData):
        table.setItem(row, col, QTableWidgetItem(item))
    
    layout.addWidget(table)

    line = QLabel()
    line.setFrameShape(QLabel.HLine)
    line.setFrameShadow(QLabel.Sunken)
    layout.addWidget(line)


# -----------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
  raise Exception("This module is not meant to be run directly.")