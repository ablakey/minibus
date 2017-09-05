#!/usr/bin/env python
from time import sleep
from pymodbus.client.sync import ModbusTcpClient


SERVER_IP = '192.168.1.5'


print 'Trying to connect to {}.'.format(SERVER_IP)
client = ModbusTcpClient(SERVER_IP)

coil_state = False

try:
    while True:

        is_dispatch = client.read_coils(102, 1)

        if is_dispatch.bits[0]:
            print "Asked to dispatch. Preparing dispatch..."
            sleep(5)
            print "Sending an acknowledge that dispatch is being prepared..."
            client.write_coil(104, True);
            sleep(5)
            print "Clearing ackwnoledge, dispatch is completed."
            client.write_coil(104, False);
        else:
            sleep(1)

        coil_state = not coil_state
        client.write_coil(100, coil_state)
finally:
    client.close()
