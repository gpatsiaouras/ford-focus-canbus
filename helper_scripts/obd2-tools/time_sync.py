import threading
import time

import can
import obd

bus = can.interface.Bus(bustype='socketcan', channel='can0')

connection = obd.OBD()
cmd_speed = obd.commands.SPEED


class TimeSync:
    def __init__(self):
        self.speed_obd = 0.0
        self.speed_can = b""
        self.threads = []
        self.should_stop = False

    def record_speed_from_obd(self):
        while True:
            response_speed = connection.query(cmd_speed)
            self.speed_obd = response_speed.value.m
            if self.should_stop:
                break

    def record_speed_from_can(self):
        while True:
            msg = bus.recv()
            if msg is not None:
                if msg.arbitration_id == 0x201:
                    self.speed_can = msg.data[4:6]
            if self.should_stop:
                break

    def _get_log_string(self):
        return f"{time.time()} {self.speed_obd} {self.speed_can.hex()}\n"

    def log_to_file(self):
        with open("time_sync.log", "w") as log:
            while True:
                log.write(self._get_log_string())
                if self.should_stop:
                    break

    def log_to_stream(self):
        while True:
            print(self._get_log_string())
            time.sleep(0.2)
            if self.should_stop:
                break

    def start(self):
        self.threads.append(threading.Thread(target=self.record_speed_from_obd).start())
        self.threads.append(threading.Thread(target=self.record_speed_from_can).start())
        self.threads.append(threading.Thread(target=self.log_to_file).start())
        self.threads.append(threading.Thread(target=self.log_to_stream).start())

    def is_running(self):
        for thread in self.threads:
            if thread.is_alive():
                return True

    def stop(self):
        self.should_stop = True


def main():
    sync = TimeSync()

    sync.start()
    try:
        time.sleep(60)
    except KeyboardInterrupt:
        print("Stopping by user")
        pass
    sync.stop()
    bus.shutdown()


if __name__ == '__main__':
    main()
