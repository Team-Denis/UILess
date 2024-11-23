

from src.parser import Parser
from src.output_obj import CommandOutput
from src.pipeline_executor import Executor



if __name__ == '__main__':
    
    pipeline: list[list[str]] = Parser.run_parsing(fp=r'python\test\tstcm.json')
    out: list[CommandOutput] = Executor.execute_pipeline_concurrent(pipeline=pipeline)
    for r in out: print(r)

