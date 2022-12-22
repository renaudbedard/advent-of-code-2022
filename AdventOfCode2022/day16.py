import sys
import re
import time
from collections import deque

start_time = time.time()

valves = {}
flow_rates = {}

for line in sys.stdin:
  tokens = line.strip().split(' ')

  valve = tokens[1]
  flow_rate = int(re.findall(r'\d+', tokens[4])[0])
  neighbors = set([node for node in ''.join(tokens[9:]).split(',')])

  valves[valve] = neighbors
  flow_rates[valve] = flow_rate

# memoization helper
def memoize(fn):
  memo = {}

  def memozied(*args):
    if args in memo:
      return memo[args]
    else:
      result = fn(*args)
      memo[args] = result
      return result

  return memozied

# BFS the valve graph to return the shortest path length
# betweent two valves
@memoize
def shorest_path_length(start, end):
  q = deque([[start]])
  visited = set()

  if start == end:
    return len([start]) - 1

  while len(q) > 0:
    path = q.popleft()
    node = path[-1]

    if node not in visited:
      neighbors = valves[node]
      
      for neighbor in neighbors:
        new_path = list(path)
        new_path.append(neighbor)
        q.append(new_path)

        if neighbor == end:
          return len(new_path) - 1
      
      visited.add(node)
  
  return None

# DFS of every possible order we could open the valves in
#
# cut down on the search space by eliminating valves with flow rate of 0
# we will never stop at those valves to open them
def should_try_to_open_valve(valve):
  return flow_rates[valve] != 0

valves_to_open = list(filter(should_try_to_open_valve, valves.keys()))

start_location = 'AA'
time_limit = 30
maximum_pressure = 0

# a stack of our current branches - [path], minutes_elapsed, { valve: <minute opened> }
s = [[[start_location], 0, {}]]

while len(s):
  path, minutes_elapsed, open_valves = s.pop()
  current_valve = path[-1]

  # if we've opened all the valves, calculate the maximum pressure
  # if we've run out of time, calculate the pressure and abandon this "branch"
  if minutes_elapsed >= time_limit or len(path) == len(valves_to_open) + 1:
    pressure_released = 0

    for valve, minute_opened in open_valves.items():
      minutes_opened = max(time_limit - minute_opened, 0)
      pressure_released += flow_rates[valve] * minutes_opened

    maximum_pressure = max(maximum_pressure, pressure_released)
  else:
    for next_valve in valves_to_open:
      if next_valve not in open_valves.keys():
        # each edge in the graph takes 1 minute to cross
        # so our travel time to the next valve we're trying to open
        # is the length of the shortest path
        # taking anything longer than the shortest possible path
        # would obviously be sub-optimal
        travel_time = shorest_path_length(current_valve, next_valve)

        # plus we'll stop at the valve for an additional minute to open it
        time_to_open_valve = 1

        new_minutes_elapsed = minutes_elapsed + travel_time + time_to_open_valve

        new_open_valves = open_valves.copy()
        new_open_valves[next_valve] = new_minutes_elapsed

        new_path = list(path)
        new_path.append(next_valve)

        s.append([new_path, new_minutes_elapsed, new_open_valves])

print(maximum_pressure)
print("%s seconds" % (time.time() - start_time))