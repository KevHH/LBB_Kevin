
import threading
import sys
import time

class Ardlistener:
    '''
        Runs a background thread to listen to 
    '''
    def __init__(self):
        # dictionary with key = event name, value = a list of callback functions
        self.callbacks = {}
    
    def register(self, event_str, f):
        '''
            register callback function f under the given event
        '''
        if event_str in self.callbacks:
            self.callbacks[event_str].append(f)
        else:
            self.callbacks[event_str] = [f]

    def run(self,ser):
        '''
            Takes in a serial.Serial object to read from. Serial port assumed to be already open. 
            Executes callbacks when input of the form "ard&event_name&data" are detected
        '''
        self.thread = threading.Thread(target=self.read_loop, args=(self.callbacks, ser))
        self.thread.daemon = True
        self.thread.start()

    @staticmethod
    def read_loop(callbacks, ser): 
        while True:
            if (ser.inWaiting() > 0):
                input = ser.readline()
                args = input.decode('utf-8').split('&')
                print()
                print()
                print(args[0], file=sys.stdout)
                print()
                print()
                if len(args) == 3 and args[0] == "ard":
                    # valid serial input from arduino
                    event_str = args[1]
                    data = args[2]
                    if event_str in callbacks:
                        # event detected, execute all callbacks with data as the argument
                        for f in callbacks[event_str]:
                            f(data)
            time.sleep(0.100)
            
