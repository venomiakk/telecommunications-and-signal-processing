import pyaudio
import wave
import numpy as np

IS_RECORDING = False
CHUNK = 1024
FORMAT = pyaudio.paInt16

LAST_SNR = -1.0


def quantize(data, levels):
    data = np.frombuffer(data, dtype=np.int16)
    max_val = np.max(data)
    min_val = np.min(data)
    delta =  (max_val - min_val) / (levels - 1)
    quantized = delta * np.floor((data/delta) + 0.5)
    return quantized.astype(np.int16)


def dequantize(quantized_data, levels):
    # np type?
    X_min = np.min(quantized_data)
    X_max = np.max(quantized_data)
    delta = (X_max - X_min) / (levels - 1)
    dequantized = delta * (np.floor((quantized_data / delta) + 0.5) - 0.5)
    return dequantized.astype(np.int16)


def calculate_snr(original_signal, quantized_signal):
    original_signal = np.frombuffer(original_signal, dtype=np.int16)
    signal_power = np.mean(np.square(original_signal))
    noise = original_signal - quantized_signal
    noise_power = np.mean(np.square(noise))
    snr = 10 * np.log10(signal_power / noise_power)
    print(f"SNR: {snr}")
    return snr


def stop_recording():
    global IS_RECORDING
    IS_RECORDING = False
    print("Nagrywanie zakończone.")


def start_recording(sample_rate, quantization_lvl):
    CHANNELS = 1
    RATE = sample_rate
    WAVE_OUTPUT_FILENAME = "recorded_audio.wav"

    audio = pyaudio.PyAudio()

    stream = audio.open(format=FORMAT, channels=CHANNELS,
                        rate=RATE, input=True,
                        frames_per_buffer=CHUNK)

    print(f"Nagrywanie, {sample_rate}Hz, {quantization_lvl} poziomow")

    frames = []
    global IS_RECORDING
    IS_RECORDING = True

    while IS_RECORDING:
        data = stream.read(CHUNK)
        frames.append(data)

    print("Zakończono nagrywanie.")

    stream.stop_stream()
    stream.close()
    audio.terminate()

    audio_data = b''.join(frames)
    quantized_audio = quantize(audio_data, quantization_lvl)
    snr = calculate_snr(audio_data, quantized_audio)
    global LAST_SNR
    LAST_SNR = np.round(snr, 2)

    wf = wave.open(WAVE_OUTPUT_FILENAME, 'wb')
    wf.setnchannels(CHANNELS)
    wf.setsampwidth(audio.get_sample_size(FORMAT))
    wf.setframerate(RATE)
    wf.writeframes(quantized_audio.tobytes())
    wf.close()



def play_audio():
    try:
        wf = wave.open('recorded_audio.wav', 'rb')
    except FileNotFoundError:
        print("Plik 'recorded_audio.wav' nie został znaleziony.")
        return False

    p = pyaudio.PyAudio()

    stream = p.open(format=p.get_format_from_width(wf.getsampwidth()),
                    channels=wf.getnchannels(),
                    rate=wf.getframerate(),
                    output=True)

    data = wf.readframes(CHUNK)
    while len(data) > 0:
        stream.write(data)
        data = wf.readframes(CHUNK)

    stream.stop_stream()
    stream.close()

    p.terminate()

    return True

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
    print("audio module")