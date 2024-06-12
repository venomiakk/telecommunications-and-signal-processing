import socket

def start_client():
    # Tworzenie gniazda TCP/IP
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Połączenie z serwerem
    server_address = ('localhost', 65432)
    print(f'Connecting to {server_address[0]} port {server_address[1]}')
    client_socket.connect(server_address)

    try:
        # Wysyłanie danych
        message = 'This is a test message.'
        print(f'Sending: {message}')
        client_socket.sendall(message.encode())

        # Odbieranie odpowiedzi
        data = client_socket.recv(1024)
        print(f'Received: {data.decode()}')

    finally:
        # Zamykanie połączenia
        client_socket.close()

if __name__ == "__main__":
    start_client()
