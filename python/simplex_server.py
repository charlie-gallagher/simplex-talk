import socket
import datetime

def print_server_header():
        print('Welcome to the socket TCP server!')
        print('\nInstructions:')
        print(' - Wait for a connection')
        print(' - Messages will be received automatically\n')

def log(msg):
    now = datetime.datetime.now()
    print(f'{now.isoformat()} {msg}')


def simple_local_server():
    print_server_header()
    with socket.create_server((socket.gethostbyname('localhost'), 8000)) as s_srv:
        try:
            while True:
                log('Waiting for a connection...')
                # It's not strictly necessary to split conn and addr, but 
                # it's convenient
                # `conn` is a socket object
                # `addr` is a tuple (hostaddr, port)
                conn, addr = s_srv.accept()
                log('Connected to <' + str(addr[0]) + ':' + str(addr[1]) + '>')
                conn.send('Hello my new connection!'.encode())
                msg = 'init'
                while True:
                    msg = conn.recv(1024).decode()
                    log('Message: ' + str(msg))
                    if msg == '':
                        break
                conn.close()
        except KeyboardInterrupt:
            log(' Keyboard interrupt received. Exiting from server.')


def manual_local_server():
    print_server_header()
    sckt = socket.socket(
            family=socket.AF_INET,
            type=socket.SOCK_STREAM,
            proto=0)

    # Bind
    srv_port = 8000
    log(f'Listening on port <{srv_port}>')
    sckt.bind(('localhost', srv_port))

    # Listen
    sckt.listen(5)

    # Accept connections
    try:
        while True:
            log('Waiting for connections...')
            conn, addr = sckt.accept()
            log('Accepted connection request from <' + str(addr[0]) + ':' + str(addr[1]) + '>')

            # Send pseudo-handshake
            log('Sending pseudo-handshake')
            conn.send('Connection accepted by simplex-server'.encode())
            log('Pseudo-handshake accepted')

            log('Listening for messages')
            while True:
                msg = conn.recv(1024).decode()
                if msg == '':
                    break
                log(f'Message: {msg}')
            conn.close()
    except KeyboardInterrupt:
        log('\nKeyboard interrupt encountered, exiting')
    sckt.close()

    



if __name__=='__main__':
    manual_local_server()
