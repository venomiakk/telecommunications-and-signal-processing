import socket
import pyaudio
import wave
import audio

CHUNK = 1024
FORMAT = pyaudio.paInt16
CHANNELS = 1

IS_RT_SENDING = True
def set_is_rt_sending():
    global IS_RT_SENDING
    IS_RT_SENDING = False



def send_file(host, port, filename='recorded_audio.wav'):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((host, port))
    server_socket.listen(1)
    print(f"Serwer nasłuchuje na {host}:{port}")

    while True:
        conn, addr = server_socket.accept()
        print(f"Połączono z {addr}")

        with open(filename, 'rb') as f:
            while (chunk := f.read(1024)):
                conn.sendall(chunk)

        conn.close()
        print(f"Plik {filename} został wysłany do {addr}")
    
        server_socket.close()
        break


def audio_stream_send(host, port, sample_rate, quantization_lvl, filename="rt_audio_send.wav"):
    print(sample_rate, quantization_lvl)
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((host, port))

    global IS_RT_SENDING
    IS_RT_SENDING = True

    p = pyaudio.PyAudio()
    stream = p.open(format=FORMAT,
                    channels=CHANNELS,
                    rate=sample_rate,
                    input=True,
                    frames_per_buffer=CHUNK)

    frames = []
    try:
        while IS_RT_SENDING:
            data = stream.read(CHUNK)
            client_socket.sendall(data)
            frames.append(data)
    except KeyboardInterrupt:
        pass

    stream.stop_stream()
    stream.close()
    p.terminate()
    client_socket.close()
    print("Klient zakończył działanie")

    audio_data = b''.join(frames)

    wf = wave.open(filename, 'wb')
    wf.setnchannels(CHANNELS)
    wf.setsampwidth(p.get_sample_size(FORMAT))
    wf.setframerate(sample_rate)
    # wf.writeframes(quantized_audio.tobytes())
    wf.writeframes(audio_data)
    wf.close()


if __name__ == "__main__":
    send_file('localhost', 65432, 'recorded_audio.wav')
