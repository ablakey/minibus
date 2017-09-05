#!/usr/bin/env python
from time import sleep
from pymodbus.client.sync import ModbusTcpClient


SERVER_HOSTNAME = 'minibus'


print 'Trying to connect to {}.'.format(SERVER_HOSTNAME)
client = ModbusTcpClient(SERVER_HOSTNAME)


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
finally:
    client.close()
