
'''

    Author :: Charles C. Stevenson (Diatomo)
    Date :: 08/02/2018

    Description :: 
        Simple server to receive a request to run an animation
        script.

'''

import subprocess
from bottle import route, run, request, response, get, run

@route('/<path>', method='GET')
def process(path):
    return subprocess.check_output(['python', path+'.py'])

run(host='localhost', port=8081, debug=True)
