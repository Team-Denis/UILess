

from src.parser import Parser
from src.output_obj import CommandOutput
from src.pipeline_executor import Executor



if __name__ == '__main__':
    
    c = Parser.run_parsing(fp=r'C:\Users\Antoine\Desktop\Odoo2024\python\test\tstcm.json')
    o = Executor.execute_pipeline(pipeline=c)
    print(o)