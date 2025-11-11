unique_lines = []

previous_speed = []
with open("time_sync.log", "r") as original_file:
    for line in original_file.readlines():
        parts = line.split(" ")
        speed = parts[1:3]
        if speed != previous_speed:
            unique_lines.append(line)
        previous_speed = speed

with open("time_sync_fixed.log", "w") as new_file:
    for line in unique_lines:
        new_file.write(line)
