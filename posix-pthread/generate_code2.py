import json
import re

with open('mappings.json', 'r') as f:
    data = json.load(f)

# (copying sigs here)
sigs = { ... }

from generate_code import sigs, extract_args
from gen_impl import impls

out_c = []
out_c.append('/* posix-pthread.c - Strict C89 Implementation */')
out_c.append('#include "posix-pthread.h"')
out_c.append('#include <errno.h>')
out_c.append('')
out_c.append('#ifndef ENOSYS')
out_c.append('#define ENOSYS 40')
out_c.append('#endif')
out_c.append('#ifndef ETIMEDOUT')
out_c.append('#define ETIMEDOUT 138')
out_c.append('#endif')
out_c.append('#ifndef EBUSY')
out_c.append('#define EBUSY 16')
out_c.append('#endif')
out_c.append('#ifndef EINVAL')
out_c.append('#define EINVAL 22')
out_c.append('#endif')
out_c.append('#ifndef EAGAIN')
out_c.append('#define EAGAIN 11')
out_c.append('#endif')
out_c.append('')
out_c.append('#if defined(_WIN32)')
out_c.append('#include <stdint.h>')
out_c.append('#endif')
out_c.append('')

for func in data['mappings']['functions']:
    name = func['posix']
    sig = sigs[name]
    is_void = sig.startswith('void ')
    is_ptr = sig.startswith('void *') or sig.startswith('sem_t *') or sig.startswith('pthread_t ')
    
    args = extract_args(sig)
    arg_casts = "".join(f"    (void){arg};\n" for arg in args)
    
    out_c.append(f"/* TODO: Implement {name} */")
    out_c.append(f"{sig} {{")
    
    if name in impls:
        out_c.append(impls[name])
    else:
        if arg_casts:
            out_c.append(arg_casts)
        if is_void:
            out_c.append("    return;")
        elif is_ptr:
            out_c.append("    return 0;")
        else:
            out_c.append("    return ENOSYS;")
    out_c.append("}\n")

with open('src/posix-pthread.c', 'w') as f:
    f.write('\n'.join(out_c))

