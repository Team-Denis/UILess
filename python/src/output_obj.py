

import sys
import os



class CommandOutput:

    def __init__(self, exit_code: int = 0, stdout: str | None = None, stderr: str | None = None) -> None:
        
        self._exit_code: int = exit_code
        self._stdout: str | None = stdout
        self._stderr: str | None = stderr

    @property
    def exitcode(self) -> int:
        return self._exit_code
    
    @property
    def stdout(self) -> str | None:
        return self._stdout
    
    @property   
    def stderr(self) -> str | None:
        return self._stderr
    
    def __repr__(self) -> str:
        return (f"CommandOutput(exit_code={self.exitcode}, "
                f"stdout={repr(self.stdout)}, "
                f"stderr={repr(self.stderr)})")
    

