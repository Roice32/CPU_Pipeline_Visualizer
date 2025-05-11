from PyQt5.QtWidgets import QWidget, QVBoxLayout, QTextEdit


# -----------------------------------------------------------------------------------------------------------------------------
class HelpTab(QWidget):
  parent = None

  # ---------------------------------------------------------------------------------------------------------------------------
  def __init__(self, parent):
    super().__init__()
    self.parent = parent

    layout = QVBoxLayout()

    # Help text content
    helpText = QTextEdit()
    helpText.setReadOnly(True)
    helpText.setText("""Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed euismod, sapien vel bibendum 
lacinia, nisl nunc ultrices nunc, vitae aliquam nisl nunc eget nunc. Donec auctor, nisl eget
aliquam tincidunt, nisl nunc ultrices nunc, vitae aliquam nisl nunc eget nunc. Donec auctor,
nisl eget aliquam tincidunt, nisl nunc ultrices nunc, vitae aliquam nisl nunc eget nunc.

Nulla facilisi. Donec auctor, nisl eget aliquam tincidunt, nisl nunc ultrices nunc, vitae 
aliquam nisl nunc eget nunc. Donec auctor, nisl eget aliquam tincidunt, nisl nunc ultrices 
nunc, vitae aliquam nisl nunc eget nunc.

Donec auctor, nisl eget aliquam tincidunt, nisl nunc ultrices nunc, vitae aliquam nisl nunc 
eget nunc. Donec auctor, nisl eget aliquam tincidunt, nisl nunc ultrices nunc, vitae aliquam
nisl nunc eget nunc.""")

    layout.addWidget(helpText)
    self.setLayout(layout)


# -----------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
  raise Exception("This module is not meant to be run directly.")
