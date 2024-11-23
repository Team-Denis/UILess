

import argparse
import sys

from src.parser import Parser
from src.output_obj import CommandOutput
from src.pipeline_executor import Executor



def main() -> None:

    parser: argparse.ArgumentParser = argparse.ArgumentParser()
    parser.add_argument('-p', '--parallel', action='store_true', help="Run pipeline in parallel")
    args: argparse.Namespace = parser.parse_args()

    raw_data = sys.stdin.read()
    pipeline: list[list[str]] = Parser.run_parsing_from_raw(raw=raw_data)

    if args.parallel:
        out: list[CommandOutput] = Executor.execute_pipeline_concurrent(pipeline=pipeline)
    else:
        out: list[CommandOutput] = Executor.execute_pipeline(pipeline=pipeline)

    for r in out: print(r.serialize_json())



if __name__ == '__main__':

    # Get-Content .\tstcm.json | python .\python\main.py
    main()
