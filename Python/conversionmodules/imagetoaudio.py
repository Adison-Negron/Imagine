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
    
    # Stretch RGB data to match the length of `time`
def stretch_to_match_length(arr, target_len):
    return np.interp(np.linspace(0, len(arr), target_len), np.arange(len(arr)), arr)



def convolve_rgb(img_array, out_path, file_name,kernel, step=2, intensity=0.6):
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

    
    #generate_audio_1(rgb_dict, out_path, intensity, file_name)
    generate_sound_overtones(rgb_dict, out_path, intensity,file_name)


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
        pad_waves.append((np.sin(subdivided_blue_array[i])))
        lead_waves.append((signal.square(subdivided_red_array[i])) * .2)
        string_waves.append((signal.sawtooth(subdivided_green_array[i])))

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





def generate_sound_overtones(rgb_dict, out_path, intensity,file_name):
    # Compute overall average for each color channel
    avg_red_overall = np.mean(rgb_dict['1'])
    avg_green_overall = np.mean(rgb_dict['2'])
    avg_blue_overall = np.mean(rgb_dict['3'])

    # Find dominant color channel
    dominant_color = max([('red', avg_red_overall), 
                          ('green', avg_green_overall), 
                          ('blue', avg_blue_overall)], key=lambda x: x[1])[0]

    # Define weight templates for different sound types
    weights_template = {
        'pad': [0.1, 0.2, 0.3, 0.4, 0.5],
        'lead': [0.2, 0.4, 0.6, 0.8, 1.0],
        'string': [0.3, 0.5, 0.7, 0.9, 1.1]
    }

    overtones = random.randint(1,25)
    amplitude = intensity
    frequency = 261.6 # C4
    sample_rate = 30000
    time = np.linspace(0, 1, sample_rate)


    red_array = np.asarray(rgb_dict['1']).astype(np.float64)
    green_array = np.asarray(rgb_dict['2']).astype(np.float64)
    blue_array = np.asarray(rgb_dict['3']).astype(np.float64)
    overtone_weight = 800
    

    overtone_list = []
    
    match dominant_color:
        case 'blue':

            x = math.floor(pow(green_array.shape[0], 1/3))
            for n in range(x,green_array.shape[0]):
                try:
                    reduced_green_array = np.mean(green_array.reshape(-1, math.floor(n)), axis=1)
                    factor = math.floor(n)
                    break
                except:
                    continue

            reduced_blue_array = np.mean(blue_array.reshape(-1, factor), axis=1)
            reduced_red_array = np.mean(red_array.reshape(-1, factor), axis=1)
            time = time[:,np.newaxis]*reduced_blue_array
            base_tone = np.sin(2 * np.pi * frequency * time*reduced_blue_array)
            alternate_square = signal.square(2 * np.pi * frequency * time*reduced_red_array)/avg_blue_overall
            alternate_sawtooth = signal.sawtooth(2 * np.pi * frequency * time*reduced_green_array)/avg_blue_overall
            blue_hash = hash(avg_blue_overall)
            seed_str = str(blue_hash)
            seed_str = seed_str.replace('0','')

            for letter in seed_str:
                if letter !=1 or 0:
                    overtone_seed = int(letter)
                    seed_str = seed_str.replace(letter, '1')

                else:
                    
                    continue


            for i in range(overtones+1):

                if seed_str !='':
                    overtone_num = int(seed_str[i])
                    seed_str = seed_str.replace(str(overtone_num), '')
                    equal_temperament_frequency_overtone = pow(2, (overtone_seed*overtone_num)/12) * frequency
                    overtone = np.sin(2 * np.pi * equal_temperament_frequency_overtone * time*blue_array)
                    equal_temperament_frequency_undertone = pow(2, -(overtone_seed*overtone_num)/12) * frequency
                    undertone = np.sin(2 * np.pi * equal_temperament_frequency_undertone * time*blue_array)
                    tones = overtone + undertone
 
                
                else:
                    equal_temperament_frequency_overtone = pow(2, i/12) * frequency
                    overtone = np.sin(2 * np.pi * equal_temperament_frequency_overtone * time*reduced_blue_array)
                    equal_temperament_frequency_undertone = pow(2, -i/12) * frequency
                    undertone = np.sin(2 * np.pi * equal_temperament_frequency_undertone * time*reduced_blue_array)
                    tones = overtone + undertone

                freq_difference = abs(equal_temperament_frequency_overtone-frequency)
                overtone_intensity = .5/freq_difference *overtone_weight

                overtone_list.append(tones*overtone_intensity)

            final_overtone = np.zeros_like(time)
            for elm in overtone_list:
                final_overtone += elm

            
            final_wave = (base_tone*alternate_sawtooth*alternate_square)*(intensity)+final_overtone

            pass

        case 'green':            
            x = math.floor(pow(green_array.shape[0], 1/3))
            for n in range(x,green_array.shape[0]):
                try:
                    reduced_green_array = np.mean(green_array.reshape(-1, math.floor(n)), axis=1)
                    factor = math.floor(n)
                    break
                except:
                    continue

            reduced_blue_array = np.mean(blue_array.reshape(-1, factor), axis=1)
            reduced_red_array = np.mean(red_array.reshape(-1, factor), axis=1)
            time = time[:,np.newaxis]*reduced_green_array
            base_tone = signal.sawtooth(2 * np.pi * frequency * time*reduced_green_array)
            alternate_square = signal.square(2 * np.pi * frequency * time*reduced_red_array)/avg_green_overall
            alternate_sin = np.sin (2 * np.pi * frequency * time*reduced_blue_array)/avg_green_overall
            green_hash = hash(avg_green_overall)
            seed_str = str(green_hash)
            seed_str = seed_str.replace('0','')
            
            for letter in seed_str:
                if letter !='1' or '0':
                    overtone_seed = int(letter)
                    seed_str = seed_str.replace(letter, '1')

                else:
                    
                    continue


            for i in range(overtones+1):

                if seed_str !='':
                    overtone_num = int(seed_str[i])
                    seed_str = seed_str.replace(str(overtone_num), '')
                    equal_temperament_frequency_overtone = pow(2, (overtone_seed*overtone_num)/12) * frequency
                    overtone = np.sin(2 * np.pi * equal_temperament_frequency_overtone * time*reduced_blue_array)
                    equal_temperament_frequency_undertone = pow(2,-(overtone_seed*overtone_num)/12) * frequency
                    undertone = np.sin(2 * np.pi * equal_temperament_frequency_undertone * time*reduced_blue_array)
                    tones = overtone + undertone
 
                
                else:
                    equal_temperament_frequency_overtone = pow(2, i/12) * frequency
                    overtone = np.sin(2 * np.pi * equal_temperament_frequency_overtone * time*reduced_blue_array)
                    equal_temperament_frequency_undertone = pow(2, -i/12) * frequency
                    undertone = np.sin(2 * np.pi * equal_temperament_frequency_undertone * time*reduced_blue_array)
                    tones = overtone + undertone

                freq_difference = abs(equal_temperament_frequency_overtone-frequency)
                overtone_intensity = .5/freq_difference *overtone_weight

                overtone_list.append(tones*overtone_intensity)


            final_overtone = np.zeros_like(time)
            for elm in overtone_list:
                final_overtone += elm

            
            final_wave = (base_tone*alternate_square*alternate_sin)*(intensity)+final_overtone
                

            pass

        case 'red':
            x = math.floor(pow(green_array.shape[0], 1/3))
            for n in range(x,green_array.shape[0]):
                try:
                    reduced_green_array = np.mean(green_array.reshape(-1, math.floor(n)), axis=1)
                    factor = math.floor(n)
                    break
                except:
                    continue
            

            reduced_blue_array = np.mean(blue_array.reshape(-1, factor), axis=1)
            reduced_red_array = np.mean(red_array.reshape(-1, factor), axis=1)
            time = time[:,np.newaxis]*reduced_green_array
            base_tone = signal.square(2 * np.pi * frequency * time*reduced_red_array)
            alternate_sin = np.sin(2 * np.pi * frequency * time*reduced_blue_array)/avg_red_overall
            alternate_sawtooth = signal.sawtooth(2 * np.pi * frequency * time*reduced_green_array)/avg_red_overall
            red_hash = hash(avg_red_overall)
            seed_str = str(red_hash)
            seed_str = seed_str.replace('0','')
            
            for letter in seed_str:
                if letter !='1' or '0':
                    overtone_seed = int(letter)
                    seed_str = seed_str.replace(letter, '1')

                else:
                    
                    continue


            for i in range(overtones+1):

                if seed_str !='':
                    overtone_num = int(seed_str[i])
                    seed_str = seed_str.replace(str(overtone_num), '')
                    equal_temperament_frequency_overtone = pow(2, (overtone_seed*overtone_num)/12) * frequency
                    overtone = np.sin(2 * np.pi * equal_temperament_frequency_overtone * time*reduced_blue_array)
                    equal_temperament_frequency_undertone = pow(2, -(overtone_seed*overtone_num)/12) * frequency
                    undertone = np.sin(2 * np.pi * equal_temperament_frequency_undertone * time*reduced_blue_array)
                    tones = overtone + undertone
 
                
                else:
                    equal_temperament_frequency_overtone = pow(2, i/12) * frequency
                    overtone = np.sin(2 * np.pi * equal_temperament_frequency_overtone * time*reduced_blue_array)
                    equal_temperament_frequency_undertone = pow(2, -i/12) * frequency
                    undertone = np.sin(2 * np.pi * equal_temperament_frequency_undertone * time*reduced_blue_array)
                    tones = overtone + undertone

                freq_difference = abs(equal_temperament_frequency_overtone-frequency)
                overtone_intensity = .5/freq_difference *overtone_weight

                overtone_list.append(tones*overtone_intensity)


            final_overtone = np.zeros_like(time)
            for elm in overtone_list:
                final_overtone += elm

            
            final_wave = (base_tone*alternate_sawtooth*alternate_sin)*(intensity)+final_overtone



    # Normalize final_wave to be within the range [-1, 1]
    max_wave_value = np.max(np.abs(final_wave))  # Find the maximum absolute value in final_wave
    if max_wave_value > 0:
        final_wave /= max_wave_value  # Normalize the wave

    # Scale to int16 range and clip to avoid overflow
    final_sound = (final_wave * 32767)
    final_sound_int16 = np.clip(final_sound, -32768, 32767).astype(np.int16)

    # Write the sound to a .wav file
    random_id = random.randint(0, 1000)
    # scipy.io.wavfile.write(f"{out_path}output{random_id}-{dominant_color}.wav", sample_rate, final_sound_int16)
    with wave.open(f"{out_path}{file_name}-output{random_id}-{dominant_color}.wav", "w") as f:
        # 2 Channels.
        f.setnchannels(1)
        # 2 bytes per sample.
        f.setsampwidth(2)
        f.setframerate(sample_rate)
        f.writeframes(final_sound_int16.tobytes())

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
        convolve_rgb(Images[i], out_path,file_names[i], kernel_size, step_size, )

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
