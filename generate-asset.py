files = [
    ['index.html', 'index_page'],
    ['page2.html', 'page2_html'],
    ['script.js', 'script_js'],
    ['cat2.jpg', 'cat2_jpg'],
]

with open('assets.h', 'w') as out_file:
    out_file.write('/* This file is auto generated, do not edit */\n\n')
    for filename, var_name in files:
        with open(filename, 'rb') as in_file:
            data = in_file.read()
            out_file.write(f'static const char {var_name}[] PROGMEM = {{')
            for byte in data:
                out_file.write(hex(byte))
                out_file.write(', ')
            out_file.write('};\n')
