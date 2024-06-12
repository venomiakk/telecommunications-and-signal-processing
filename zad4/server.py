import socket

def start_server():
    # Tworzenie gniazda TCP/IP
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Wiązanie gniazda z adresem IP i portem
    server_address = ('localhost', 65432)
    print(f'Starting server on {server_address[0]} port {server_address[1]}')
    server_socket.bind(server_address)

    # Nasłuchiwanie połączeń
    server_socket.listen(1)

    while True:
        print('Waiting for a connection...')
        connection, client_address = server_socket.accept()

        try:
            print(f'Connection from {client_address}')

            # Odbieranie danych w małych porcjach
            while True:
                data = connection.recv(1024)
                if data:
                    print(f'Received: {data.decode()}')
                    connection.sendall(data)  # Odesłanie danych do klienta (echo)
                else:
                    break

        finally:
            # Zamykanie połączenia
            connection.close()

if __name__ == "__main__":
    start_server()
