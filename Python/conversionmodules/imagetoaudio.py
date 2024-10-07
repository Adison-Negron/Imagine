import numpy as np
from PIL import Image
import os
import glob
import scipy.io
import sys

def convolve(img_array, kernel, step=2):
    x = img_array.shape[0]
    y = img_array.shape[1]

    resultant_arr = []

    for i in range(0, x - kernel + 1, step):
        for j in range(0, y - kernel + 1, step):
            avg = np.mean(img_array[i:i+kernel, j:j+kernel])/10
            resultant_arr.append(avg)

    resultant_arr = np.array(resultant_arr)
    print(resultant_arr.shape)

    return resultant_arr

def generate_audio(arrays, out_path):
    sample_rate = 44800

    for i in range(len(arrays)):
        # Convert to radians
        array_radian = np.radians(arrays[i])
        
        # Generate sine and cosine wave
        array_sine = np.sin(array_radian)
        array_cosine = np.cos(array_radian)
        array_square = np.square(array_sine)
        
        # Combine sine and cosine to add variety
        combined_wave = array_sine + 0.5 * array_cosine
        


        # Normalize the combined wave to the range of int16 (to avoid distortion)
        max_value = np.max(np.abs(combined_wave))
        if max_value != 0:
            combined_wave_normalized = combined_wave / max_value
        else:
            combined_wave_normalized = combined_wave  # In case max_value is 0, keep it unchanged

        # Convert the normalized wave to int16
        combined_wave_int16 = np.int16(combined_wave_normalized * 32767)  # scale to int16
        sine_wave_int16 = np.int16(array_sine * 32767)  # scale to int16
        cosine_wave_int16 = np.int16(array_cosine * 32767)  # scale to int16
        square_wave_int16 = np.int16(array_square * 32767)  # scale to int16

        # Save as a .wav file in int16 format
        scipy.io.wavfile.write(out_path + "output_combined_" + str(i) + ".wav", sample_rate, combined_wave_int16)
        scipy.io.wavfile.write(out_path + "output_sine_" + str(i) + ".wav", sample_rate, sine_wave_int16)
        scipy.io.wavfile.write(out_path + "output_cosine_" + str(i) + ".wav", sample_rate, cosine_wave_int16)
        scipy.io.wavfile.write(out_path + "output_square_" + str(i) + ".wav", sample_rate, square_wave_int16)

    print("Generated audio files")

def imagetoaudio(img_path, out_path, kernel_size, step_size):
    Images = []
    for file in glob.glob(img_path + "*.jpg"):
        img = Image.open(file)
        img = img.convert('RGB')
        width, height = img.size
        aspect_ratio = width / height

        img = img.resize((int(aspect_ratio * 1024), 1024))  # Resize with consistent height
        img_array = np.asarray(img)

        Images.append(img_array)
        print(img_array.shape)

    convolution_results = []
    for i in range(len(Images)):
        print("Convolving image " + str(i) + " with Kernel Size " + str(kernel_size) + " and Step Size " + str(step_size))
        results = convolve(Images[i], kernel_size, step_size)
        convolution_results.append(results)

    generate_audio(convolution_results, out_path)

    print("Finished")
    return convolution_results


if __name__ == "__main__":
    
    """
    Converts an image to an audio file
    args:
    img_path
    out_path (optional)
    """

    debug = True

    if not debug:
        img_path = sys.argv[1]
        out_path = sys.argv[2] if len(sys.argv) > 2 else None
        kernel_size = int(sys.argv[3]) if len(sys.argv) > 3 else 2
        step_size = int(sys.argv[4]) if len(sys.argv) > 4 else 1

        print("args:", img_path, out_path, kernel_size, step_size)

        imagetoaudio(img_path, out_path, kernel_size, step_size)

    else:
        img_path = os.getcwd() + "/imgs/"
        out_path = os.getcwd() + "/output/"
        kernel_size = 100
        step_size = 5
        imagetoaudio(img_path, out_path, kernel_size, step_size)

        print("Done")
