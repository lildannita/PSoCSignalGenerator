import numpy as np
import matplotlib.pyplot as plt

# Создание массива данных для синусоиды
num_points = 128  # Число точек в синусоиде

# Генерация синусоидальных значений от -1 до 1
sine_values = np.sin(np.linspace(0, 2 * np.pi, num_points))

# Минимальное и максимальное значения в новом диапазоне
min_val = 0x00
max_val = 0xFF

# Масштабирование значений синуса в новый диапазон
sine_wave_scaled = ((sine_values + 1) / 2) * (max_val - min_val) + min_val

# Преобразование значений в целые числа и форматирование в 8-битный формат
sine_wave_8bit = sine_wave_scaled.astype(np.uint8)
sine_wave_8bit.tolist()

sine_wave_hex = ['0x{:02X}'.format(val) for val in sine_wave_8bit]
line_width = 20
for i in range(0, len(sine_wave_hex), line_width):
    line = ', '.join(sine_wave_hex[i:i+line_width])
    if i + line_width < len(sine_wave_hex):
        line += ','
    print(line)

# Построение графика
plt.figure(figsize=(14, 7))
plt.plot(sine_wave_8bit, marker='o')
plt.title("Синусоидальный сигнал для VDAC")
plt.xlabel("Индекс")
plt.ylabel("Значение VDAC")
plt.grid(True)
plt.show()
