import argparse
import sys
import json

from src.parser import Parser
from src.outputObj import CommandOutput
from src.pipelineExecutor import Executor

def main() -> None:
    parser: argparse.ArgumentParser = argparse.ArgumentParser()
    parser.add_argument('-p', '--parallel', action='store_true', help="Run pipeline in parallel")
    parser.add_argument('-f', '--file', type=str, help="JSON file input")
    args: argparse.Namespace = parser.parse_args()

    if args.file:
        # Load JSON from file
        rd = Parser.load_json(fp=args.file)
        c = Parser.parse_pipeline_to_commands(pipeline=rd[Parser.PIPELINE_FLAG])
        pipeline = c
    else:
        raw_data = sys.stdin.read()
        pipeline: list[list[str]] = Parser.run_parsing_from_raw(raw=raw_data)

    if args.parallel:
        out: list[CommandOutput] = Executor.execute_pipeline_concurrent(pipeline=pipeline)
    else:
        out: list[CommandOutput] = Executor.execute_pipeline(pipeline=pipeline)

    if args.parallel:
        # Collect all outputs into a list of dictionaries
        results = []
        for r in out:
            cmd_output_dict = {
                "exit_code": r.exitcode,
                "stdout": r.stdout,
                "stderr": r.stderr
            }
            results.append(cmd_output_dict)
    else:
        # Single pipeline output as a single dictionary
        results = []
        if out:
            r = out[0]
            cmd_output_dict = {
                "exit_code": r.exitcode,
                "stdout": r.stdout,
                "stderr": r.stderr
            }
            results = cmd_output_dict

        # Serialize and print the results
    print(json.dumps(results, ensure_ascii=False, indent=4))

if __name__ == '__main__':
    main()