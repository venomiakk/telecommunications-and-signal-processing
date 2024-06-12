import pyaudio
import wave

ISRECORDING = True

def set_recording():
    ISRECORDING = False

def save_recording():
    # Konfiguracja audio
    FORMAT = pyaudio.paInt16
    CHANNELS = 1
    RATE = 44100
    CHUNK = 1024
    RECORD_SECONDS = 10
    WAVE_OUTPUT_FILENAME = "recorded_audio.wav"

    audio = pyaudio.PyAudio()

    # Inicjalizacja nagrywania
    stream = audio.open(format=FORMAT, channels=CHANNELS,
                        rate=RATE, input=True,
                        frames_per_buffer=CHUNK)

    print("Nagrywanie...")

    frames = []

    for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)):
        data = stream.read(CHUNK)
        frames.append(data)

    print("Zakończono nagrywanie.")

    # Zatrzymanie nagrywania
    stream.stop_stream()
    stream.close()
    audio.terminate()

    # Zapisanie dźwięku do pliku WAV
    wf = wave.open(WAVE_OUTPUT_FILENAME, 'wb')
    wf.setnchannels(CHANNELS)
    wf.setsampwidth(audio.get_sample_size(FORMAT))
    wf.setframerate(RATE)
    wf.writeframes(b''.join(frames))
    wf.close()

def load_recording():
    # Wczytanie pliku WAV
    WAVE_OUTPUT_FILENAME = "recorded_audio.wav"
    wf = wave.open(WAVE_OUTPUT_FILENAME, 'rb')

    # Konfiguracja audio z pliku WAV
    FORMAT = pyaudio.paInt16
    CHANNELS = wf.getnchannels()
    RATE = wf.getframerate()
    CHUNK = 1024

if __name__ == "__main__":
    save_recording()