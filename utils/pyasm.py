import pyparsing as pp
import yaml
from pathlib import Path
from pydantic import BaseModel
from enum import Enum
from typer import Typer, Option

class SrcType(Enum):
  null       = 0
  reg        = 1
  addr       = 2
  addr_reg   = 3
  imm        = 4
  sp         = 5
  stack_base = 6
  stack_size = 7
  simd       = 8

class ExclusiveSrc(BaseModel):
  src: SrcType
  exclusiveSrc: list[SrcType] | None

class Encoding(BaseModel):
  opcode: int | None
  src1: dict[SrcType, list[SrcType] | None]
  src2: dict[SrcType, list[SrcType] | None]

class Instruction(BaseModel):
  opcode: int | None
  src1Type: SrcType
  src2Type: SrcType
  src1: int | str
  src2: int | str
  length: int

class Section(BaseModel):
  instrDict: dict[str, list[Instruction]]

class Assembler:
  parser:pp.ParserElement
  instrEncodings = {}
  sectionDict = {}
  midInstrDict:     dict[str, list[Instruction]] = {}
  bootInstructions: dict[str, list[Instruction]] = {}
  excpVectors:      dict[int, list[Instruction]] = {}
  currentLabel = "0"
  bootLabel = "1"
  currentSection :str
  codeStartAddr = 0x1000
  bootStartAddr = 0xFFF0

  labelDict = {}

  def _getSourceFromYmlDict(self, sourceKey:str, sourceValues:dict) -> dict[SrcType, list[SrcType] | None]:
    src:dict[SrcType, list[SrcType] | None] = {}
    if sourceKey in sourceValues:
      for item in sourceValues[sourceKey]:
        if type(item) is str:
          src[SrcType[item]] = None
        else:
          for key in item:
            src[SrcType[key]] = []
            for srcType in item[key]:
              src[SrcType[key]].append(SrcType[srcType])  #type:ignore
    else:
      src[SrcType.null] = None

    return src

  def getInstructionsFromYml(self, configFile:Path) -> dict[str, Encoding]:
    if not configFile.exists():
      raise FileNotFoundError(f"Could not find file {configFile}")

    with open(configFile, "r") as fh:
      data = yaml.safe_load(fh)
      instrEncodings = {}

      for item in data:
        src1:dict[SrcType, list[SrcType] | None] = self._getSourceFromYmlDict("src1", item)
        src2:dict[SrcType, list[SrcType] | None] = self._getSourceFromYmlDict("src2", item)
        mnemonic = item["mnemonic"]
        opcode = item["opcode"]
        enc:Encoding = Encoding(opcode = opcode, src1 = src1, src2 = src2)
        instrEncodings[mnemonic] = enc

      return instrEncodings
    
  def solveLabels(self, instrDict:dict, startAddr:int, endAddr:int):
    labelDict = {}
    currentLabelValue = startAddr
    for label in instrDict:
      labelDict[label] = currentLabelValue
      for instr in instrDict[label]:
        currentLabelValue = currentLabelValue + instr.length
        if currentLabelValue > endAddr:
          raise Exception(f"Resulting program would be larger than the maximum allowed memory of 64kB")

    return labelDict

  def __init__(self, configFile:Path = Path("asm_cfg.yml")) -> None:

    comment  : pp.ParserElement = pp.Literal(";") + pp.restOfLine
    lineEnder: pp.ParserElement = pp.Suppress(comment) | pp.LineEnd().suppress()

    data:pp.ParserElement = pp.Literal(".data") + pp.Optional(":")
    code:pp.ParserElement = pp.Literal(".code") + pp.Optional(":")
    excp:pp.ParserElement = pp.Combine(pp.Literal(".vector_") + pp.oneOf("0 2 4 6 8 A C E")) + pp.Optional(":")
    boot:pp.ParserElement = pp.Literal(".boot") + pp.Optional(":")
    section:pp.ParserElement = (code | data | excp | boot) + lineEnder

    #Parse Action function definitions
    #Must do these here to access class members with ease, although the code gets bloated
    def labelFn(toks):
      label = toks[0]
      if label in self.bootInstructions or label in self.midInstrDict:
        raise Exception(f"Label {label} is duplicated")

      if self.currentSection == ".boot":
        self.bootLabel = label
        self.bootInstructions[label] = []
      else:
        self.currentLabel = label
        self.midInstrDict[label] = []

    def sectionFn(toks):
      if toks[0] in self.sectionDict:
        raise Exception(f"{toks[0]} section already exists. It can only be defined once.")
      self.sectionDict[toks[0]] = []
      self.currentSection = toks[0]

    def mnemnonicFn(s, loc, toks):
      origTok = toks[0]
      toks[0] = toks[0].lower()
      if toks[0] not in self.instrEncodings:
        raise Exception(f"Unknown instruction '{origTok}' found: {s.lstrip()}")

    def basicRegFn(toks):
      toks[0] = (int(toks[0]), SrcType.reg)
    
    def simdRegFn(toks):
      toks[0] = (int(toks[0]), SrcType.simd)
    
    def stackRegFn(toks):
      toks[0] = (0, SrcType[toks[0]])

    def hexImmFn(toks):
      toks[0] = int(toks[0], base=16)

    def decImmFn(toks):
      toks[0] = int(toks[0])

    def immFn(toks):
      toks[0] = (toks[0], SrcType.imm)

    def addrFn(toks):
      toks[0] = (toks[0], SrcType.addr)

    def regAddrFn(toks):
      toks[0] = (int(toks[0]), SrcType.addr_reg)

    def instrFn(s, loc, toks):
      mnemonic = toks[0]
      src1Type:SrcType = toks[1][1] if len(toks) >= 2 else SrcType.null
      src2Type:SrcType = toks[2][1] if len(toks) == 3 else SrcType.null
      src1     = toks[1][0] if len(toks) >= 2 else 0
      src2     = toks[2][0] if len(toks) == 3 else 0
      encoding = self.instrEncodings[mnemonic]
      #Check if the instruction is valid
      if src1Type not in encoding.src1:
        raise Exception(f"src1 can't be of type {src1Type} for \"{mnemonic.upper()}\"\nValid types are: {list(encoding.src1.keys())}")
      if src2Type not in encoding.src2:
        raise Exception(f"src2 can't be of type {src2Type} for \"{mnemonic.upper()}\"\nValid types are: {list(encoding.src2.keys())}")

      if encoding.src1[src1Type] is not None:
        exclusiveSrcs:list[SrcType] = encoding.src1[src1Type] #type:ignore
        if src2Type not in exclusiveSrcs:
          raise Exception(f"src2 can't be of type {src2Type} for \"{mnemonic.upper()}\" when src1 is of type {src1Type}\nValid types for src2 are: {list(exclusiveSrcs)}")

      if encoding.src2[src2Type] is not None:
        exclusiveSrcs:list[SrcType] = encoding.src2[src2Type] #type:ignore
        if src1Type not in exclusiveSrcs:
          raise Exception(f"src1 can't be of type {src1Type} for \"{mnemonic.upper()}\" when src2 is of type {src2Type}\nValid types for src1 are: {list(exclusiveSrcs)}")

      instrLen = 2 #bytes
      if encoding.opcode is None:
        if src2Type is SrcType.imm:
          instrLen = 2 * src2
      else:
        if src1Type in [SrcType.addr, SrcType.imm]:
          instrLen = instrLen + 2
        if src2Type in [SrcType.addr, SrcType.imm]:
          instrLen = instrLen + 2

      instr = Instruction(opcode=encoding.opcode, src1Type=src1Type, src2Type=src2Type, src1=src1, src2=src2, length=instrLen)
      if self.currentSection == ".code":
        if self.currentLabel not in self.midInstrDict:
          self.midInstrDict[self.currentLabel] = []
        
        self.midInstrDict[self.currentLabel].append(instr)
      elif self.currentSection == ".boot":
        if self.bootLabel not in self.bootInstructions:
          self.bootInstructions[self.bootLabel] = []

        self.bootInstructions[self.bootLabel].append(instr)

      elif self.currentSection == ".data":
        if instr.opcode is not None:
          raise Exception(f"Regular instructions are not allowed in data section")
        
        if self.currentLabel not in self.midInstrDict:
          self.midInstrDict[self.currentLabel] = []
        
        self.midInstrDict[self.currentLabel].append(instr)
      elif self.currentSection.startswith(".vector"):
        #Exception/Intr vector
        if instr.opcode is not None:
          raise Exception(f"Regular instructions are not allowed in exception section.\nAn exception vector handle address should be defined here")
        if self.currentSection[-1] in self.excpVectors:
          raise Exception(f"Exception vector should contain a single 16 bit word")
        
        addr = int(self.currentSection[-1], 16)
        self.excpVectors[addr] = []
        self.excpVectors[addr].append(instr)
        
      else:
        raise Exception(f"Found an instruction in a non-code section")

    label:pp.ParserElement = pp.Word(pp.alphas + "_", pp.alphanums + "_")
    labelDefine = label + pp.Suppress(":") + lineEnder
    labelDefine.setParseAction(labelFn)

    mnemonic:pp.ParserElement = pp.Word(pp.alphas, pp.alphanums + "_")
    mnemonic.setParseAction(mnemnonicFn)

    basicReg :pp.ParserElement = pp.Suppress(pp.CaselessLiteral("R")) + pp.Regex("[0-7]")
    simdReg  :pp.ParserElement = pp.Suppress(pp.CaselessLiteral("Z")) + pp.Regex("[0-3]")
    stackReg :pp.ParserElement = pp.CaselessLiteral("sp") | pp.CaselessLiteral("stack_base") | pp.CaselessLiteral("stack_size")
    
    register :pp.ParserElement = basicReg | simdReg | stackReg
    decImm   :pp.ParserElement = pp.Word(pp.nums)
    hexImm   :pp.ParserElement = pp.Suppress("0x") + pp.Word(pp.hexnums)
    immediate:pp.ParserElement = hexImm | decImm | label
    addrImm  :pp.ParserElement = hexImm | decImm | label
    addr     :pp.ParserElement = pp.Suppress("[") + addrImm + pp.Suppress("]")
    addrReg  :pp.ParserElement = pp.Suppress("[") + basicReg.copy() + pp.Suppress("]")

    basicReg.setParseAction(basicRegFn)
    simdReg.setParseAction(simdRegFn)
    stackReg.setParseAction(stackRegFn)
    immediate.setParseAction(immFn)
    addr.setParseAction(addrFn)
    addrReg.setParseAction(regAddrFn)
    section.setParseAction(sectionFn)
    hexImm.setParseAction(hexImmFn)
    decImm.setParseAction(decImmFn)

    src = register | immediate | addrReg | addr

    instruction: pp.ParserElement = mnemonic + pp.Optional(src) + pp.Optional(",").suppress() + pp.Optional(src) + (pp.Suppress(comment) | pp.LineEnd().suppress())
    instruction.setParseAction(instrFn)


    self.parser = section | labelDefine | instruction | lineEnder
    # self.parser.ignore_whitespace()
    self.instrEncodings = self.getInstructionsFromYml(configFile)

  def _getSrcIntVal(self, srcVal: int|str) -> int:
    srcIntVal:int = 0
    if type(srcVal) is str:
      if srcVal not in self.labelDict:
        raise Exception(f"Label {srcVal} could not be solved properly. Did you define it?")
      srcIntVal = self.labelDict[srcVal]
    else:
      srcIntVal = srcVal  #type:ignore
    
    return srcIntVal
    
  def _getSrcEncoding(self, srcType:SrcType, srcVal:int|str) -> int:
    if srcType is SrcType.null:
      return 0
    
    srcIntVal = self._getSrcIntVal(srcVal)

    if srcType is SrcType.reg:
      return 0x8 | srcIntVal
    if srcType is SrcType.addr_reg:
      return 0x18 | srcIntVal
    elif srcType is SrcType.simd:
      return 0x10 | srcIntVal
    if srcType is SrcType.imm:
      return 0x1
    if srcType is SrcType.addr:
      return 0x2
    if srcType is SrcType.stack_base:
      return 0x4
    if srcType is SrcType.sp:
      return 0x3
    if srcType is SrcType.stack_size:
      return 0x5

  def _getParamValue(self, srcType, srcVal:int|str):
    if srcType in [SrcType.imm, SrcType.addr]:
      return self._getSrcIntVal(srcVal)
    
    return None

  def _writeInstr(self, fileHandle, instr: Instruction):
    src1Enc = self._getSrcEncoding(instr.src1Type, instr.src1)
    src2Enc = self._getSrcEncoding(instr.src2Type, instr.src2)
    src1Par = self._getParamValue(instr.src1Type, instr.src1)
    src2Par = self._getParamValue(instr.src2Type, instr.src2)
    if instr.opcode is not None:
      instrBytes = instr.opcode << 10 | src1Enc << 5 | src2Enc
      fileHandle.write(f"{instrBytes:04x}\n")
      if src1Par is not None:
        fileHandle.write(f"{src1Par:04x}\n")
      if src2Par is not None:
        fileHandle.write(f"{src2Par:04x}\n")
    else:
      if src2Par is None:
        fileHandle.write(f"{src1Par:04x}\n")
      else:
        for i in range(0, src2Par):
          fileHandle.write(f"{src1Par:04x}\n")

  def _handleInstr(self, fileHandle, instr:Instruction):
    self._writeInstr(fileHandle, instr)

  def assemble(self, inFile:Path, outFile: Path, printLabels: bool):
    if not inFile.exists():
      raise FileNotFoundError(f"File {inFile} does not exist")

    asmLines = []
    with open(inFile, "r") as fh:
      asmLines = fh.readlines()

    for index, line in enumerate(asmLines, 1):
      try:
        res = self.parser.parseString(line)
      except Exception as exc:
        newExc = Exception(f"Error when attempting to parse line {index}: {line.lstrip().rstrip()}\n{exc}")
        raise newExc

    #now the middle man (midInstrDict) is populated
    if ".boot" not in self.sectionDict:
      raise Exception(".boot section is not defined")

    self.labelDict.update(self.solveLabels(self.bootInstructions, self.bootStartAddr, 0x10000))
    self.labelDict.update(self.solveLabels(self.midInstrDict, self.codeStartAddr, self.bootStartAddr))
    
    if printLabels:
      for label in self.labelDict:
        if label[0] not in "0123456789":
          print(f"{label} -> #{self.labelDict[label]:04x}")

    with open(outFile, "w") as output:
      for label in self.bootInstructions:
        output.write(f"#{self.bootStartAddr:04x}\n")
        for instr in self.bootInstructions[label]:
          self._handleInstr(output, instr)

      for addr in self.excpVectors:
        output.write(f"#{addr:04x}\n")
        for instr in self.excpVectors[addr]:
          self._handleInstr(output, instr)

      for label in self.midInstrDict:
        output.write(f"#{self.labelDict[label]:04x}\n")
        for instr in self.midInstrDict[label]:
          self._handleInstr(output, instr)


app = Typer(no_args_is_help=True, add_completion=False, pretty_exceptions_show_locals=False)

@app.command()
def generateBinary(infile :Path = Option(help = "Path to input file"),
                   outFile:Path = Option(help = "Path to output file"),
                   printLabels:bool = Option(False, help = "If True, it prints the values assigned to the labels to standard output")
                   ):
  avengers = Assembler()
  try:
    avengers.assemble(infile, outFile, printLabels)
  except Exception as exc:
    print(exc)

if __name__ == "__main__":
  app()
