import argparse
import pickle
import socket
import threading
import time


class Item:
    DYNAMIC = 0
    STATIC = 1

    def __init__(self, next_node, distance):
        self.next_node = next_node
        self.distance = distance
        self.type = Item.DYNAMIC

    def __hash__(self):
        return hash(self.next_node) + hash(self.distance)

    def __str__(self):
        return f'{self.next_node} - {self.distance}'


class RouteTable:
    def __init__(self, self_port, inf):
        self.inf = inf
        self.table = {self_port: Item(next_node='self', distance=0)}

    def update_table(self, table, neighbor_port):
        updated = False
        for port, item in table.table.items():
            distance_new = min(self.inf, item.distance + 3)
            if port not in self.table:
                self.table[port] = Item(neighbor_port, distance_new)
                updated = True
            else:
                if self.table[port].type == Item.STATIC:
                    continue
                if self.table[port].next_node == neighbor_port:
                    if distance_new != self.table[port].distance:
                        self.table[port].distance = distance_new
                        updated = True
                else:
                    if distance_new < self.table[port].distance:
                        self.table[port].next_node = neighbor_port
                        self.table[port].distance = distance_new
                        updated = True
        return updated

    def lookup(self, port):
        if port in self.table:
            return self.table[port].next_node, \
                self.table[port].distance,
        else:
            return None

    def offline(self, port):
        for dst_port, item in self.table.items():
            if dst_port == port or item.next_node == port:
                self.table[dst_port].distance = self.inf

    def __getitem__(self, router):
        return self.table[router]

    def __len__(self):
        return len(self.table)

    def __str__(self):
        line = '  ' + '-' * 25
        head = '  | Router  NextHop  Dist |\n  |-----------------------|'
        rows = [line, head]
        for port, item in self.table.items():
            next_node = item.next_node
            dist = item.distance if item.distance < self.inf else 'inf'
            rows.append(f'  | {port:^6}  {next_node:^7}  {dist:^4} |')
        rows.append(line)
        return '\n'.join(rows)


class DVR:
    def __init__(self, port, neighbors, verbose):
        self.port = port
        self.verbose = verbose
        self.ip = '127.0.0.1'
        self.send_table_interval = 3
        self.timeout_interval = 5
        self.inf = 8
        self.buffer_size = 1024
        self.neighbors = {port: None for port in neighbors}
        self.route_table = RouteTable(self.port, self.inf)
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.socket.bind((self.ip, self.port))
        print(f'DVR Bind on {self.port}, Neighbors: {list(self.neighbors)}')
        print('Interact mode:', 'off' if self.verbose else 'on')
        print(f'Init routing table:\n{self.route_table}')

    def start(self):
        threads = [
            threading.Thread(target=self.send),
            threading.Thread(target=self.recv),
        ]
        if not self.verbose:
            threads.append(threading.Thread(target=self.interact))
        for thread in threads:
            thread.start()
        for thread in threads:
            thread.join()
        self.socket.close()

    def interact(self):
        while True:
            commands = input().split()
            try:
                self.parse_commands(commands)
            except Exception as e:
                print('Command error!')
                print(e)

    def parse_commands(self, commands):
        commands[0].upper()
        if commands[0] == 'T':  # Display current routing table
            print('Current routing table:')
            print(self.route_table)
        elif commands[0] == 'N':  # Display current active neighbors
            print('Current active neighbors:')
            print([port for port, t in self.neighbors.items() if t])
        elif commands[0] == 'D':  # Send data to a router
            port = int(commands[1])
            next_node, distance = self.route_table.lookup(port)
            if next_node is None or distance == self.route_table.inf:
                print(f'[Data] {port} is unreachable')
            elif next_node == 'self':
                print(f'[Data] Send data to itself')
            else:
                data = {
                    'type': 'data',
                    'src': self.port,
                    'dst': port,
                    'path': [self.port],
                }
                self.socket.sendto(pickle.dumps(data), (self.ip, next_node))
                print(f'[Data] Send data to next hop {next_node}')
                print(f'[Data] Path: {self.port}')
        elif commands[0] == 'R':  # Refuse a neighbor
            port = int(commands[1])
            if port not in self.neighbors:
                print(f'Router {port} is not a neighbor')
                print('Neighbors:', list(self.neighbors))
                return
            if self.route_table[port].type == Item.STATIC:
                print(f'Router {port} has already been refused')
            else:
                self.route_table[port].type = Item.STATIC
                self.route_table.offline(port)
                print(f'Refuse router {port}')
                print(f'Routing table updated\n{self.route_table}')
        elif commands[0] == 'A':  # Make a neighbor dynamic
            port = int(commands[1])
            if port not in self.neighbors:
                print(f'Router {port} is not a neighbor')
                print('Neighbors:', list(self.neighbors))
                return
            if self.route_table[port].type == Item.DYNAMIC:
                print(f'Router {port} has already been dynamic')
            else:
                self.route_table[port].type = Item.DYNAMIC
                print(f'Make router {port} dynamic')
        else:
            print(f'Unkonwn command {commands[0]}')
            print('Available commands: T, N, D, R, A, P')

    def send(self):
        while True:
            for port in self.neighbors:
                data = {
                    'type': 'table',
                    'content': self.route_table,
                }
                self.socket.sendto(pickle.dumps(data), (self.ip, port))
            if self.verbose and any(self.neighbors.values()):
                print(f'[Send] Send routing table to {list(self.neighbors)}')
            time.sleep(self.send_table_interval)

    def recv(self):
        print(f'[Recv] Listening on {self.port} ...')
        while True:
            data, (ip, port) = self.socket.recvfrom(self.buffer_size)
            data = pickle.loads(data)
            if data['type'] == 'table':  # Receive a routing table
                route_table = data['content']
                if self.verbose:
                    print(f'[Recv] Receive routing table from {port}')
                self.reset_timer(port)
                updated = self.route_table.update_table(route_table, port)
                if updated:
                    print(f'[Recv] Routing table updated\n{self.route_table}')
            elif data['type'] == 'data':  # Receive a data package
                print(f'[Data] Receive data from {data["src"]}')
                if data['dst'] == self.port:  # The router is the dst
                    data['path'].append(self.port)
                    path = '->'.join(map(str, data['path']))
                    print(f'[Data] Final path: {path}')
                else:  # Forward data to next hop
                    next_node, distance = self.route_table.lookup(data['dst'])
                    if next_node is None or distance == self.route_table.inf:
                        print(f'[Data] {port} is unreachable')
                    else:
                        data['path'].append(self.port)
                        self.socket.sendto(pickle.dumps(
                            data), (self.ip, next_node))
                        print(f'[Data] Forward data to next hop {port}')
                        path = '->'.join(map(str, data['path']))
                        print(f'[Data] Path: {path}')

    def reset_timer(self, port):
        if port not in self.neighbors:
            return
        if self.neighbors[port] is not None:
            self.neighbors[port].cancel()
        else:
            print(f'[Alive] Router {port} online')

        def timeout(port):
            print(f'[Alive] Router {port} offline')
            self.neighbors[port] = None
            self.route_table.offline(port)
            print(f'[Alive] Routing table updated\n{self.route_table}')
        timer = threading.Timer(
            interval=self.timeout_interval, function=timeout, args=(port,))
        timer.start()
        self.neighbors[port] = timer


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Distance Vector Routing')
    parser.add_argument('port', type=int)
    parser.add_argument('neighbor_ports', type=int, nargs='+')
    parser.add_argument('--verbose', '-v', action='store_true')
    args = parser.parse_args()
    dvr = DVR(args.port, args.neighbor_ports, args.verbose)
    dvr.start()
