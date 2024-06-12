import tkinter as tk
import re


def test1():
    print("test1")

def test2(txt):
    print(txt)

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
    quantization_lvl.insert(0, "16")

    btn1 = tk.Button(text="Btn1", command=lambda: test1(), state="disabled")
    btn1.place(x=100, y=200)
    btn2 = tk.Button(text="Btn2", command=lambda: test2("asv"))
    btn2.place(x=100, y=300)
    
    root.mainloop() 


if __name__ == "__main__":
    show_interface()