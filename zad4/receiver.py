import socket
import pyaudio
import wave
import threading
import audio

CHUNK = 1024
FORMAT = pyaudio.paInt16
CHANNELS = 1

RT_BLOCK_BTN = threading.Event()

def download_file(host, port, filename='downloaded.wav'):
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((host, port))

    with open(filename, 'wb') as f:
        while True:
            data = client_socket.recv(1024)
            if not data:
                break
            f.write(data)

    client_socket.close()
    print(f"Plik {filename} został pobrany")

def audio_stream_receive(host, port, sample_rate, quantization_lvl, filename="rt_audio_rec.wav"):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((host, port))
    server_socket.listen(1)
    print(f"Serwer nasłuchuje na {host}:{port}")

    conn, addr = server_socket.accept()
    print(f"Połączono z {addr}")

    p = pyaudio.PyAudio()
    stream = p.open(format=FORMAT,
                    channels=CHANNELS,
                    rate=sample_rate,
                    output=True,
                    frames_per_buffer=CHUNK)

    frames = []
    try:
        while True:
            data = conn.recv(CHUNK)
            if not data:
                break
            # data = audio.quantize(data, quantization_lvl)
            stream.write(data)
            frames.append(data)
    except KeyboardInterrupt:
        pass

    stream.stop_stream()
    stream.close()
    p.terminate()
    conn.close()
    server_socket.close()
    RT_BLOCK_BTN.clear()
    print("Serwer zakończył działanie")

    audio_data = b''.join(frames)

    wf = wave.open(filename, 'wb')
    wf.setnchannels(CHANNELS)
    wf.setsampwidth(p.get_sample_size(FORMAT))
    wf.setframerate(sample_rate)
    # wf.writeframes(quantized_audio.tobytes())
    wf.writeframes(audio_data)
    wf.close()


if __name__ == "__main__":
    download_file('localhost', 65432, 'downloaded.wav')
