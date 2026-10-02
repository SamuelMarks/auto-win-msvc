import sys
import os
import json
import urllib.request
import re
from html import unescape

def strip_html(html):
    # Remove HTML tags
    text = re.sub(r'<[^>]+>', '', html)
    # Remove extra spaces
    text = re.sub(r'[ \t]+', ' ', text)
    return text.strip()

def scrape_function(func_name, header_name):
    urls_to_try = [
        f"https://man7.org/linux/man-pages/man3/{func_name}.3p.html",
        f"https://man7.org/linux/man-pages/man2/{func_name}.2.html",
        f"https://man7.org/linux/man-pages/man3/{func_name}.3.html"
    ]

    html = None
    for url in urls_to_try:
        try:
            html = urllib.request.urlopen(url).read().decode('utf-8')
            break
        except Exception as e:
            continue

    if not html:
        print(f"Failed to fetch {func_name} from any known man page URLs.")
        return None

    pres = re.findall(r'<pre>(.*?)</pre>', html, re.DOTALL)

    # Heuristic: the SYNOPSIS is usually the first <pre> block after the SYNOPSIS heading
    # or typically pres[3] in man7.org html.
    # A more robust way: find index of SYNOPSIS header
    synopsis = ""
    description = ""

    synopsis_match = re.search(r'<a id="SYNOPSIS".*?</h2>\s*<pre>(.*?)</pre>', html, re.DOTALL)
    if synopsis_match:
        raw_syn = unescape(synopsis_match.group(1))
        # Extract the prototype for the function
        lines = raw_syn.split('\n')
        proto_lines = []
        in_func = False
        for line in lines:
            if func_name + '(' in strip_html(line):
                proto_lines.append(line)
                in_func = True
                if ';' in line:
                    break
            elif in_func:
                proto_lines.append(line)
                if ';' in line:
                    break
        synopsis = ' '.join(strip_html(l) for l in proto_lines)

    desc_match = re.search(r'<a id="DESCRIPTION".*?</h2>\s*<pre>(.*?)</pre>', html, re.DOTALL)
    if desc_match:
        raw_desc = unescape(desc_match.group(1))
        # First paragraph: split by double newline
        paragraphs = raw_desc.split('\n\n')
        if paragraphs:
            description = strip_html(paragraphs[0])
            # replace newlines with space
            description = description.replace('\n', ' ')

    if synopsis and description:
        return {
            "prototype": synopsis,
            "description": description
        }
    return None

def main():
    if len(sys.argv) < 3:
        print("Usage: python scrape_manpages.py <header.h> <func1> <func2> ...")
        sys.exit(1)

    header = sys.argv[1]
    funcs = sys.argv[2:]

    os.makedirs('specs', exist_ok=True)
    out_file = os.path.join('specs', header.replace('/', '_').replace('.h', '.json'))

    data = {}
    if os.path.exists(out_file):
        with open(out_file, 'r') as f:
            data = json.load(f)

    if header not in data:
        data[header] = {}

    for func in funcs:
        print(f"Scraping {func}...")
        res = scrape_function(func, header)
        if res:
            data[header][func] = res
        else:
            # Add a stub to avoid failing
            data[header][func] = {
                "prototype": f"int {func}(void);",
                "description": f"Documentation for {func} not available."
            }

    with open(out_file, 'w') as f:
        json.dump(data, f, indent=4)

    print(f"Saved to {out_file}")

if __name__ == '__main__':
    main()
