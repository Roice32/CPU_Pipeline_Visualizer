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
      "The address space is 64kB, so your code can be up to 64kB in size. Addresses point to bytes, so they must always be even, "
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
      ("Memory", "Holds the 64kB of physical memory\n"
                 "External to the CPU", "Load/Store"),
      ("Load/Store (LS)", "Delivers fetch windows from memory to IC\n"
                          "Delivers data from memory to EX\n"
                          "Stores data from EX to memory\n"
                          "Signals written addresses to IC for cache invalidation\n", "Instruction Cache\nExecute"),
      ("Instruction Cache (IC)", "Requests fetch windows from LS\n"
                                 "Delivers fetch windows to DE\n"
                                 "Keeps an internal IP to maintain constant delivery of fetch windows to DE\n"
                                 "Starts simulation from #fff0", "Load/Store\nDecode"),
      ("Decode (DE)", "Decodes raw words from fetch windows into OpCode, Src1, Src2, Param1, Param2\n"
                      "Delivers decoded instructions to Execute\n"
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
      data,
      line_after=False
    )

    self.GenTextSection(
      layout,
      None,
      "\tThe modules communicate with eachother via pipes. All pipes send Synchronized Data Packages, composed of:\n"
      "> data specific to each pipe\n"
      "> cycle it was sent at\n"
      "> IP that data / request is from / referring to\n"
      "> if an exception was triggered by the data / request\n"
      "> the exception data, if applicable\n"
      "> the address of the handler for the exception, if applicable\n",
      line_after=False
    )

    tableHeaders = ["From", "To", "Data Type", "Purpose"]
    data = [
      ("IC",
       "LS",
       "Address",
       "Request a fetch window from memory"),
      ("LS",
       "IC",
       "Fetch window",
       "Deliver the requested fetch window\n"
       "Signal a write from EX to an address possibly cached in IC\n"
       "(uses only lower 16 bits of fetch window)"),
      ("IC",
       "DE",
       "Fetch window",
       "Deliver the fetch window for decoding"),
      ("DE",
       "IC",
       "Address",
       "Forward IP change signal from EX"),
      ("DE",
       "EX",
       "Instruction",
       "Deliver decoded instructions for execution\n"
       "Signal MISALIGNED_IP exception\n"
       "Signal INVALID_DECODE exception"),
      ("EX",
       "DE",
       "Address",
       "Signal IP change"),
      ("EX",
       "LS",
       "Word array",
       "Request fetch of data from memory (empty data sent)\n"
       "Request store of data to memory\n"),
      ("LS",
       "EX",
       "Word array",
       "Deliver requested data from memory\n"
       "Confirm store of data to memory (empty data sent)\n"
       "Signal MISALIGNED_ACCESS exception")
    ]

    self.GenTableSection(
      layout,
      None,
      tableHeaders,
      data,
      line_after=False
    )

    self.GenTextSection(
      layout,
      "Caches",
      "\tThe IC has a Direct-Mapping Cache, storing fetch windows.\n"
      "Let X be the size of the cache in words. The cache will hold X/4 entries. Let lX = log2(X/4)\n"
      "Each entry has an index and a tag. Each address of a fetch window is split into 3 parts:\n"
      "1. Unused bits (bits [2:0], least-significant), since fetch windows always start at addresses multiple of 8\n"
      "2. Index bits (next lX bits), which are used to find the entry index in the cache\n"
      "3. Tag bits (remaining, most-significant bits), which locate the entry as belonging to a certain page of memory "
      "(the 64kB address space is split into as many regions as the tag bits allow).\n\n"
      "This strategy makes cache swapping easier, as a the index directly points to the entry in the cache to be replaced, "
      "skipping the need to search through all entries for the oldest one, in a LRU (Least Recently Used) strategy.\n\n"

      "\tThe LS has a K-Way Set-Associative Cache, storing words.\n"
      "Let X be the size of the cache in words, and K the number of ways (elements in each set). The cache will hold X entries, "
      "grouped into X / K sets. Let lXK = log2(X / K) and lK = log2(K)\n"
      "Each entry has an index, a sub-index, and a tag. The address of a word is split similarly to how IC does the splitting, "
      "with the difference that there is only one unused bit (0), and the index is lXK bits long. The sub-index is the lower "
      "lK bits of the index.\n\n"
      "This strategy allows the LS to store multiple words with same index (localized address within page), but from different "
      "pages (different tags), whilst still maintaining fast search and replacement of entries, checking the tag of only K entries "
      "to find a hit / the oldest entry to replace.\n"
      "Note that the LS does not cache the words of fetch windows, as they would quickly fill the cache, "
      "leaving little to no room for words needed by EX. Besides, it would be redundant, as the IC already caches them."
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
        "May only hold a 'dw' with the handler's label from .code"),
      ("Save State", "N/A", "[#0010 - #0027]", "Area where {IP, SP_REG, FLAGS, ExcpData, R0-7} are saved when an exception occurs", "N/A",
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
      "\tSource types accepted by the instructions are:\n",
      line_after=False
    )

    tableHeaders = ["Type", "Mnemonic(s)", "Hex Value(s)", "Description"]
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
      line_after=False
    )

    self.GenTextSection(
      layout,
      None,
      "\tIn the table below, the following notations are used:\n"
      "simd-comp = {simd, addr, addr_reg}; non-simd-comp = SrcTypes \\ simd-comp\n"
      "stack = {sp, stack_base, stack_size};\n"
      "stack-comp = {reg, imm, addr, addr_reg}; non-stack-comp = SrcTypes \\ stack-comp",
      line_after=False
    )

    tableHeaders = ["Mnemonic", "OpCode", "Src1", "Src2", "Outcome", "Affected Flags", "Invalid Src1, Src2\nCombinations"]
    data = [
      ("add",
       "0x1",
       "reg\naddr\naddr_reg\nsimd",
       "reg\nimm\naddr\naddr_reg\nsimd",
       "Src1 = Src1 + Src2",
       "ZERO = 1 if result is 0",
       "simd, non-simd-comp\nnon-simd-comp, simd"),
      ("sub",
       "0x2",
       "reg\naddr\naddr_reg\nsimd",
       "reg\nimm\naddr\naddr_reg\nsimd",
       "Src1 = Src1 - Src2",
       "ZERO = 1 if result is 0",
       "simd, non-simd-comp\nnon-simd-comp, simd"),
      ("mov",
       "0x3",
       "reg\naddr\naddr_reg\nsp\nstack_base\nstack_size\nsimd",
       "reg\naddr\naddr_reg\nimm\nsp\nstack_base\nstack_size\nsimd",
       "Src1 = Src2",
       "-",
       "stack, stack\nsimd, non-simd-comp\nnon-simd-comp, simd\n"),
      ("mul",
       "0x4",
       "reg\naddr\naddr_reg\nimm\nsimd",
       "reg\naddr\naddr_reg\nimm\nsimd",
       "For non-SIMD:\n[R0:R1] = Src1 * Src2\nFor SIMD:\n[Z0_X:Z1_X] =\nSrc1_X * Src2_X\n(X = 0..3)",
       "ZERO = 1 if result (any X for SIMD) is 0",
       "simd, non-simd-comp\nnon-simd-comp, simd"),
      ("div",
       "0x5",
       "reg\naddr\naddr_reg\nimm\nsimd",
       "reg\naddr\naddr_reg\nimm\nsimd",
       "For non-SIMD:\nR0 = Src1 / Src2\nR1 = Src1 % Src2\nFor SIMD:\nZ0_X = Src1_X / Src2_X,\nZ1_X = Src1_X % Src2_X\n(X = 0..3)",
       "ZERO = 1 if result (any X for SIMD) is 0",
       "simd, non-simd-comp\nnon-simd-comp, simd"),
      ("cmp",
       "0x6",
       "reg\nimm\naddr_reg\naddr\nsp\nstack_base\nstack_size",
       "reg\nimm\naddr_reg\naddr\nsp\nstack_base\nstack_size",
       "Updates the Flags based on Src1 ? Src2",
       "ZERO = 1 if\nSrc1 == Src2 == 0\nEQUAL = 1 if\nSrc1 == Src2\nGREATER = 1 if\nSrc1 > Src2",
       "stack, stack\nsimd, non-simd-comp\nnon-simd-comp, simd"),
      ("jmp",
       "0x7",
       "reg\nimm\naddr\naddr_reg",
       "null",
       "IP = Src1",
       "-",
       "-"),
      ("je",
       "0x9",
       "reg\nimm\naddr\naddr_reg",
       "null",
       "IP = Src1 if\nFlags.EQUAL == 1",
       "-",
       "-"),
      ("jl",
       "0xa",
       "reg\nimm\naddr\naddr_reg",
       "null",
       "IP = Src1 if\nFlags.GREATER == 0 and Flags.EQUAL == 0",
       "-",
       "-"),
      ("jg",
       "0xb",
       "reg\nimm\naddr\naddr_reg",
       "null",
       "IP = Src1 if\nFlags.GREATER == 1",
       "-",
       "-"),
      ("jz",
       "0xc",
       "reg\nimm\naddr\naddr_reg",
       "null",
       "IP = Src1 if\nFlags.ZERO == 1",
       "-",
       "-"),
      ("call",
       "0xd",
       "reg\nimm\naddr\naddr_reg",
       "null",
       "R7-R0, Flags, IP+0x2 pushed to stack\nIP = Src1",
       "-",
       "-"),
      ("ret",
       "0xe",
       "null",
       "null",
       "IP, Flags, R0-R7 popped from stack in order",
       "-",
       "-"),
      ("end_sim",
       "0xf",
       "null",
       "null",
       "End simulation",
       "-",
       "-"),
      ("push",
       "0x10",
       "reg, addr, imm, addr_reg",
       "null",
       "Src1 pushed to stack",
       "-",
       "-"),
      ("pop",
       "0x11",
       "reg, addr, addr_reg, null",
       "null",
       "Pop 1 word from stack\nIf Src1 != null, write it to Src1",
       "-",
       "-"),
      ("excp_exit",
       "0x12",
       "null",
       "null",
       "Restore IP, SP_REG, Flags, ExcpData, R0-R7\nfrom Save State area",
       "EXCEPTION = 0",
       "-"),
      ("gather",
       "0x16",
       "simd",
       "simd",
       "Src1_X = [Src2_X]\nfor X = 0..3\n",
       "-",
       "-"),
      ("scatter",
       "0x17",
       "simd",
       "simd",
       "[Src1_X] = Src2_X\nfor X = 0..3\n",
       "-",
       "-")
    ]

    self.GenTableSection(
      layout,
      "Available Instructions",
      tableHeaders,
      data,
    )

    self.GenTextSection(
      layout,
      None,
      "\tThere are also two pre-assembler directives (usually used immediately after a label):\n"
      "dw <val> - Declares a 16-bit word with initial value = <val>\n"
      "dblock <val>, <count> - Declares a contiguous block of <count> words, all with initial value = <val>\n"
    )

    tableHeaders = ["Exception", "Exception Data", "Vector Entry", "Generated in", "Caused by"]

    data = [
      ("DIV_BY_ZERO",
       "0x0000",
       ".vector_0",
       "EX",
       "Src2 of 'div' (or any Src2_X for SIMD) is 0"),
      ("UNKNOWN_OP_CODE",
       "0x0001",
       ".vector_2",
       "DE",
       "Value of first 6 bits in a decoded word is not a valid OpCode"),
      ("NULL_SRC",
       "0x0002",
       ".vector_2",
       "DE",
       "Src1 or Src2 is null when null is not expected"),
      ("NON_NULL_SRC",
       "0x0004",
       ".vector_2",
       "DE",
       "Src1 or Src2 is not null when null is expected"),
      ("INCOMPATIBLE_PARAMS",
       "0x0008",
       ".vector_2",
       "DE",
       "Src1 and Src2 are incompatible for the given OpCode"),
      ("PUSH_OVERFLOW",
       "0x0000",
       ".vector_4",
       "EX",
       "Pushing to the stack when it is full (SP_REG == 0x0)"),
      ("POP_OVERFLOW",
       "0xffff",
       ".vector_4",
       "EX",
       "Popping from the stack when it is empty (SP_REG == STACK_SIZE)"),
      ("MISALIGNED_ACCESS",
       "Address of requested data",
       ".vector_6",
       "LS",
       "A memory access was made to an odd address"),
      ("MISALIGNED_IP",
       "0x0000",
       ".vector_8",
       "DE",
       "A call / jump was made to an odd address")
    ]

    self.GenTableSection(
      layout,
      "Exceptions",
      tableHeaders,
      data
    )

    self.GenTextSection(
      layout,
      "Configuring the CPU",
      "\tThe 'Config' tab is dedicated to parameterizing the CPU's architecture.\n"
      "\tThe configuration options fall into 4 categories:\n"
      "1. Modules' cycles count per operation: How many cycles each module to execute one operation "
      "(e.g., the Decode module stays busy for 2 cycles to turn 1-3 words into an instruction).\n"
      "2. Caches' settings: Controls the size of the Instruction Cache (IC) and Load/Store Cache (LS), "
      "as well as the lowered cycle count for these modules when a cache hit occurs and no physical memory "
      "access is needed.\n"
      "3. Simulation rules: If the EX module should consider OpCode 0x0 as a 'NOP' or trigger an INVALID_DECODE exception.\n"
      "If reads from memory locations not initialized / written to should return 0x00 or a random value, simulating garbage data.\n"
      "And setting a maximum number of cycles for the simulation, after which it will stop automatically.\n"
      "4. Simulation settings, which do not affect the outcome of the simulation: The clock's period (in ms), "
      "whether to display all cycles of the simulation or only the first and last, and the speed of the autoplay "
      "of the simulation (in ms)."
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