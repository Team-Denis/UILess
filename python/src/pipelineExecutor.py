import sys
import subprocess

from concurrent.futures import ThreadPoolExecutor, as_completed
from src.outputObj import CommandOutput

class Executor:
    @staticmethod
    def execute_command(command: list[str]) -> CommandOutput:
        is_windows: bool = sys.platform.startswith("win")

        command = [x.replace(" ", "\\ ") for x in command]

        strcmd: str = ' '.join(command)

        if is_windows:
            if '|' in strcmd or '>' in strcmd:
                res: subprocess.CompletedProcess = subprocess.run(["powershell", "-Command", strcmd], text=True, capture_output=True)
            else:
                res: subprocess.CompletedProcess = subprocess.run(strcmd, shell=True, text=True, capture_output=True)
        else:
            res: subprocess.CompletedProcess = subprocess.run(strcmd, shell=True, text=True, capture_output=True)

        cmdopt: CommandOutput = CommandOutput(exit_code=res.returncode, stdout=res.stdout, stderr=res.stderr)
        return cmdopt

    @staticmethod
    def execute_pipeline(pipeline: list[list[str]]) -> list[CommandOutput]:
        ret: list[CommandOutput] = [Executor.execute_command(command=c) for c in pipeline]
        return ret
    
    @staticmethod
    def execute_pipeline_concurrent(pipeline: list[list[str]]) -> list[CommandOutput]:
        ret: list[CommandOutput] = list()
        
        with ThreadPoolExecutor() as executor:
            futures = {executor.submit(Executor.execute_command, command): command for command in pipeline}
            
            for future in as_completed(futures):
                ret.append(future.result())
                
        return ret