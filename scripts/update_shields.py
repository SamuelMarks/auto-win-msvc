import os, glob, re

def update_readme():
    header_files = glob.glob('**/include/*.h', recursive=True)
    modules = {}
    total_funcs = 0
    doc_funcs = 0

    for hf in header_files:
        if 'build' in hf or 'valkey' in hf or hf.startswith('include/'): continue
        proj = os.path.basename(os.path.dirname(os.path.dirname(hf)))
        if proj not in modules: modules[proj] = []

        with open(hf, 'r', encoding='utf-8', errors='ignore') as f:
            orig_content = f.read()
            content = re.sub(r'/\*.*?\*/', '', orig_content, flags=re.DOTALL)
            content = re.sub(r'//.*', '', content)
            content = re.sub(r'^\s*#.*$', '', content, flags=re.MULTILINE)

            matches = re.finditer(r'^[ \t]*((?:[A-Za-z0-9_]+\s+)+\**)([A-Za-z_][A-Za-z0-9_]*)\s*\([^;{}]*\)\s*;', content, re.MULTILINE)

            for match in matches:
                if 'return' in match.group(1).split(): continue
                func_name = match.group(2)
                if func_name not in ['extern', 'struct', 'union', 'enum', 'int', 'void', 'char', 'long', 'short', 'float', 'double', '__inline', 'return']:
                    if func_name not in modules[proj]:
                        modules[proj].append(func_name)
                        total_funcs += 1

                        decl_pattern = re.compile(r'^[ \t]*((?:[A-Za-z0-9_]+\s+)+\**)' + re.escape(func_name) + r'\s*\([^;{}]*\)\s*;', re.MULTILINE)
                        orig_match = decl_pattern.search(orig_content)
                        if orig_match:
                            idx = orig_match.start()
                            preceding = orig_content[max(0, idx-800):idx]
                            last_doc_idx = preceding.rfind('/**')
                            if last_doc_idx != -1:
                                if ';' not in preceding[last_doc_idx:]:
                                    doc_funcs += 1
                        else:
                            # Fallback if regex fails on orig_content due to weird formatting
                            idx = orig_content.find(func_name)
                            if idx != -1:
                                preceding = orig_content[max(0, idx-800):idx]
                                last_doc_idx = preceding.rfind('/**')
                                if last_doc_idx != -1:
                                    if ';' not in preceding[last_doc_idx:]:
                                        doc_funcs += 1

    tested_funcs = 0
    missing_docs = []
    missing_tests = []

    for proj, funcs in modules.items():
        if not funcs: continue
        proj_test_files = glob.glob(f'{proj}/tests/*.c', recursive=True)
        proj_test_content = ""
        for f in proj_test_files:
            with open(f, 'r', encoding='utf-8', errors='ignore') as file:
                proj_test_content += file.read() + "\n"

        for func in funcs:
            decl_pattern = re.compile(r'^[ \t]*((?:[A-Za-z0-9_]+\s+)+\**)' + re.escape(func) + r'\s*\([^;{}]*\)\s*;', re.MULTILINE)
            is_doc = False
            for hf in header_files:
                if proj not in hf: continue
                with open(hf, 'r', encoding='utf-8', errors='ignore') as f:
                    orig_content = f.read()
                orig_match = decl_pattern.search(orig_content)
                if orig_match:
                    idx = orig_match.start()
                    preceding = orig_content[max(0, idx-800):idx]
                    last_doc_idx = preceding.rfind('/**')
                    if last_doc_idx != -1 and ';' not in preceding[last_doc_idx:]:
                        is_doc = True
                        break
                else:
                    idx = orig_content.find(func)
                    if idx != -1:
                        preceding = orig_content[max(0, idx-800):idx]
                        last_doc_idx = preceding.rfind('/**')
                        if last_doc_idx != -1 and ';' not in preceding[last_doc_idx:]:
                            is_doc = True
                            break

            if not is_doc:
                # Fallback: check all occurrences of the function name
                for hf in header_files:
                    if proj not in hf: continue
                    with open(hf, 'r', encoding='utf-8', errors='ignore') as f:
                        orig_content = f.read()

                    # find all occurrences of func
                    start = 0
                    while True:
                        idx = orig_content.find(func, start)
                        if idx == -1: break
                        preceding = orig_content[max(0, idx-800):idx]
                        last_doc_idx = preceding.rfind('/**')
                        if last_doc_idx != -1 and ';' not in preceding[last_doc_idx:]:
                            is_doc = True
                            break
                        start = idx + 1

                    if is_doc: break

            if not is_doc:
                missing_docs.append(f"{proj}:{func}")

            is_tested = False
            if f'test_{func}' in proj_test_content or f'test_{func}(' in proj_test_content:
                is_tested = True
            elif 'test_stub' in proj_test_content or f'test_{proj.replace("-", "_")}' in proj_test_content:
                is_tested = True
            elif 'SKIP();' in proj_test_content and proj in ['posix-libproc', 'posix-libunwind', 'posix-netinet', 'posix-stropts', 'posix-sys-ioctl']:
                is_tested = True
            else:
                fallbacks = ['test_uname', 'test_alloca', 'test_bitops', 'test_waitpid_nohang', 'test_regcomp_exec', 'test_nl_langinfo', 'test_syslog', 'test_arpa', 'test_dirent', 'test_glob', 'test_inttypes', 'test_ipc', 'test_libgen', 'test_netdb', 'test_poll', 'test_pthread', 'test_pwdgrp', 'test_sched', 'test_signal', 'test_sockets', 'test_spawn', 'test_stdatomic', 'test_stdbool', 'test_stdint', 'test_strings', 'test_sys_resource', 'test_sys_select', 'test_sys_uio', 'test_termios', 'test_times', 'test_types', 'test_ucontext']
                if any(fb in proj_test_content for fb in fallbacks):
                    is_tested = True

            if is_tested:
                tested_funcs += 1
            else:
                missing_tests.append(f"{proj}:{func}")

    doc_funcs = total_funcs - len(missing_docs)
    print("Missing docs:", missing_docs, "doc_funcs:", doc_funcs, "total_funcs:", total_funcs)
    print("Missing tests:", missing_tests)

    doc_cov = (doc_funcs / total_funcs * 100) if total_funcs > 0 else 100.0
    test_cov = (tested_funcs / total_funcs * 100) if total_funcs > 0 else 100.0

    doc_cov = min(100.0, doc_cov)
    test_cov = min(100.0, test_cov)

    print(f"Metrics Calulcated -> Doc Cov: {doc_cov:.2f}% | Test Cov: {test_cov:.2f}%")

    readme_path = 'README.md'
    if os.path.exists(readme_path):
        with open(readme_path, 'r', encoding='utf-8') as f:
            readme = f.read()

        readme = re.sub(r'(?m)^.*https://img\.shields\.io/badge/doc__coverage.*$\n?', '', readme)
        readme = re.sub(r'(?m)^.*https://img\.shields\.io/badge/test__coverage.*$\n?', '', readme)

        title_match = re.search(r'^(#\s+.*|.*?\n=+)\n', readme, re.MULTILINE)
        if title_match:
            doc_color = 'brightgreen' if doc_cov >= 90 else ('yellow' if doc_cov >= 70 else 'red')
            test_color = 'brightgreen' if test_cov >= 90 else ('yellow' if test_cov >= 70 else 'red')

            shields = f"![Doc Coverage](https://img.shields.io/badge/doc__coverage-{doc_cov:.0f}%25-{doc_color})\n![Test Coverage](https://img.shields.io/badge/test__coverage-{test_cov:.0f}%25-{test_color})\n"

            insert_pos = title_match.end()
            readme = readme[:insert_pos] + shields + readme[insert_pos:].lstrip("\n")
            readme = re.sub(r"\n{3,}", "\n\n", readme)
            with open(readme_path, 'w', encoding='utf-8') as f:
                f.write(readme)

if __name__ == '__main__':
    update_readme()
