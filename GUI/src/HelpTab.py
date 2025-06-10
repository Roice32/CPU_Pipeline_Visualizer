from PyQt5.QtWidgets import QWidget, QVBoxLayout, QLabel, QTextEdit
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QFont
from PyQt5.QtWidgets import QScrollArea, QWidget
from PyQt5.QtWidgets import QTableWidget, QTableWidgetItem, QHeaderView


# -----------------------------------------------------------------------------------------------------------------------------
class HelpTab(QWidget):
  parent = None

  chapterFont     = None
  contentFont     = None
  tableHeaderFont = None

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
    titleFont.setPointSize(16)
    titleFont.setBold(True)

    self.chapterFont = QFont()
    self.chapterFont.setPointSize(14)
    self.chapterFont.setBold(True)
    self.chapterFont.setUnderline(True)

    self.contentFont = QFont()
    self.contentFont.setPointSize(11)

    self.tableHeaderFont = QFont()
    self.tableHeaderFont.setPointSize(12)
    self.tableHeaderFont.setBold(True)

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
      "same arithmetic operation to 4 words at once.\n"

      "\tThe Flags of the CPU are: ZERO (last arithmetic op lead to a 0-result, or a compare found both operands 0), "
      "EQUAL (last compare found both operands equal), GREATER (last compare found the first operand bigger), and EXCEPTION "
      "(there is an active exception that the CPU is trying to recover from)."
    )

    tableHeaders = ["Name", "Purpose", "Communicates with"]
    data = [
      ("Clock", "Generates clock signals for synchronizing the other modules", "All"),
      ("Memory", "Holds the 64KB of physical memory\n"
                 "External to the CPU", "Load/Store"),
      ("Load/Store (LS)", "Delivers fetch windows from memory to IC\n"
                          "Delivers data from memory to EX\n"
                          "Stores data from EX to memory\n"
                          "Signals written addresses to IC for cache invalidation\n", "Instruction Cache\nExecute"),
      ("Instruction Cache (IC)", "Requests fetch windows from LS\n"
                                 "Delivers fetch windows to DE\n"
                                 "Keeps an internal IP to maintain constant delivery of fetch windows to DE\n"
                                 "Starts simulation from #fff0", "Load/Store\nDecode"),
      ("Decode (DE)", "Decodes raw words from fetch windows into OpCode, Src1, Scr2, Param1, Param2\n"
                      "Delivers decoded instructions to Execute"
                      "Forwards IP change signals to the Instruction Cache\n"
                      "Keeps internal storage of at most 2 fetch windows", "Instruction Cache\nExecute"),
      ("Execute (EX)", "Performs arithmetic/logic operations\n"
                       "Handles exceptions\n"
                       "Makes request to Load/Store", "Decode\nLoad/Store")
    ]
    self.GenTableSection(
      layout,
      "Modules",
      tableHeaders,
      data
    )

    self.GenTextSection(
      layout,
      "Code Structure",
      "\tYour code has the following sections:",
      line_after=False
    )

    tableHeaders = ["Section", "Asm Label", "Range", "Purpose", "Required?", "Extra"]
    data = [
      ("Boot", ".boot", "[#fff0 - #ffff]", "Contains the initial instructions to be executed by the CPU", "Yes",
       "Only one .boot section is allowed\n"
       "Since it's so small, it should only have a jump to the .code section"),
      ("Data", ".data", "[#1000 - #ffef]", "Contains only declarations of variables", "No",
       "Only one .data section is allowed\n"
       "Shared address space with .code and Stack\n"),
      ("Code", ".code", "[#1000 - #ffef]", "Contains the instructions & labels of your code\n"
                                           "May also contain declarations of variables", "No",
       "Only one .code section is allowed\n",
       "Shared address space with .data and Stack"),
      ("Stack", "N/A", "[(ST_BASE-ST_SIZE) - ST_BASE)\n" 
                       "[#dfee - #efee) (default)", "Downwards-growing stack for the CPU", "N/A",
       "Not a user-defined section\n"
       "May be modified by writing to ST_BASE and ST_SIZE\n"
       "Shared address space with .data and .code\n"),
      ("Exception\n"
       "Handlers\n"
       "Vector", ".vector_X\n"
                 "X = 0|2|4|6|8", "[(2*X) - (2*X+1)]\n", "Hold the handlers' addresses for exceptions\n", "No",
        "Only one .vector_X section is allowed per exceptions\n"
        "May only hold a 'dw' instruction with the handler's label from .code"),
      ("Save State", "N/A", "[#0010 - #0027]", "Area where {IP, SP_REG, FLAGS, R0-7} are saved when an exception occurs", "N/A",
       "Not a user-defined section\n")
    ]

    self.GenTableSection(
      layout,
      None,
      tableHeaders,
      data
    )

    self.GenTextSection(
      layout,
      "Available Instructions",
      "\tSource types acceptes by the instructions are:\n",
      line_after=False
    )

    tableHeaders = ["Type", "Mnemonic(s)", "Value(s)", "Description"]
    data = [
      ("null", " ", "0x0", "Src not present"),
      ("imm", "0 - ffff\n"
              "0x0 - 0xffff\n"
              "label", "0x1", "Immediate hex value / label"),
      ("addr", "[0] - [ffff]\n"
               "[0x0] - [0xffff]\n"
               "[label]", "0x2", "Value at memory address / label"),
      ("sp", "sp", "0x3", "Current stack pointer\n"
                          "(how many bytes are free in the stack)"),
      ("stack_base", "stack_base", "0x4", "Top end of the stack\n"
                                          "(from where it grows downwards)"),
      ("stack_size", "stack_size", "0x5", "Size of the stack in bytes"),
      ("reg", "r0 - r7", "0x8-0xF", "Value in general purpose registers R0 - R7"),
      ("addr_reg", "[r0] - [r7]", "0x18-0x1F", "Value at address stored in general purpose registers R0 - R7"),
      ("simd", "Z0 - Z3", "0x10-0x13", "SIMD registers Z0 through Z3"),
    ]

    self.GenTableSection(
      layout,
      None,
      tableHeaders,
      data,
      False
    )

    tableHeaders = ["Mnemonic", "OpCode", "Src1", "Src2", "Description", "Affected Flags", "Invalid Src1 + Src2\nCombinations"]
    data = [
      ("add", "0x1", "reg, addr, addr_reg, simd", "reg, imm, addr, addr_reg, simd", "Addition operation", "WIP", "simd+reg, simd+imm"),
      ("sub", "0x2", "reg, addr, addr_reg, simd", "reg, imm, addr, addr_reg, simd", "Subtraction operation", "WIP", "simd+reg, simd+imm"),
      ("mov", "0x3", "reg, addr, addr_reg, sp, stack_base, stack_size, simd", "reg, addr, addr_reg, imm, sp, stack_base, stack_size, simd", "Move operation", "WIP", "sp+imm, stack_base+imm, stack_size+imm, simd+reg, simd+imm"),
      ("mul", "0x4", "reg, addr, addr_reg, imm, simd", "reg, addr, addr_reg, imm, simd", "Multiplication operation", "WIP", "simd+reg, simd+imm"),
      ("div", "0x5", "reg, addr, addr_reg, imm, simd", "reg, addr, addr_reg, imm, simd", "Division operation", "WIP", "simd+reg, simd+imm"),
      ("cmp", "0x6", "reg, imm, addr_reg, addr, sp, stack_base, stack_size", "reg, imm, addr_reg, addr, sp, stack_base, stack_size", "Compare operation", "WIP", "sp+reg, sp+addr, sp+addr_reg, sp+imm, stack_base+reg, stack_base+addr, stack_base+addr_reg, stack_base+imm, stack_size+reg, stack_size+addr, stack_size+addr_reg, stack_size+imm"),
      ("jmp", "0x7", "reg, imm, addr, addr_reg", "-", "Unconditional jump", "WIP", ""),
      ("je", "0x9", "reg, imm, addr, addr_reg", "-", "Jump if equal", "WIP", ""),
      ("jl", "0xA", "reg, imm, addr, addr_reg", "-", "Jump if less", "WIP", ""),
      ("jg", "0xB", "reg, imm, addr, addr_reg", "-", "Jump if greater", "WIP", ""),
      ("jz", "0xC", "reg, imm, addr, addr_reg", "-", "Jump if zero", "WIP", ""),
      ("call", "0xD", "reg, imm, addr, addr_reg", "-", "Function call", "WIP", ""),
      ("ret", "0xE", "-", "-", "Return from function", "WIP", ""),
      ("end_sim", "0xF", "-", "-", "End simulation", "WIP", ""),
      ("push", "0x10", "reg, addr, imm, addr_reg", "-", "Push to stack", "WIP", ""),
      ("pop", "0x11", "reg, addr, addr_reg, null", "-", "Pop from stack", "WIP", ""),
      ("dw", "null", "imm", "-", "Define word", "WIP", ""),
      ("dblock", "null", "imm", "imm", "Define block", "WIP", ""),
      ("excp_exit", "0x12", "-", "-", "Exception exit", "WIP", ""),
      ("gather", "0x16", "simd", "simd", "SIMD gather operation", "WIP", ""),
      ("scatter", "0x17", "simd", "simd", "SIMD scatter operation", "WIP", "")
    ]

    self.GenTableSection(
      layout,
      "Available Instructions",
      tableHeaders,
      data,
    )

    self.GenTableSection(
      layout,
      "Exceptions",
      [],
      []
    )

    self.GenTextSection(
      layout,
      "Interpreting the Output",
      "Content for Interpreting the Output section will be added here..."
    )

    self.GenTextSection(
      layout,
      "Configuring the CPU",
      "Content for Configuring the CPU section will be added here...",
      line_after=False
    )

    # Add stretch to push content to top
    layout.addStretch()

    # Set the content widget to the scroll area
    scrollArea.setWidget(contentWidget)
    
    # Add scroll area to main layout
    mainLayout.addWidget(scrollArea)
    mainLayout.setContentsMargins(0, 0, 0, 0)  # Remove margins from main layout

  # ---------------------------------------------------------------------------------------------------------------------------
  def GenTitle(self, layout: QVBoxLayout, title: str) -> None:
    titleLabel = QLabel(title)
    titleLabel.setFont(self.chapterFont)
    titleLabel.setAlignment(Qt.AlignLeft | Qt.AlignTop)
    layout.addWidget(titleLabel)

  # ---------------------------------------------------------------------------------------------------------------------------
  def GenLine(self, layout: QVBoxLayout) -> None:
    line = QLabel()
    line.setAlignment(Qt.AlignLeft | Qt.AlignTop)
    line.setFrameShape(QLabel.HLine)
    line.setFrameShadow(QLabel.Sunken)
    layout.addWidget(line)

  # ---------------------------------------------------------------------------------------------------------------------------
  def GenTextSection(self,
                     layout: QVBoxLayout,
                     title: str | None,
                     content: str,
                     line_after: bool = True) -> None:
    if title is not None:
      self.GenTitle(layout, title)

    contentLabel = QLabel(content)
    contentLabel.setFont(self.contentFont)
    contentLabel.setWordWrap(True)
    layout.addWidget(contentLabel)

    if line_after:
      self.GenLine(layout)

  # ---------------------------------------------------------------------------------------------------------------------------
  def GenTableSection(self,
                      layout: QVBoxLayout,
                      title: str | None,
                      headers: list[str],
                      data: list[list[str]],
                      line_after: bool = True) -> None:
    if title is not None:
      self.GenTitle(layout, title)

    table = QTableWidget()
    table.setColumnCount(len(headers))
    table.setRowCount(len(data))
    table.setHorizontalHeaderLabels(headers)

    for row, rowData in enumerate(data):
      for col, item in enumerate(rowData):
        tableItem = QTableWidgetItem(item)
        tableItem.setFlags(Qt.ItemIsEnabled)
        tableItem.setFont(self.contentFont)
        tableItem.setTextAlignment(Qt.AlignCenter)
        table.setItem(row, col, tableItem)

    table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
    table.verticalHeader().setVisible(False)
    table.setStyleSheet(
            "QTableWidget {"
            "   border: 1px solid black;"
            "   background-color: white;"
            "}"
            "QHeaderView::section {"
            "   background-color: #dcdcdc;"
            "   color: black;"
            "   padding: 0px;"
            "   margin: 0px;"
            "   border: 1px solid black;"
            "   border-bottom-color: black;"
            "   font-weight: bold;"
            "   font-size: 16px;"
            "}"
            "QTableWidget::horizontalHeader { border-bottom: 1px solid black; }"
    )
    table.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
    table.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
    table.resizeRowsToContents()
    table.resizeColumnsToContents()

    tableHeight = table.horizontalHeader().height()
    for row in range(table.rowCount()):
      tableHeight += table.rowHeight(row)
    table.setMinimumHeight(tableHeight)
    table.setMaximumHeight(tableHeight)

    layout.addWidget(table)

    if line_after:
      self.GenLine(layout)


# -----------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
  raise Exception("This module is not meant to be run directly.")