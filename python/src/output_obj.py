

import sys
import os



class CommandOutput:

    def __init__(self, exit_code: int = 0, stdout: str | None = None, stderr: str | None = None) -> None:
        
        self._exit_code: int = exit_code
        self._stdout: str | None = stdout
        self._stderr: str | None = stderr

    def exitcode(self) -> int:
        return self._exit_code

    def stdout(self) -> str | None:
        return self._stdout
    
    def stderr(self) -> str | None:
        return self._stderr
    

