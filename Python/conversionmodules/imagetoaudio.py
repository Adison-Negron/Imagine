import numpy as np
from PIL import Image
import os
import glob
import scipy.io
import sys
import random
from scipy import signal
import wave
import math
import hashlib

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
    generate_audio_nosplit(resultant_arr, out_path)
    return


def convolve_rgb(img_array,kernel, step=2):
    x = img_array.shape[0]
    y = img_array.shape[1]

    red_values = img_array[:, :, 0]
    green_values = img_array[:, :, 1]
    blue_values = img_array[:, :, 2]

    rgb_dict = {'1': [], '2': [], '3': []}

    # Calculate average values for each channel
    for i in range(0, x - kernel + 1, step):
        for j in range(0, y - kernel + 1, step):
            avg_red = (np.mean(red_values[i:i + kernel, j:j + kernel])+0.000000000001) / 256
            avg_green = (np.mean(green_values[i:i + kernel, j:j + kernel])+0.000000000001) / 256
            avg_blue = (np.mean(blue_values[i:i + kernel, j:j + kernel])+0.000000000001) / 256
            
            rgb_dict['1'].append(avg_red)
            rgb_dict['2'].append(avg_green)
            rgb_dict['3'].append(avg_blue)

    
    
    return rgb_dict

def brightness(rgb):
    return np.mean(rgb)

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

def generate_audio_1(rgb_dict, out_path, intensity, file_name):

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

    brightness = (avg_red_overall + avg_green_overall + avg_blue_overall) / 3

    for i in range(audio_divisions):
        pad_waves.append((np.sin(np.deg2rad(subdivided_blue_array[i]))))
        lead_waves.append((signal.square(np.deg2rad(subdivided_red_array[i])))*.2)
        string_waves.append((signal.sawtooth(np.deg2rad(subdivided_green_array[i]))))

    concatenated_pad_waves = np.concatenate(pad_waves)
    concatenated_lead_waves = np.concatenate(lead_waves)
    concatenated_string_waves = np.concatenate(string_waves)

    concatenated_pad_waves, concatenated_lead_waves, concatenated_string_waves = ensure_same_dimension(
        [concatenated_pad_waves, concatenated_lead_waves, concatenated_string_waves])

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

    # Adjust the amplitude of the combined wave
    combined_wave = combined_wave * intensity

    # Normalize the combined wave to the range of int16 (to avoid distortion)
    output_file_name = f"{out_path}{file_name}_output_{dominant_color}.wav"
    scipy.io.wavfile.write(output_file_name, sample_rate, (combined_wave * 32767).astype(np.int16))

    print(f"Generated audio file: {output_file_name}")
    return combined_wave



def interpolate_rgb_array(array, target_length):
    return np.interp(np.linspace(0, len(array) - 1, target_length), 
                     np.arange(len(array)), array)

# Function to modulate the frequency with options for wave type and control over modulation parameters
def modulate_frequency(wave_type, base_time, array, base_frequency, 
                       modulation_duration=6, modulation_intensity=0.4, envelope_intensity=1.0,intensity = .8):
    """
    wave_type: 'sine', 'square', 'triangle'
    base_time: the time array
    array: the interpolated RGB array for modulation
    base_frequency: the base frequency (e.g., 261.6 for C4)
    modulation_duration: how long the modulation lasts before fading out
    modulation_intensity: how strong the modulation is applied
    envelope_intensity: intensity of the fade-out over time (0 = immediate stop, 1 = full duration)
    """

    # Apply a smoothing window to prevent drastic changes in the modulated frequency
    smooth_rgb = np.convolve(array, np.ones(50) / 50, mode='same')  # Smooth over 50 samples

    # Normalize smoothed RGB values to a modulation factor range
    modulated_frequency = base_frequency + (smooth_rgb - np.mean(smooth_rgb)) * modulation_intensity

    # Create a linear fade-out envelope for the modulation after 'modulation_duration' seconds
    fade_time = np.minimum(base_time / modulation_duration, 1)  # From 1 (full intensity) to 0 (no intensity)
    modulation_envelope = (1 - fade_time) * envelope_intensity  # Decreases from 1 to 0 over time, scaled by envelope_intensity

    # Apply modulation based on wave type
    match wave_type: 
        case 'sine':
            modulated_wave = np.sin(2 * np.pi * modulated_frequency * base_time) * modulation_envelope
        case 'saw':
            modulated_wave = signal.sawtooth(2 * np.pi * modulated_frequency * base_time) * modulation_envelope
        case 'square':
            modulated_wave = signal.square(2 * np.pi * modulated_frequency * base_time) * modulation_envelope

    return modulated_wave * intensity  # Adjust overall wave amplitude

# Main sound generation function
def generate_sound_v3(rgb_dict, out_path, level, file_name, sample_rate=44800):
    # Average RGB values
    avg_red_overall = np.mean(rgb_dict['1'])
    avg_green_overall = np.mean(rgb_dict['2'])
    avg_blue_overall = np.mean(rgb_dict['3'])

    # Find dominant color channel
    dominant_color = max([('red', avg_red_overall), 
                          ('green', avg_green_overall), 
                          ('blue', avg_blue_overall)], key=lambda x: x[1])[0]

    # Set frequency for C4 tone (~261.6 Hz)
    base_frequency = 261.6
    duration = 5  # Duration of the sound in seconds

    # Create a time array
    time = np.arange(0, duration, 1/sample_rate)

    # Interpolate RGB array to match the time array length
    interpolate_red = interpolate_rgb_array(rgb_dict['1'], len(time))
    interpolate_green = interpolate_rgb_array(rgb_dict['2'], len(time))
    interpolate_blue = interpolate_rgb_array(rgb_dict['3'], len(time))
    

    # Select the channel based on dominant color and apply modulation
    if dominant_color == 'red':
        combined_wave = modulate_frequency('square', time, interpolate_red, base_frequency, 
                                           modulation_duration=6, modulation_intensity=0.6, envelope_intensity=0.8,intensity = .9) * level
    elif dominant_color == 'green':
        combined_wave = modulate_frequency('saw', time, interpolate_green, base_frequency,
                                           modulation_duration=5, modulation_intensity=0.6, envelope_intensity=0.7,intensity = .9) * level
    else:  # 'blue'
        combined_wave = modulate_frequency('sine', time, interpolate_blue, base_frequency, 
                                           modulation_duration=4, modulation_intensity=0.6, envelope_intensity=1.0,intensity = .9) * level

    # Normalize the combined wave to the range of int16 (to avoid distortion)

    # Apply LFO


    combined_wave = modify_base_tone (combined_wave, color_avg(avg_red_overall, avg_green_overall, avg_blue_overall), 'sine', time, interpolate_red, interpolate_green, interpolate_blue, intensity=1, scalar_freq=1, scalar_amplitude=1)




    combined_wave = combined_wave * level
    combined_wave = (combined_wave * 32767).astype(np.int16)
    
    # Write the output to a WAV file
    output_file_name = f"{out_path}{file_name}_output_{dominant_color}.wav"
    scipy.io.wavfile.write(output_file_name, sample_rate, combined_wave)

    return 



def color_avg(r,g,b):
    return np.mean([r,g,b])

def color_diff(r,g,b):
    return r - g - b

def apply_lfo(sound, color_avg, type, variance, time, interpolate_red, interpolate_green, interpolate_blue, intensity=1, scalar_freq=1, scalar_amplitude=1):


    lfo_frequency = map_to_range_with_variability(color_avg, 0, 1)
    if type == 'sine':
        lfo = (interpolate_blue * scalar_amplitude) * np.sin(2 * np.pi * lfo_frequency * time) * intensity
    elif type == 'square':  
        lfo = (interpolate_red * scalar_amplitude) * signal.square(2 * np.pi * lfo_frequency * time) * intensity
    elif type == 'sawtooth':
        lfo = (interpolate_green * scalar_amplitude) * signal.sawtooth(2 * np.pi * lfo_frequency * time) * intensity

    return lfo

def map_to_range_with_variability(x, input_min, input_max, output_min=0.2, output_max=30, variability_factor=2):
    # Clamp the input value to ensure it's within the expected range
    x = max(min(x, input_max), input_min)
    # Scale the input to [0, 1]
    normalized_input = (x - input_min) / (input_max - input_min)
    # Apply an exponential function to increase variability
    variable_value = math.pow(normalized_input, variability_factor)
    # Linearly map the variable value to the output range
    return output_min + (output_max - output_min) * variable_value

def calculate_overtone_frequencies(base_freq,num,type):

 
    match type:
        case 'sine':
            frequencies = []
            for n in range(2,num+2):
                frequencies.append(base_freq*n)

            return frequencies
        case 'square':
            frequencies = []
            for n in range(2,num+2):
                frequencies.append(base_freq*n+1)
            return frequencies
        
        case 'sawtooth':

            frequencies = []
            for n in range(2,num+2):
                frequencies.append(base_freq*n+1)

    return frequencies



def apply_overtones(sound, wave_type, base_freq, brightness, rgb_dict, time):
    # Derive a seed from the image's characteristics (e.g., average of RGB channels)
    seed_value = int(np.mean([np.mean(rgb_dict['1']), np.mean(rgb_dict['2']), np.mean(rgb_dict['3'])]) * 100)
    
    # Calculate intensity adjustment factor based on brightness
    intensity_scale = 0.3 + (0.7 * brightness)  # Intensity scale varies with brightness

    def calculate_intensity(n, total_overtones):
        # Intensity is scaled between 0 and 1 based on position and seed value
        base_intensity = .7 - (n / total_overtones)  # Decreases linearly from 1 to 0
        variability = (seed_value % (n + 1)) / (total_overtones + 1)  # Adds some variation
        intensity = base_intensity * (1 + variability * intensity_scale)
        # Ensure the intensity remains within [0, 1] range
        return np.clip(intensity, 0, 1)

    # Match based on wave type and generate overtones with variable intensities
    match wave_type:
        case 'sine':
            overtone_amount = int(round(6 + (15 - 6) * brightness))
            overtone_frequencies = calculate_overtone_frequencies(base_freq, overtone_amount, 'sine')
            for n in range(overtone_amount):
                intensity = calculate_intensity(n, overtone_amount)
                sound += np.sin(2 * np.pi * overtone_frequencies[n] * time) * intensity

        case 'square':
            overtone_amount = int(round(15 + (25 - 15) * brightness))
            overtone_frequencies = calculate_overtone_frequencies(base_freq, overtone_amount, 'square')
            for n in range(overtone_amount):
                intensity = calculate_intensity(n, overtone_amount)
                sound += np.square(2 * np.pi * overtone_frequencies[n] * time) * intensity

        case 'sawtooth':
            overtone_amount = int(round(10 + (20 - 10) * brightness))
            overtone_frequencies = calculate_overtone_frequencies(base_freq, overtone_amount, 'sawtooth')
            for n in range(overtone_amount):
                intensity = calculate_intensity(n, overtone_amount)
                sound += np.sawtooth(2 * np.pi * overtone_frequencies[n] * time) * intensity

    return sound




def modify_base_tone(sound, color_avg, type, time, interpolate_red, interpolate_green, interpolate_blue, intensity=1, scalar_freq=1, scalar_amplitude=1):

    #apply overtones
    sound = apply_overtones(sound, type, 261.6, color_avg, {'1': interpolate_red, '2': interpolate_green, '3': interpolate_blue}, time)



    #apply lfo
    types = []
    lfos = []
    lfo_amount = int(round(1 + (5 - 1) * color_avg))
    #lfo_amount = int(abs(np.mod(3, 5) * color_diff))
    split_red = np.array_split(interpolate_red, lfo_amount)
    split_green = np.array_split(interpolate_green, lfo_amount)
    split_blue = np.array_split(interpolate_blue, lfo_amount)

    for elm in range(lfo_amount):
        segment_avg_red = np.mean(split_red[elm])
        segment_avg_blue = np.mean(split_blue[elm])
        segment_avg_green = np.mean(split_green[elm])
        
        max_val = np.max([segment_avg_red,segment_avg_blue,segment_avg_green])
        min_val = np.min([segment_avg_red,segment_avg_blue,segment_avg_green])

        delta = max_val - min_val
        lightness = (max_val + min_val) / 2

        if delta == 0:
            saturation = .01
        else:
            if lightness < 0.5:
                saturation = delta / (max_val + min_val)
            else:
                saturation = delta / (2 - max_val - min_val)
        

        if max_val == segment_avg_red:
            types.append('square')
        elif max_val == segment_avg_blue:
            types.append('sine')
        else:
            types.append('sawtooth') 

    for i in range(lfo_amount):
        lfo = apply_lfo(sound, color_avg+saturation, types[i], 1, time, interpolate_red, interpolate_green, interpolate_blue)
        lfos.append(lfo)
    final_lfo = np.ones_like(sound)
    for i in range(len(lfos)):
        final_lfo *= lfos[i]


    return final_lfo * sound
    return sound
        





def multiply_wave(wave, second_wave, scalar=1):


    return wave * (second_wave*scalar)


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
    file_names = []
    rerun = False
    if not os.path.isdir(out_path):
        rerun = True
        os.makedirs(out_path)

    
    if not os.path.isdir(img_path):
        os.makedirs(img_path)
        rerun = True

    if rerun:
        print("Error: First run detected, paths created,restart needed")
        exit()
        
    # Iterate over all .jpg files in the specified directory
    for file in glob.glob(img_path + "*.jpg"):
        # Extract the file name without extension
        file_name = os.path.splitext(os.path.basename(file))[0]
        file_names.append(file_name)
        
        # Open and process the image
        img = Image.open(file)
        img = img.convert('RGB')
        width, height = img.size
        aspect_ratio = width / height
        img = img.resize((int(aspect_ratio * 1024), 1024))  # Resize with consistent height
        img_array = np.asarray(img)

        Images.append(img_array)
        print(f"Processing image: {file_name}, Shape: {img_array.shape}")

    # Pass the image file name along with the image array
    for i in range(len(Images)):
        print(f"Convolving image {file_names[i]} with Kernel Size {kernel_size} and Step Size {step_size}")
        rgb_dict = convolve_rgb(Images[i], kernel_size, step_size, )  
        generate_sound_v3(rgb_dict=rgb_dict, out_path=out_path+"audionew/", level=.5, file_name=file_names[i],sample_rate=48000)
        # generate_audio_1(rgb_dict=rgb_dict,out_path=out_path+"audioold/", intensity=.9, file_name=file_names[i])
              



    print("Finished")
        #convolve(Images[i], out_path, kernel_size, step_size)
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
        kernel_size = 20
        step_size = 10
        imagetoaudio(img_path, out_path, kernel_size, step_size)

        print("Done")
