import can

bus = None
try:
    bus = can.interface.Bus(bustype='socketcan', channel='can0')
    print("✅ Connected to CAN bus (can0). Listening for messages...")

    while True:
        msg = bus.recv(timeout=1.0)
        if msg:
            if msg.arbitration_id == 0x201:
                print(msg.data[4:6].hex())

except KeyboardInterrupt:
    print("\n🛑 Stopped by user.")
finally:
    if bus:
        bus.shutdown()
    print("CAN bus closed cleanly.")
