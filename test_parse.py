#! /usr/bin/env python3

import subprocess

command_line = 'find .  -type f -print -maxdepth 2 | grep pdf | nl | wc ; who | grep $USER'
command_line_list = command_line.split()
command_line_list_length = len(command_line_list)

ok = 1

result = subprocess.run(['./parse', command_line], stdout=subprocess.PIPE).stdout.decode('utf-8')
# test vectors
#result = 'find .  -type f -print -maxdepth 2 | grep pdf | nl | wc ; who | grep $USER'
#result = 'find .  -type f -print -maxdepth 2 | grep pdf | nl | wc a'
result_list = result.split()
result_list_length = len(result_list)

if result_list_length != command_line_list_length:
    print('List of parsed items:', result_list_length,'is not correct length:', command_line_list_length )
    exit()

for val in range( result_list_length ):
    if (command_line_list[val] != result_list[val]):
        print('./parse returned ', result_list[val], ' which does not equal ', command_line_list[val])
        ok = 0


if (ok == 1 ):
    print ('OK')

    
    
    
