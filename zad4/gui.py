import tkinter as tk
from tkinter import messagebox
import re
import threading
import time
import audio


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

    label_sr = tk.Label(root, text="Częstotliwość próbkowania [Hz]:")
    label_sr.place(x=50, y=50)

    sample_rate = tk.Entry(root, validate="key", validatecommand=(validate_int, '%P'))
    sample_rate.place(x=240, y=50)


    label_ql = tk.Label(root, text="Poziom kwantyzacji:")
    label_ql.place(x=450, y=50)

    quantization_lvl = tk.Entry(root, validate="key", validatecommand=(validate_int, '%P'))
    quantization_lvl.place(x=570, y=50)

    sample_rate.insert(0, "44100")
    quantization_lvl.insert(0, "256")
    

    startRecBtn = tk.Button(text="Start", command=lambda: start_audio_recording())
    startRecBtn.place(x=100, y=200)
    stopRecBtn = tk.Button(text="Stop", command=lambda: stop_audio_recording())
    stopRecBtn.place(x=100, y=250)

    playRecBtn = tk.Button(text="Odtwórz", command=lambda: play_recorded_audio())
    playRecBtn.place(x=100, y=300)
    
    def start_audio_recording():
        startRecBtn.config(state="disabled")
        sr = int(sample_rate.get())
        ql = int(quantization_lvl.get())
        threading.Thread(target=audio.start_recording, args=(sr, ql)).start()


    def stop_audio_recording():
        startRecBtn.config(state="normal")
        audio.stop_recording()
        
        frame = tk.Frame(root, width=100, height=100, bg='#f0f0f0')
        frame.place(x=140, y=190)
        
        # :(
        time.sleep(0.2)
        snr = audio.LAST_SNR
        snrLab = tk.Label(root, text=f"SNR: {snr} dB")
        snrLab.place(x=150, y=200)


    def play_recorded_audio():
        played = audio.play_audio()
        if not played:
            messagebox.showerror("Błąd", "Plik 'recorded_audio.wav' nie został znaleziony.")

    root.mainloop() 


if __name__ == "__main__":
    show_interface()