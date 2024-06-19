import socket
import pyaudio
import wave
import audio
import numpy as np

LAST_SNR = -1.0

CHUNK = 1024
FORMAT = pyaudio.paInt16
CHANNELS = 1

IS_RT_SENDING = True
def set_is_rt_sending():
    global IS_RT_SENDING
    IS_RT_SENDING = False


#Funkcja wysylajaca plik audio
def send_file(host, port, filename='recorded_audio.wav'):
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((host, port))
    print(f"Połączono z serwerem {host}:{port}")

    with open(filename, 'rb') as f:
        while (chunk := f.read(1024)):
            client_socket.sendall(chunk)

    client_socket.close()
    print(f"Plik {filename} został wysłany do {host}:{port}")


# Funkcja wysylajaca dzwiek w czasie rzeczywistym
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
    frames_q = []
    try:
        while IS_RT_SENDING:
            data = stream.read(CHUNK)
            client_socket.sendall(data)
            frames.append(data)
            frames_q.append(data)
    except KeyboardInterrupt:
        pass

    stream.stop_stream()
    stream.close()
    p.terminate()
    client_socket.close()
    print("Klient zakończył działanie")

    audio_data = b''.join(frames)
    audio_data_q = b''.join(frames_q)
    audio_data_q = audio.quantize(audio_data_q, quantization_lvl)
    snr = audio.calculate_snr(audio_data, audio_data_q, quantization_lvl)
    global LAST_SNR
    LAST_SNR = np.round(snr, 2)

    wf = wave.open(filename, 'wb')
    wf.setnchannels(CHANNELS)
    wf.setsampwidth(p.get_sample_size(FORMAT))
    wf.setframerate(sample_rate)
    # wf.writeframes(quantized_audio.tobytes())
    wf.writeframes(audio_data)
    wf.close()


if __name__ == "__main__":
    send_file('localhost', 65432, 'recorded_audio.wav')
