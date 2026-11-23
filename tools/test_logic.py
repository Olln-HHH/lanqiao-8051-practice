"""Run edited C logic on a PC with fake registers/1-Wire responses, not a board test."""
from pathlib import Path
import argparse, re, subprocess, tempfile

ROOT = Path(__file__).resolve().parents[1]

def function(path, name):
    text = (ROOT / path).read_text(encoding="utf-8")
    match = re.search(r"\b(?:void|bit)\s+" + name + r"\([^;{}]*\)\s*(?:interrupt\s+\d+\s*)?\{", text)
    if not match:
        raise ValueError(f"Function not found: {path}: {name}")
    depth, end = 1, text.index("{", match.start()) + 1
    while depth:
        depth += (text[end] == "{") - (text[end] == "}")
        end += 1
    return re.sub(r"\s+interrupt\s+\d+", "", text[match.start():end])

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cc", default="gcc", help="GCC-compatible host C compiler")
    args = parser.parse_args()
    extracted = "\n".join([
        function("03-past-exams/16-provincial-v3/main.c", "get_V"),
        function("01-modules/10-pwm/main.c", "servicetimer0"),
        function("02-exercises/02-key-long-short/main.c", "Server_Timer0"),
        function("03-past-exams/16-provincial-v3/onewire.c", "OneWire_ReadRaw"),
    ])
    # int is mapped to a 16-bit short for these isolated C51 functions.
    # Actual firmware compilation still uses Keil C51; SFRs and bit are mocked here.
    harness = r'''
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#define int short
#define bit unsigned char
_Static_assert(sizeof(unsigned int)==2,"16-bit arithmetic required");
_Static_assert(sizeof(unsigned long)==4,"32-bit long required");
unsigned char sample,V_val,count,pwm_duty,L1,press,TH0,TL0;
unsigned int V_int;
unsigned char AD_read(unsigned char channel){(void)channel;return sample;}
unsigned char scratch[9]={0x50,0x05,0x4b,0x46,0x7f,0xff,0x0c,0x10,0x1c};
signed char index_byte=-1;
unsigned char busy=0,missing=0,starts=0;
bit init_ds18b20(void){return missing;}
void Write_DS18B20(unsigned char dat){
    if(dat==0xbe){index_byte=0;}
    if(dat==0x44){starts++;index_byte=-1;}
}
unsigned char Read_DS18B20(void){
    if(index_byte>=0){return scratch[index_byte++];}
    return busy?0:0xff;
}
'''+extracted+r'''
#undef int
int main(void){
    int i,duty,on;
    short raw=1234;
    sample=0;get_V(1);assert(V_int==0);
    sample=128;get_V(1);assert(V_int==251);
    sample=255;get_V(1);assert(V_int==500);
    for(i=0;i<256;i++){sample=(unsigned char)i;get_V(1);assert(V_int==(i*500L+127)/255);}
    puts("PASS ADC: all 256 input codes, including full scale");
    for(duty=0;duty<=100;duty++){
        pwm_duty=(unsigned char)duty;count=0;on=0;
        for(i=0;i<100;i++){servicetimer0();on+=(L1==0);}
        assert(on==duty);
    }
    puts("PASS PWM: all 101 duty levels and period wrap");
    count=0;press=1;
    for(i=0;i<10000;i++){Server_Timer0();}
    assert(count==101);
    puts("PASS long press: saturation after 10000 timer ticks");
    assert(!OneWire_ReadRaw(&raw)&&raw==1234&&starts==1);
    busy=1;assert(!OneWire_ReadRaw(&raw)&&raw==1234&&starts==1);
    busy=0;assert(OneWire_ReadRaw(&raw)&&raw==0x0550&&starts==2);
    scratch[8]^=1;
    assert(!OneWire_ReadRaw(&raw)&&raw==0x0550);
    missing=1;assert(!OneWire_ReadRaw(&raw)&&raw==0x0550);
    puts("PASS DS18B20: start, busy, known CRC vector, corrupt data, disconnected sensor");
    return 0;
}
'''
    with tempfile.TemporaryDirectory(prefix="lanqiao-logic-") as temp:
        src=Path(temp)/"logic.c";exe=Path(temp)/"logic.exe"
        src.write_text(harness,encoding="utf-8")
        subprocess.run([args.cc,"-std=c11","-O2","-Wall","-Wextra","-Werror",str(src),"-o",str(exe)],check=True)
        subprocess.run([str(exe)],check=True)

if __name__ == "__main__":
    main()
