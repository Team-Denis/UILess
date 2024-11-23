

import subprocess
from src.output_obj import CommandOutput
import sys


class Executor:

    @staticmethod
    def execute_pipeline(pipeline: list[list[str]]) -> list[CommandOutput]:
        
        ret: list[CommandOutput] = list()
        is_windows = sys.platform.startswith("win")

        for command in pipeline:
            
            strcmd: str = ' '.join(command)

            if is_windows:
                if '|' in strcmd or '>' in strcmd:
                    res: subprocess.CompletedProcess = subprocess.run(["powershell", "-Command", strcmd], text=True, capture_output=True)
                else:
                    res: subprocess.CompletedProcess = subprocess.run(strcmd, shell=True, text=True, capture_output=True)
            else:
                res: subprocess.CompletedProcess = subprocess.run(strcmd, shell=True, text=True, capture_output=True)

            cmdopt: CommandOutput = CommandOutput(exit_code=res.returncode, stdout=res.stdout, stderr=res.stderr)
            ret.append(cmdopt)

        return ret

    

    