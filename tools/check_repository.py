"""Check project dependencies, local documentation links and packaged firmware hashes."""
from pathlib import Path
import hashlib,json,re,xml.etree.ElementTree as ET

ROOT=Path(__file__).resolve().parents[1]

def main():
    projects=json.loads((ROOT/'projects.json').read_text(encoding='utf-8'))
    errors=[]
    for rec in projects:
        folder=ROOT/rec['path'];proj=folder/'project.uvproj'
        for el in ET.parse(proj).findall('.//FilePath'):
            path=(folder/el.text.replace('\\','/')).resolve()
            if not path.is_relative_to(ROOT) or not path.is_file():errors.append(f'Missing/outside project source: {rec["path"]}: {el.text}')
        local={p.name.lower() for p in folder.rglob('*.h')}
        builtin={'regx52.h','reg52.h','reg51.h','intrins.h','absacc.h','stdio.h','string.h','math.h','stdlib.h'}
        for source in list(folder.rglob('*.c'))+list(folder.rglob('*.h')):
            for include in re.findall(r'^\s*#include\s*[<"]([^>"\n]+)[>"]',source.read_text(encoding='utf-8'),re.M):
                if include.lower() not in local|builtin:errors.append(f'Missing include: {source.relative_to(ROOT)}: {include}')
        if not (folder/'README.md').is_file():errors.append(f'Missing README: {rec["path"]}')
    for p in ROOT.rglob('*.md'):
        for url in re.findall(r'\]\(([^)]+)\)',p.read_text(encoding='utf-8')):
            if '://' in url or url.startswith('#'):continue
            target=(p.parent/url.split('#')[0]).resolve()
            if not target.exists():errors.append(f'Broken link: {p.relative_to(ROOT)}: {url}')
    for rec in json.loads((ROOT/'firmware/manifest.json').read_text(encoding='utf-8')):
        p=ROOT/rec['file']
        if not p.is_file() or hashlib.sha256(p.read_bytes()).hexdigest()!=rec['sha256']:errors.append(f'Firmware hash mismatch: {rec["file"]}')
    for error in errors:print('ERROR',error)
    if errors:raise SystemExit(1)
    print(f'PASS {len(projects)} projects: source references, headers, README links and firmware SHA-256')

if __name__=='__main__':main()
