

import json
import os
import requests
import subprocess




class EmojiGrabber:

    @staticmethod
    def json_safe_load(fp: str) -> dict | None:

        assert os.path.isfile(path=fp), f'[E] fp @ {fp} is not a file.'

        try:
            with open(fp, 'r') as f:
                d: dict | any = json.load(f)
            return d
        
        except FileNotFoundError:
            print(f"[E] Filepath @ '{fp}' doesn't exist.")
        except json.JSONDecodeError as e:
            print(f"[E] JSONDecodeError for '{fp}' -> ({e}).")
        except Exception as e:
            print(f'Unknown exception -> {e}.')

        return None
    
    @staticmethod
    def reformat_dict(emjmap: dict) -> dict:

        ret: dict = dict()
        
        if emjmap:
            for em in emjmap:

                asset_svg: str = em['asset']
                for n in em['names']:
                    ret[n.replace(':', '')] = asset_svg

        return ret

    @staticmethod
    def get_emoji_map(fp: str) -> dict:

        sfld: dict | None = EmojiGrabber.json_safe_load(fp=fp)
        dc: dict = EmojiGrabber.reformat_dict(emjmap=sfld)
        return dc
    
    @staticmethod
    def save_svg(svglink: str, dfp: str, name: str) -> None:
        
        uext: str = ".svg"
        u_dfp: str = os.path.normpath(os.path.join(dfp, f"{name}{uext}"))

        try:
            response = requests.get(svglink)
            response.raise_for_status()
            
            with open(u_dfp, 'wb') as file:
                file.write(response.content)
            
        except requests.exceptions.RequestException as e:
            print(f"[E] Error downloading {svglink} : {e}")

    @staticmethod
    def convert_every_svg_to_png(dst: str) -> None:
        
        for f in os.listdir(dst):

            cur: str = os.path.join(dst, f)
            png_cur_path: str = cur.replace('.svg', '.png')

            command: list = [
                "inkscape",
                cur,
                "--export-type=png",
                "--export-filename", png_cur_path]
            subprocess.run(command, check=True)

    @staticmethod
    def query_emoji(fp: str, query: list[str], dfp: str) -> None:
        
        # get the query map
        querymp: dict = dict()
        emjmap: dict = EmojiGrabber.get_emoji_map(fp=fp)
        for q in query: querymp[q] = emjmap[q]
        
        # download the queried emojis using svg format
        for n, p in querymp.items():
            EmojiGrabber.save_svg(svglink=p, dfp=dfp, name=n)
        # converting to png
        EmojiGrabber.convert_every_svg_to_png(dst=dfp)



# if __name__ == '__maison__':

if __name__ == '__main__':

    dscd_fp: str = f'./python/scripts/discord_emoji.json'
    EmojiGrabber.query_emoji(fp=dscd_fp, query=['joy', 'worm'], dfp='./python/scripts/dwld')
  
