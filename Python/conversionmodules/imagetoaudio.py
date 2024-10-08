import numpy as np
from PIL import Image
import os
import glob
import scipy.io
import sys
import random
from scipy import signal

def convolve(img_array,out_path, kernel, step=2):
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

# Function to ensure all arrays have the same dimension
def ensure_same_dimension(arrays):
    # Find maximum size
    max_size = max(arr.shape[0] for arr in arrays)
    
    # Pad or truncate arrays to the same size
    adjusted_arrays = []
    for arr in arrays:
        if arr.shape[0] < max_size:
            adjusted_arrays.append(np.pad(arr, (0, max_size - arr.shape[0]), mode='constant'))
        else:
            adjusted_arrays.append(arr[:max_size])  # Truncate if larger
    
    return adjusted_arrays
    

def generate_rgb(img_array, out_path, kernel, step=2, intensity=0.9):
    x = img_array.shape[0]
    y = img_array.shape[1]

    red_values = img_array[:, :, 0]
    green_values = img_array[:, :, 1]
    blue_values = img_array[:, :, 2]

    rgb_dict = {'1': [], '2': [], '3': []}

    # Calculate average values for each channel
    for i in range(0, x - kernel + 1, step):
        for j in range(0, y - kernel + 1, step):
            avg_red = (np.mean(red_values[i:i + kernel, j:j + kernel]))+0.000000000001 / 256
            avg_green = np.mean(green_values[i:i + kernel, j:j + kernel])+0.000000000001 / 256
            avg_blue = np.mean(blue_values[i:i + kernel, j:j + kernel])+0.000000000001 / 256
            
            rgb_dict['1'].append(avg_red)
            rgb_dict['2'].append(avg_green)
            rgb_dict['3'].append(avg_blue)

    # Compute overall average for each color channel
    avg_red_overall = np.mean(rgb_dict['1'])
    avg_green_overall = np.mean(rgb_dict['2'])
    avg_blue_overall = np.mean(rgb_dict['3'])

    # Find dominant color channel
    dominant_color = max([('red', avg_red_overall), 
                          ('green', avg_green_overall), 
                          ('blue', avg_blue_overall)], key=lambda x: x[1])[0]
    
    weights = [.5, .2, .1]

    templates = {
        'pad': [weights[2], weights[1], weights[0]],
        'lead': [weights[0], weights[1], weights[2]],
        'string': [weights[1], weights[2], weights[0]]
    }

    audio_divisions = 10

    # Split the RGB values into segments
    subdivided_blue_array = np.array_split(rgb_dict["3"], audio_divisions)
    subdivided_red_array = np.array_split(rgb_dict["1"], audio_divisions)
    subdivided_green_array = np.array_split(rgb_dict["2"], audio_divisions)

    pad_waves = []
    lead_waves = []
    string_waves = []

    brightness = (avg_red_overall + avg_green_overall + avg_blue_overall)/3
    
    for i in range(audio_divisions):
        pad_waves.append((np.sin(np.deg2rad(subdivided_blue_array[i]))))
        lead_waves.append((signal.square(np.deg2rad(subdivided_red_array[i])))*.2)
        string_waves.append((signal.sawtooth(np.deg2rad(subdivided_green_array[i]))))

    concatenated_pad_waves = np.concatenate(pad_waves)
    concatenated_lead_waves = np.concatenate(lead_waves)
    concatenated_string_waves = np.concatenate(string_waves)

    concatenated_pad_waves, concatenated_lead_waves, concatenated_string_waves = ensure_same_dimension([concatenated_pad_waves, concatenated_lead_waves, concatenated_string_waves])

    
    # Combine the waves based on the dominant color
    match dominant_color:
        case 'blue':
            weights = templates['pad']
            combined_wave = (np.multiply(concatenated_pad_waves, weights[2]) +
                             (np.multiply(concatenated_lead_waves, weights[0]) / avg_blue_overall) +
                             (np.multiply(concatenated_string_waves, weights[1]) / avg_blue_overall))
        case 'green':
            weights = templates["string"]
            combined_wave = ((np.multiply(concatenated_pad_waves, weights[2]) / avg_green_overall) +
                             (np.multiply(concatenated_lead_waves, weights[0]) / avg_green_overall) +
                             (np.multiply(concatenated_string_waves, weights[1])))
        case 'red':
            weights = templates["lead"]
            combined_wave = ((np.multiply(concatenated_pad_waves, weights[2]) / avg_red_overall) +
                             (np.multiply(concatenated_lead_waves, weights[0])) +
                             (np.multiply(concatenated_string_waves, weights[1]) / avg_red_overall))

    sample_rate = 30000

    #change the amplitude of the combined wave
    combined_wave = combined_wave*intensity
    # Apply the intensity factor to control the output waveform amplitude
    random_id = random.randint(0,100)
    # Normalize the combined wave to the range of int16 (to avoid distortion)
    scipy.io.wavfile.write(f"{out_path}output{random_id}-{dominant_color}.wav", sample_rate, (combined_wave * 32767).astype(np.int16))  
    
    return combined_wave

def generate_audio_nosplit(arrays, out_path):
    sample_rate = 44800

    for i in range(len(arrays)):

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
            combined_wave_normalized = combined_wave  

        # Convert the normalized wave to int16
        combined_wave_int16 = np.int16(combined_wave_normalized * 32767) 
        sine_wave_int16 = np.int16(array_sine * 32767) 
        cosine_wave_int16 = np.int16(array_cosine * 32767)  
        square_wave_int16 = np.int16(array_square * 32767) 

        # Saving files to disk
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
        generate_rgb(Images[i], out_path, kernel_size, step_size)
        # convolution_results.append(results)



    # generate_audio_nosplit(convolution_results, out_path)

    print("Finished")
    return 


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
        kernel_size = 10
        step_size = 3
        imagetoaudio(img_path, out_path, kernel_size, step_size)

        print("Done")
