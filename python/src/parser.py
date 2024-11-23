

import json
import itertools



class Parser:


    COMMAND_FLAG: str = 'cmd'
    ARGUMENTS_FLAG: str = 'args'
    STREAM_FLAG: str = 'stream'
    START_COMMAND_FLAG: str = 'stcmd'
    INTERMEDIATE_COMMAND_FLAG: str = 'mdcmd'
    END_COMMAND_FLAG: str = 'edcmd'
    FILEWRITE_COMMAND: str = 'FILEWRITE'
    PIPELINE_FLAG: str = 'pipeline'


    @staticmethod
    def load_json(fp) -> any:
        # JSON Safe load from Pipeline
        with open(fp, 'r') as f:
            d: any = json.load(f)
        return d
    
    @staticmethod
    def load_json_from_raw(raw: str) -> any:
        d: any = json.loads(raw)
        return d

    @staticmethod
    def format_command(cmdict: dict) -> list:

        cmd: str        = cmdict.get(Parser.COMMAND_FLAG    , None)
        args: list[str] = cmdict.get(Parser.ARGUMENTS_FLAG  , None)
        stream: int     = cmdict.get(Parser.STREAM_FLAG     , None)

        res: list = list()
        # Cas de commande classique (cmd != FILEWRITE_COMMAND, (args), stream)
        if cmd and cmd != Parser.FILEWRITE_COMMAND:
            res += [cmd] + args
        
        # Cas de commande de fin
        elif cmd == Parser.FILEWRITE_COMMAND:
            if stream == 0 or not stream:
                res += ['>'] + args
            elif stream == 2:
                res += ['2>'] + args

        return res

    @staticmethod
    def parse_pipeline_to_commands(pipeline: dict) -> list[list[str]]:

        commands: list = list()

        for cmstruct in pipeline:

            stflow: list = Parser.format_command(cmdict=cmstruct.get(Parser.START_COMMAND_FLAG, dict()))
            mdflow: list = [['|'] + Parser.format_command(cmdict=m) for m in cmstruct.get(Parser.INTERMEDIATE_COMMAND_FLAG, dict())]
            edflow: list = Parser.format_command(cmdict=cmstruct.get(Parser.END_COMMAND_FLAG, dict()))

            flow: list = stflow + mdflow + edflow
            flat_flow: list = list(itertools.chain.from_iterable(sb if isinstance(sb, list) else [sb] for sb in flow))
            
            commands.append(flat_flow)
        
        return commands
    
    @staticmethod
    def run_parsing(fp: str) -> list[list[str]]:  
        rd: list[dict] = Parser.load_json(fp=fp)
        c: list[list[str]] = Parser.parse_pipeline_to_commands(pipeline=rd[Parser.PIPELINE_FLAG])
        return c

    @staticmethod
    def run_parsing_from_raw(raw: str) -> list[list[str]]:
        rd: list[dict] = Parser.load_json_from_raw(raw=raw)
        c: list[list[str]] = Parser.parse_pipeline_to_commands(pipeline=rd[Parser.PIPELINE_FLAG])
        return c



