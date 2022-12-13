import socket

def simple_local_client():
    with socket.create_connection((socket.gethostbyname('localhost'), 8000)) as client:
        msg = client.recv(1024).decode()
        print('Message from server: ' + msg)
        try:
            while msg != 'exit':
                msg = input('Input >>> ')
                n_sent = client.send(msg.encode(encoding='utf-8'))
                # This doesn't seem to work when the server disconnects
                # Maybe a timeout is more appropriate. While the while() loop is
                # running, no exception gets thrown. Maybe that's because the
                # send() doesn't check whether the message arrives. But it _does_
                # in theory, because it returns the number of bytes sent. Oh well.
                if n_sent == 0:
                    print('Message failed to send, exiting.')
                    break
        except KeyboardInterrupt:
            print(' Keyboard interrupt received. Exiting from client.')

def manual_local_client():
    # Steps:
    # 1. Create socket
    # 2. Bind socket to local network
    # 3. Request connection to server network
    # 4. Send and receive messages
    # 5. Close socket
    # TODO: Surround this with 'with:'
    sckt = socket.socket(
            family=socket.AF_INET,
            type=socket.SOCK_STREAM,
            proto=0)

    # (host, port)
    # port=0 indicates "any free port". For the client, port doesn't matter.
    sckt.bind(('localhost', 0))
    # (host, port)
    sckt.connect(('localhost', 8000))

    # Block until server confirms connection
    msg = sckt.recv(1024).decode()
    print('Message from server: ' + msg)

    try:
        while True:
            msg = input('Input >>> ')
            if msg == 'exit':
                sckt.send(''.encode())
                break
            n_sent = sckt.send(msg.encode(encoding='utf-8'))
    except KeyboardInterrupt:
        print(' Keyboard interrupt received. Exiting from client.\nTip: You can exit by typing "exit".')
    # Clean up. This is automatic with 'with: ' statement.
    sckt.shutdown(socket.SHUT_RDWR)
    sckt.close()
    

    
if __name__=='__main__':
    manual_local_client()
