import json

with open('mappings.json') as f:
    data = json.load(f)

for f in data['mappings']['functions']:
    if f.get('shim', {}).get('type') == 'macro':
        code = f['shim']['code']
        if 'sleep(x)' in code:
            f['shim']['code'] = '#define sleep(x) (Sleep((x) * 1000), 0)'
        elif 'usleep(x)' in code:
            f['shim']['code'] = '#define usleep(x) (Sleep((x) / 1000), 0)'

with open('mappings.json', 'w') as f:
    json.dump(data, f, indent=2)

