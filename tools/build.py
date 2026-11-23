"""Windows + Keil C51: rebuild one or all independent practice projects."""
from pathlib import Path
import argparse, json, os, re, subprocess, time

ROOT=Path(__file__).resolve().parents[1]

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--keil',default=os.environ.get('KEIL_UV4'),help='Path to UV4.exe')
    parser.add_argument('--project',default='',help='Directory from projects.json; omit to build all')
    args=parser.parse_args()
    if not args.keil or not Path(args.keil).is_file():
        parser.error('Specify --keil with the installed Keil C51 UV4.exe path')
    projects=json.loads((ROOT/'projects.json').read_text(encoding='utf-8'))
    selected=[p for p in projects if not args.project or p['path']==args.project]
    if not selected:parser.error('Unknown project directory')
    failed=[]
    for p in selected:
        folder=ROOT/p['path'];build=folder/'build';build.mkdir(exist_ok=True)
        log=build/'compile.log';firmware=build/'firmware.hex'
        if log.exists():log.unlink()
        started=time.time()
        result=subprocess.run([args.keil,'-r',str(folder/'project.uvproj'),'-t','Practice','-j0','-o',str(log)],timeout=120)
        raw=log.read_bytes() if log.exists() else b''
        try:text=raw.decode('utf-8-sig')
        except UnicodeDecodeError:text=raw.decode('gb18030',errors='replace')
        summary=re.search(r'(\d+) Error\(s\), (\d+) Warning\(s\)',text)
        ok=bool(summary and summary[1]=='0' and result.returncode in [0,1] and firmware.exists() and firmware.stat().st_mtime>=started-3)
        print(('PASS ' if ok else 'FAIL ')+p['path']+' | '+(summary[0] if summary else 'See build/compile.log'),flush=True)
        if not ok:failed.append(p['path'])
    raise SystemExit(1 if failed else 0)

if __name__=='__main__':main()
