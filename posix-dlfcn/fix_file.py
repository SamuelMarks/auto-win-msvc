import re

with open('src/posix-dlfcn.c', 'r') as f:
    text = f.read()

# 1. Change void set_dlerror to int set_dlerror
text = re.sub(r'static void set_dlerror\(DWORD err_code\) \{', r'static int set_dlerror(DWORD err_code) {', text)
text = re.sub(r'thread_dlerror_set = 0;\s*return;', r'thread_dlerror_set = 0;\n        return 0;', text)
text = re.sub(r'thread_dlerror_set = 1;\s*\}', r'thread_dlerror_set = 1;\n    return 0;\n}', text)

# 2. Add NUM_FORMAT macro after headers
num_format_macro = '''
#if defined(_MSC_VER) || defined(__MINGW32__)
#define NUM_FORMAT "%lu"
#else
#define NUM_FORMAT "%lu"
#endif
'''
if 'NUM_FORMAT' not in text:
    text = text.replace('#include <memoryapi.h>', '#include <memoryapi.h>\n' + num_format_macro)

# 3. Update sprintf_s / sprintf to use NUM_FORMAT
text = text.replace('"Unknown error code: %lu"', '"Unknown error code: " NUM_FORMAT')

# 4. Use memcpy_s if available for the trick
memcpy_trick_old = '''    /* Standard trick to avoid strict C89 warnings when casting function to void pointer */
    ret_ptr = NULL;
    memcpy(&ret_ptr, &proc, sizeof(proc));
    return ret_ptr;'''

memcpy_trick_new = '''    /* Standard trick to avoid strict C89 warnings when casting function to void pointer */
    ret_ptr = NULL;
#if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
    memcpy_s(&ret_ptr, sizeof(ret_ptr), &proc, sizeof(proc));
#else
    memcpy(&ret_ptr, &proc, sizeof(proc));
#endif
    return ret_ptr;'''

text = text.replace(memcpy_trick_old, memcpy_trick_new)

with open('src/posix-dlfcn.c', 'w') as f:
    f.write(text)
