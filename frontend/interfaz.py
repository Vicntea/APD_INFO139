import tkinter as tk
from tkinter import messagebox
import subprocess
import os

def probar_palabra():
    palabra = entrada.get()

    try:
        # Ruta al ejecutable del backend
        backend_path = os.path.abspath(os.path.join("..", "backend", "bin", "apd_simulator.exe"))

        result = subprocess.run(
            [backend_path],
            input=palabra.encode(),
            capture_output=True,
            timeout=5
        )

        salida = result.stdout.decode()
        messagebox.showinfo("Resultado", salida)

    except Exception as e:
        messagebox.showerror("Error", str(e))

root = tk.Tk()
root.title("Simulador de Autómata")

tk.Label(root, text="Ingresa palabra (a^n b^n):").pack(pady=5)
entrada = tk.Entry(root, width=30)
entrada.pack(pady=5)

tk.Button(root, text="Probar palabra", command=probar_palabra).pack(pady=10)

root.mainloop()
