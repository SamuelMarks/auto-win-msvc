import re

with open('src/posix-pthread.c', 'r') as f:
    c = f.read()

funcs = [
    ('void', 'InitializeSRWLock', '(void**)'),
    ('void', 'AcquireSRWLockExclusive', '(void**)'),
    ('void', 'ReleaseSRWLockExclusive', '(void**)'),
    ('void', 'AcquireSRWLockShared', '(void**)'),
    ('void', 'ReleaseSRWLockShared', '(void**)'),
    ('unsigned char', 'TryAcquireSRWLockExclusive', '(void**)'),
    ('unsigned char', 'TryAcquireSRWLockShared', '(void**)'),
    ('void', 'InitializeConditionVariable', '(void**)'),
    ('int', 'SleepConditionVariableSRW', '(void**, void**, unsigned long, unsigned long)'),
    ('void', 'WakeConditionVariable', '(void**)'),
    ('void', 'WakeAllConditionVariable', '(void**)'),
    ('int', 'InitOnceBeginInitialize', '(void**, unsigned long, int*, void**)'),
    ('int', 'InitOnceComplete', '(void**, unsigned long, void*)')
]

c = re.sub(r'__declspec\(dllimport\) void\*\s+WINAPI\s+InterlockedCompareExchangePointer.*?;', '', c)

for ret, name, args in funcs:
    pattern = r'__declspec\(dllimport\)\s+' + ret + r'\s+WINAPI\s+' + name + r'\s*\(.*?\);'
    c = re.sub(pattern, '', c)

    clean_args = args[1:-1].split(',')

    wrapper_args = []
    call_args = []
    arg_idx = 0
    for t in clean_args:
        t = t.strip()
        if t == 'void':
            wrapper_args.append('void')
        else:
            wrapper_args.append(f'{t} a{arg_idx}')
            call_args.append(f'a{arg_idx}')
            arg_idx += 1

    wrapper = f'''
typedef {ret} (WINAPI * PFN_{name}){args};
static {ret} WINAPI dyn_{name}({', '.join(wrapper_args)}) {{
    static PFN_{name} pfn = 0;
    static int init = 0;
    if (!init) {{
        void* mod = GetModuleHandleA("kernel32.dll");
        if (mod) pfn = (PFN_{name})GetProcAddress(mod, "{name}");
        init = 1;
    }}
    if (pfn) return pfn({', '.join(call_args)});
'''
    if ret != 'void':
        wrapper += '    return 0;\n'
    wrapper += '}\n'

    c = c.replace('typedef struct {', wrapper + '\ntypedef struct {', 1)

c = c.replace('__declspec(dllimport) unsigned long WINAPI GetCurrentThreadId(void);', '__declspec(dllimport) unsigned long WINAPI GetCurrentThreadId(void);\n__declspec(dllimport) void* WINAPI GetModuleHandleA(const char*);\n__declspec(dllimport) void* WINAPI GetProcAddress(void*, const char*);')

for ret, name, args in funcs:
    c = re.sub(r'(?<!dyn_)' + name + r'\(', f'dyn_{name}(', c)

with open('src/posix-pthread.c', 'w') as f:
    f.write(c)
