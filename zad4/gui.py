import tkinter as tk
from tkinter import messagebox, ttk
import re
import threading
import time
import audio
import receiver
import sender


HOST = 'localhost'
PORT = 65432

RT_BLOCK_BTN = threading.Event()


def only_int(P):
    if re.match("^[0-9]*$", P):
        return True
    return False


def show_interface():
      
    root = tk.Tk() 
    root.geometry("800x600")
    root.title("Przetwarzanie analogowo-cyfrowe") 
    canvas = tk.Canvas(root, height=600, width=800)
    canvas.pack()
    canvas.create_line(400, 100, 400, 500)

    validate_int = root.register(only_int)

    sign = tk.Label(root, text="Adrian Michałek, Hanna Mikołajczyk, Maksymilian Paluśkiewicz")
    sign.place(x=5, y=5)

    label_sr = tk.Label(root, text="Częstotliwość próbkowania [Hz]:")
    label_sr.place(x=50, y=50)

    sample_rate = tk.Entry(root, validate="key", validatecommand=(validate_int, '%P'))
    sample_rate.place(x=240, y=50)


    label_ql = tk.Label(root, text="Stopień kwantyzacji:")
    label_ql.place(x=450, y=50)

    quantization_lvl = tk.Entry(root, validate="key", validatecommand=(validate_int, '%P'))
    quantization_lvl.place(x=570, y=50)

    sample_rate.insert(0, "44100")
    quantization_lvl.insert(0, "8")
    
    #region Nagrywanie przyciski
    titleLabel1 = tk.Label(root, text="Nagrywanie\ndo pliku")
    titleLabel1.place(x=40, y=150)
    startRecBtn = tk.Button(text="Start", command=lambda: start_audio_recording())
    startRecBtn.place(x=50, y=200)
    stopRecBtn = tk.Button(text="Stop", command=lambda: stop_audio_recording())
    stopRecBtn.place(x=50, y=250)

    playRecBtn = tk.Button(text="Odtwórz", command=lambda: play_recorded_audio('recorded_audio.wav'))
    playRecBtn.place(x=40, y=300)
    
    sendRecBtn = tk.Button(text="Wyślij", command=lambda: sender.send_file(HOST, PORT))
    sendRecBtn.place(x=40, y=400)

    titleLabel4 = tk.Label(root, text="Czas rzeczywisty")
    titleLabel4.place(x=140, y=150)

    rtSendBtn = tk.Button(text="Wysyłaj", command=lambda: start_sending_audio())
    rtSendBtn.place(x=150, y=200)

    rtStopBtn = tk.Button(text="Stop", command=lambda: stop_sending_audio())
    rtStopBtn.place(x=150, y=250)

    rtSendPlayBtn = tk.Button(text="Odtwórz", command=lambda: play_recorded_audio('rt_audio_send.wav'))
    rtSendPlayBtn.place(x=150, y=300)
    #endregion

    #region Nagrywanie do pliku
    def start_audio_recording():
        startRecBtn.config(state="disabled")
        sr = int(sample_rate.get())
        ql = int(quantization_lvl.get())
        ql = 2 ** ql
        threading.Thread(target=audio.start_recording, args=(sr, ql)).start()


    def stop_audio_recording():
        startRecBtn.config(state="normal")
        audio.stop_recording()
        
        frame = tk.Frame(root, width=100, height=50, bg='#f0f0f0') #f0f0f0
        frame.place(x=10, y=340)
        
        # :(
        time.sleep(0.2)
        snr = audio.LAST_SNR
        snrLab = tk.Label(root, text=f"SNR: {snr} dB")
        snrLab.place(x=30, y=350)


    def play_recorded_audio(filename):
        played = audio.play_audio(filename)
        if not played:
            messagebox.showerror(f"Błąd", "Plik {filename} nie został znaleziony.")
    #endregion

    #region Wysylanie RT
    def start_sending_audio():
        rtSendBtn.config(state="disabled")
        sr = int(sample_rate.get())
        ql = int(quantization_lvl.get())
        ql = 2 ** ql
        threading.Thread(target=sender.audio_stream_send, args=(HOST, PORT, sr, ql)).start()
    
    def stop_sending_audio():
        sender.set_is_rt_sending()
        rtSendBtn.config(state="normal")

        frame = tk.Frame(root, width=100, height=50, bg='#f0f0f0') #f0f0f0
        frame.place(x=150, y=350)
        
        time.sleep(0.2)
        snr = sender.LAST_SNR
        snrLab = tk.Label(root, text=f"SNR: {snr} dB")
        snrLab.place(x=150, y=350)

    #endregion

    #region Odbieranie przyciski
    titleLabel2 = tk.Label(root, text="Odbieranie\npliku")
    titleLabel2.place(x=440, y=150)

    downloadRecBtn = tk.Button(text="Pobierz", command=lambda: receiver.download_file(HOST, PORT))
    downloadRecBtn.place(x=450, y=200)

    playDwnRecBtn = tk.Button(text="Odtwórz", command=lambda: play_recorded_audio('downloaded.wav'))
    playDwnRecBtn.place(x=450, y=250)

    titleLabel4 = tk.Label(root, text="Czas rzeczywisty")
    titleLabel4.place(x=540, y=150)

    rtReceiveBtn = tk.Button(text="Odbieraj", command=lambda: start_receiving_audio())
    rtReceiveBtn.place(x=550, y=200)

    rtReceivePlayBtn = tk.Button(text="Odtwórz", command=lambda: play_recorded_audio('rt_audio_rec.wav'))
    rtReceivePlayBtn.place(x=550, y=250)
    #endregion

    #region Odbieranie RT
    def start_receiving_audio():
        sr = int(sample_rate.get())
        ql = int(quantization_lvl.get())
        ql = 2 ** ql
        threading.Thread(target=receiver.audio_stream_receive, args=(HOST, PORT, sr, ql)).start()
        threading.Thread(target=block_receiving_btn).start()
        # block_receiving_btn()

    def block_receiving_btn():
        receiver.RT_BLOCK_BTN.set()
        while receiver.RT_BLOCK_BTN.is_set():
            rtReceiveBtn.config(state="disabled")
        
        rtReceiveBtn.config(state="normal")

        frame = tk.Frame(root, width=100, height=50, bg='#f0f0f0') #f0f0f0
        frame.place(x=550, y=300)
        
        time.sleep(0.2)
        snr = receiver.LAST_SNR
        snrLab = tk.Label(root, text=f"SNR: {snr} dB")
        snrLab.place(x=550, y=300)

    #endregion

    #region SNR
    selectR = ttk.Combobox(
        state="readonly",
        values=[" Nagranie", " Przesłane nagranie", " rt Nagranie", " rt Przesłane nagranie"],
        width=24
    )
    selectR.place(x=220, y=500)
    selectR.set("Nagranie")

    snrBtn = tk.Button(text="Oblicz SNR", command=lambda: calc_snr())
    snrBtn.place(x=250, y=540)
    
    def calc_snr():
        selected = selectR.current()  # Odczytaj wartość z comboboxa
        print(f"Selected value: {selected}")
        file = "recorded_audio.wav"
        if selected == 1:
            file = "downloaded.wav"
        elif selected == 2:
            file = "rt_audio_send.wav"
        elif selected == 3:
            file = "rt_audio_rec.wav"

        ql = int(quantization_lvl.get())
        ql = 2 ** ql

        snr = audio.calculate_snr_from_file(file, ql)
        #TODO sprawdzic snry
        frame = tk.Frame(root, width=100, height=50, bg='#f0f0f0') #f0f0f0
        frame.place(x=430, y=530)

        snrLab = tk.Label(root, text=f"SNR: {snr} dB")
        snrLab.place(x=430, y=530)
    #endregion

    root.mainloop() 


if __name__ == "__main__":
    show_interface()