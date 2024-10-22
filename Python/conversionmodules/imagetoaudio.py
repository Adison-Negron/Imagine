import sys
import os
import glob
import numpy as np
from PIL import Image
from scipy import signal
import math
import scipy

def image_convolution(image, kernel_size, step_size):
    """
    Compute the average RGB values for each channel
    of an image array in blocks of size kernel_size x kernel_size,
    stepping by step_size.

    Parameters
    ----------
    image : numpy array
        The image array to process
    kernel_size : int
        The size of the kernel to use
    step_size : int
        The step size to use

    Returns
    -------
    rgb_dict : dict
        A dictionary with keys '1', '2', and '3' containing the
        average red, green, and blue values respectively, for each
        block of the image array
    """

    if image is None:
        raise ValueError('Image cannot be null')

    if kernel_size <= 0 or step_size <= 0:
        raise ValueError('Kernel size and step size must be positive')

    x = image.shape[0]
    y = image.shape[1]

    if kernel_size > x or kernel_size > y:
        raise ValueError('Kernel size cannot be larger than the image')

    red_values = image[:, :, 0]
    green_values = image[:, :, 1]
    blue_values = image[:, :, 2]

    rgb_dict = {'1': [], '2': [], '3': []}

    # Calculate average values for each channel
    for i in range(0, x - kernel_size + 1, step_size):
        for j in range(0, y - kernel_size + 1, step_size):
            avg_red = (np.mean(red_values[i:i + kernel_size, j:j + kernel_size])+0.000000000001) / 256
            avg_green = (np.mean(green_values[i:i + kernel_size, j:j + kernel_size])+0.000000000001) / 256
            avg_blue = (np.mean(blue_values[i:i + kernel_size, j:j + kernel_size])+0.000000000001) / 256
            
            rgb_dict['1'].append(avg_red)
            rgb_dict['2'].append(avg_green)
            rgb_dict['3'].append(avg_blue)
    return rgb_dict

def interpolate_rgb_array(array, target_length):
    """
    Interpolates an array of RGB values to a target length.

    Parameters
    ----------
    array : numpy array
        The array of RGB values to interpolate
    target_length : int
        The target length of the interpolated array

    Returns
    -------
    numpy array
        The interpolated array of RGB values
    """
    if array is None:
        raise ValueError('Array cannot be null')
    if target_length <= 0:
        raise ValueError('Target length must be greater than 0')

    return np.interp(np.linspace(0, len(array) - 1, target_length), 
                     np.arange(len(array)), array)

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


def generate_sound(
    rgb_dict,
    out_path,
    sound_level,
    file_name,
    sample_rate=44800,
    sound_duration=6,
    modulation_intensity=0.4,
    modulation_envelope_intensity=0.8,
    modulation_duration=6,
    lfo_scalar_freq=1,
    lfo_scalar_amplitude=1,
    lfo_intensity=1,
    overtone_num_scalar=1,
    lfo_amount_scalar=1
):
    """
    Generate a sound based on the given RGB values.

    Parameters
    ----------
    rgb_dict : dict
        A dictionary mapping color channel names ('1', '2', '3') to lists of RGB values.
    out_path : str
        The path to write the output sound file to.
    sound_level : float
        The volume of the sound, from 0 (silent) to 1 (maximum volume).
    file_name : str
        The name of the output file, without extension.
    sample_rate : int, optional
        The sample rate of the output sound file, in Hz. Default is 44800.
    sound_duration : float, optional
        The duration of the sound, in seconds. Default is 6.
    modulation_intensity : float, optional
        The intensity of the modulation, from 0 (no modulation) to 1 (maximum modulation). Default is 0.4.
    modulation_envelope_intensity : float, optional
        The intensity of the modulation envelope, from 0 (no modulation) to 1 (maximum modulation). Default is 0.8.
    modulation_duration : float, optional
        The duration of the modulation, in seconds. Default is 6.
    lfo_scalar_freq : float, optional
        The frequency scaling factor for the LFO. Default is 1.
    lfo_scalar_amplitude : float, optional
        The amplitude scaling factor for the LFO. Default is 1.
    lfo_intensity : float, optional
        The intensity of the LFO, from 0 (no LFO) to 1 (maximum LFO). Default is 1.

    Returns
    -------
    None
    """
    if rgb_dict is None:
        raise ValueError('rgb_dict cannot be null')
    if out_path is None:
        raise ValueError('out_path cannot be null')
    if file_name is None:
        raise ValueError('file_name cannot be null')
    if sound_level < 0 or sound_level > 2:
        raise ValueError('sound_level must be between 0 and 1')

    avg_red_overall = np.mean(rgb_dict['1'])
    avg_green_overall = np.mean(rgb_dict['2'])
    avg_blue_overall = np.mean(rgb_dict['3'])

    # Find dominant color channel
    dominant_color = max([('red', avg_red_overall), 
                          ('green', avg_green_overall), 
                          ('blue', avg_blue_overall)], key=lambda x: x[1])[0]

    # Set frequency for C4 tone (~261.6 Hz)
    base_frequency = 261.6  # Duration of the sound in seconds

    # Create a time array
    time = np.arange(0, sound_duration, 1/sample_rate)

    # Interpolate RGB array to match the time array length
    interpolate_red = interpolate_rgb_array(rgb_dict['1'], len(time))
    interpolate_green = interpolate_rgb_array(rgb_dict['2'], len(time))
    interpolate_blue = interpolate_rgb_array(rgb_dict['3'], len(time))
    
    if dominant_color == 'red':
        combined_wave = modulate_frequency('square', time, interpolate_red, base_frequency, 
                                           modulation_duration=modulation_duration, modulation_intensity=modulation_intensity, envelope_intensity=modulation_envelope_intensity,intensity = .9) * sound_level
    elif dominant_color == 'green':
        combined_wave = modulate_frequency('saw', time, interpolate_green, base_frequency,
                                           modulation_duration=modulation_duration, modulation_intensity=modulation_intensity, envelope_intensity=modulation_envelope_intensity,intensity = .9) * sound_level
    else:  # 'blue'
        combined_wave = modulate_frequency('sine', time, interpolate_blue, base_frequency, 
                                           modulation_duration=modulation_duration, modulation_intensity=modulation_intensity, envelope_intensity=modulation_envelope_intensity,intensity = .9) * sound_level

    #Generate overtones and apply lfo

    average_color = color_avg(avg_red_overall,avg_green_overall,avg_blue_overall)

    final_sound = modify_base_tone(sound = combined_wave, color_average=average_color, overtone_type = 'sine', time=time, interpolate_red=interpolate_red, interpolate_green=interpolate_green, interpolate_blue=interpolate_blue, intensity = lfo_intensity, scalar_freq=lfo_scalar_freq, scalar_amplitude=lfo_scalar_amplitude,base_freq=base_frequency,overtone_num_scalar = overtone_num_scalar,lfo_amount_scalar = lfo_amount_scalar)
    

    # Save the sound file
    final_sound = final_sound * sound_level
    final_sound = (final_sound * 32767).astype(np.int16)
    
    # Write the output to a WAV file
    output_file_name = f"{out_path}{file_name}_output_{dominant_color}.wav"
    try:
        scipy.io.wavfile.write(output_file_name, sample_rate, final_sound)
    except Exception as e:
        print(f"Error writing to file: {e}")


def color_avg(r, g, b):
    """
    Calculate the average of the given RGB values.

    Parameters
    ----------
    r, g, b : float
        The red, green, and blue color values.

    Returns
    -------
    float
        The average of the given RGB values.
    """
    if r is None or g is None or b is None:
        raise ValueError('RGB values cannot be null')
    return np.mean([r, g, b])


def color_diff(r, g, b):
    
    """
    Calculate the difference between the given RGB values.

    Parameters
    ----------
    r, g, b : float
        The red, green, and blue color values.

    Returns
    -------
    float
        The difference between the given RGB values.
    """
    if r is None or g is None or b is None:
        raise ValueError('RGB values cannot be null')
    return r - g - b

def calculate_overtone_frequencies(base_freq, num, type):
    """
    Calculate the overtone frequencies based on the given parameters.

    Parameters
    ----------
    base_freq : float
        The base frequency for the overtones.
    num : int
        The number of overtones to calculate.
    type : str
        The type of waveform for the overtones. Can be 'sine', 'square', or 'sawtooth'.

    Returns
    -------
    list
        A list of calculated overtone frequencies.
    """
    frequencies = []
    
    if type == 'sine':
        frequencies = [base_freq * n for n in range(2, num + 2)]
    elif type == 'square':
        frequencies = [base_freq * n + 1 for n in range(2, num + 2)]
    elif type == 'sawtooth':
        frequencies = [base_freq * n + 1 for n in range(2, num + 2)]

    return frequencies
def map_to_range_with_variability(x, input_min, input_max, output_min=0.2, output_max=30, variability_factor=2):
    # Clamp the input value to ensure it's within the expected range
    """
    Map a value from an input range to an output range with added variability.

    The input value is first clamped to the input range, then scaled to [0, 1].
    An exponential function is applied to increase the variability of the output.
    Finally, the value is linearly mapped to the output range.

    Parameters
    ----------
    x : float
        The input value.
    input_min : float
        The minimum of the input range.
    input_max : float
        The maximum of the input range.
    output_min : float, optional
        The minimum of the output range. Defaults to 0.2.
    output_max : float, optional
        The maximum of the output range. Defaults to 30.
    variability_factor : float, optional
        The exponent to which the normalized input should be raised. Defaults to 2.

    Returns
    -------
    float
        The mapped value.
    """
    if x is None:
        raise ValueError('Input value cannot be null')
    if input_min is None:
        raise ValueError('Input minimum cannot be null')
    if input_max is None:
        raise ValueError('Input maximum cannot be null')
    if output_min is None:
        raise ValueError('Output minimum cannot be null')
    if output_max is None:
        raise ValueError('Output maximum cannot be null')
    if variability_factor is None:
        raise ValueError('Variability factor cannot be null')
    if input_min >= input_max:
        raise ValueError('Input range is invalid')
    if output_min >= output_max:
        raise ValueError('Output range is invalid')
    

    x = max(min(x, input_max), input_min)
    # Scale the input to [0, 1]
    normalized_input = (x - input_min) / (input_max - input_min)
    # Apply an exponential function to increase variability
    variable_value = math.pow(normalized_input, variability_factor)
    # Linearly map the variable value to the output range
    return output_min + (output_max - output_min) * variable_value


def apply_overtones(sound, wave_type, base_freq, brightness, rgb_dict, time,overtone_num_scalar=1):
    """
    Apply overtones to a sound based on an image's characteristics.

    Parameters
    ----------
    sound : array_like
        The sound to which overtones should be applied.
    wave_type : str
        The type of wave to apply overtones to. Options are 'sine', 'square', and 'sawtooth'.
    base_freq : float
        The base frequency of the sound.
    brightness : float
        The brightness of the image.
    rgb_dict : dict
        A dictionary with keys '1', '2', and '3' containing the red, green, and blue color channels of the image.
    time : array_like
        An array of time values at which the sound should be evaluated.

    Returns
    -------
    array_like
        The sound with overtones applied.
    """
    if sound is None:
        raise ValueError('Input sound cannot be null')
    if wave_type is None:
        raise ValueError('Input wave type cannot be null')
    if base_freq is None:
        raise ValueError('Input base frequency cannot be null')
    if brightness is None:
        raise ValueError('Input brightness cannot be null')
    if rgb_dict is None:
        raise ValueError('Input RGB dictionary cannot be null')
    if time is None:
        raise ValueError('Input time array cannot be null')

    # Derive a seed from the image's characteristics (e.g., average of RGB channels)
    seed_value = int(np.mean([np.mean(rgb_dict['1']), np.mean(rgb_dict['2']), np.mean(rgb_dict['3'])]) * 100)

    # Calculate intensity adjustment factor based on brightness
    intensity_scale = 0.5 + (0.8 * brightness)  # Intensity scale varies with brightness

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
            overtone_amount = int(round(4 + (12 - 4) * brightness)*overtone_num_scalar)
            overtone_frequencies = calculate_overtone_frequencies(base_freq, overtone_amount, 'sine')
            for n in range(overtone_amount):
                intensity = calculate_intensity(n, overtone_amount)
                sound += np.sin(2 * np.pi * overtone_frequencies[n] * time) * intensity

        case 'square':
            overtone_amount = int(round(10 + (20 - 10) * brightness)*overtone_num_scalar)
            overtone_frequencies = calculate_overtone_frequencies(base_freq, overtone_amount, 'square')
            for n in range(overtone_amount):
                intensity = calculate_intensity(n, overtone_amount)
                sound += np.square(2 * np.pi * overtone_frequencies[n] * time) * intensity

        case 'sawtooth':
            overtone_amount = int(round(8 + (15 - 8) * brightness)*overtone_num_scalar)
            overtone_frequencies = calculate_overtone_frequencies(base_freq, overtone_amount, 'sawtooth')
            for n in range(overtone_amount):
                intensity = calculate_intensity(n, overtone_amount)
                sound += np.sawtooth(2 * np.pi * overtone_frequencies[n] * time) * intensity

    return sound

def brightness(rgb):
    return np.mean(rgb)


def multiply_wave(wave, second_wave, scalar=1):
    """
    Multiply two waves together with an optional scalar factor.

    Parameters
    ----------
    wave : array_like
        The first wave to be multiplied.
    second_wave : array_like
        The second wave to be multiplied.
    scalar : float, optional
        The scalar factor to apply to the second wave. Defaults to 1.

    Returns
    -------
    array_like
        The product of the two waves, scaled by the scalar factor.
    """
    if wave is None or second_wave is None:
        raise ValueError("wave and second_wave cannot be null")
    if scalar is None:
        raise ValueError("scalar cannot be null")
    return wave * (second_wave*scalar)


def apply_lfo(sound, color_average, time, interpolate_red, interpolate_green, interpolate_blue, intensity=1, scalar_freq=1, scalar_amplitude=1,lfo_amount_scalar=1):
    """
    Apply a Low Frequency Oscillator (LFO) to a sound wave.

    Parameters
    ----------
    sound : numpy array
        The input sound wave to be modified.
    color_average : float
        The average color value of the image.
    time : numpy array
        The time array of the sound wave.
    interpolate_red : numpy array
        The interpolated red color channel of the image.
    interpolate_green : numpy array
        The interpolated green color channel of the image.
    interpolate_blue : numpy array
        The interpolated blue color channel of the image.
    intensity : float, optional
        The intensity of the LFO. Defaults to 1.
    scalar_freq : float, optional
        The frequency scalar of the LFO. Defaults to 1.
    scalar_amplitude : float, optional
        The amplitude scalar of the LFO. Defaults to 1.

    Returns
    -------
    numpy array
        The modified sound wave.
    """
    lfo_amount = int(round(1 + (5 - 1) * color_average)*lfo_amount_scalar)
    split_red = np.array_split(interpolate_red, lfo_amount)
    split_green = np.array_split(interpolate_green, lfo_amount)
    split_blue = np.array_split(interpolate_blue, lfo_amount)

    lfo_frequencies = []
    for i in range(lfo_amount):
        segment_avg_red = np.mean(split_red[i])
        segment_avg_green = np.mean(split_green[i])
        segment_avg_blue = np.mean(split_blue[i])
        max_val = np.max([segment_avg_red, segment_avg_green, segment_avg_blue])
        min_val = np.min([segment_avg_red, segment_avg_green, segment_avg_blue])

        delta = max_val - min_val
        lightness = (max_val + min_val) / 2

        if delta == 0:
            saturation = 0.01
        else:
            if lightness < 0.5:
                saturation = delta / (max_val + min_val)
            else:
                saturation = delta / (2 - max_val - min_val)

        if max_val == segment_avg_red:
            lfo_frequencies.append('square')
        elif max_val == segment_avg_blue:
            lfo_frequencies.append('sine')
        else:
            lfo_frequencies.append('sawtooth')

    lfo_sound = np.ones_like(sound)
    for i in range(len(lfo_frequencies)):
        if lfo_frequencies[i] == 'sine':
            lfo_sound *= (interpolate_blue * scalar_amplitude) * np.sin(2 * np.pi * (map_to_range_with_variability(color_average + saturation, 0, 1) * scalar_freq) * time) * intensity
        elif lfo_frequencies[i] == 'square':
            lfo_sound *= (interpolate_red * scalar_amplitude) * signal.square(2 * np.pi * (map_to_range_with_variability(color_average + saturation, 0, 1) * scalar_freq) * time) * intensity
        elif lfo_frequencies[i] == 'sawtooth':
            lfo_sound *= (interpolate_green * scalar_amplitude) * signal.sawtooth(2 * np.pi * (map_to_range_with_variability(color_average + saturation, 0, 1) * scalar_freq) * time) * intensity

    return multiply_wave(lfo_sound, sound)

def modify_base_tone(sound, color_average, overtone_type,base_freq, time, interpolate_red, interpolate_green, interpolate_blue, intensity=1, scalar_freq=1, scalar_amplitude=1,overtone_num_scalar=1,lfo_amount_scalar = 1):
    """
    Modify a base sound wave by applying overtones and a low frequency
    oscillator (LFO).

    Parameters
    ----------
    sound : numpy array
        The input sound wave to be modified.
    color_average : float
        The average color value of the image.
    overtone_type : str
        The type of overtones to apply. Can be 'sine', 'square', or 'sawtooth'.
    time : numpy array
        The time array of the sound wave.
    interpolate_red : numpy array
        The interpolated red color channel of the image.
    interpolate_green : numpy array
        The interpolated green color channel of the image.
    interpolate_blue : numpy array
        The interpolated blue color channel of the image.
    intensity : float, optional
        The intensity of the LFO. Defaults to 1.
    scalar_freq : float, optional
        The frequency scalar of the LFO. Defaults to 1.
    scalar_amplitude : float, optional
        The amplitude scalar of the LFO. Defaults to 1.

    Returns
    -------
    numpy array
        The modified sound wave.
    """
    if sound is None:
        raise ValueError('Input sound cannot be null')
    if color_average is None:
        raise ValueError('Input color_average cannot be null')
    if overtone_type not in ['sine', 'square', 'sawtooth']:
        raise ValueError('Invalid overtone_type. Must be "sine", "square", or "sawtooth"')
    if time is None:
        raise ValueError('Input time array cannot be null')
    if interpolate_red is None or interpolate_green is None or interpolate_blue is None:
        raise ValueError('Interpolated color channels cannot be null')

    sound = apply_overtones(sound = sound, wave_type = overtone_type, time = time, base_freq=base_freq, brightness= brightness(rgb = (interpolate_red, interpolate_green, interpolate_blue)), rgb_dict = {'1': interpolate_red, '2': interpolate_green, '3': interpolate_blue},overtone_num_scalar=overtone_num_scalar)

    # Apply LFO
    sound = apply_lfo(sound = sound , color_average = color_average, time = time, interpolate_red=interpolate_red, interpolate_green=interpolate_green, interpolate_blue=interpolate_blue, intensity = intensity, scalar_freq = scalar_freq, scalar_amplitude = scalar_amplitude, lfo_amount_scalar=lfo_amount_scalar)

    return sound


def main_generation_handler(
        img_path: str,
        out_path: str,
        kernel_size: int,
        step_size: int,
        sound_level: int,
        sample_rate: int,
        sound_duration: int,
        modulation_duration: int,
        modulation_intensity: float,
        modulation_envelope_intensity: float,
        overtone_num_scalar: float,
        lfo_scalar_freq: float,
        lfo_scalar_amplitude: float,
        lfo_intensity: float,
        lfo_amount_scalar: float
):
    
    
    #check if img_path is single file or folder, accept any img file path
    """
    Handles image to audio conversion, given an image path, output path, and conversion parameters

    Parameters
    ----------
    img_path: str
        path to the image file or folder containing image files
    out_path: str
        path to the output folder where the audio files will be saved
    kernel_size: int
        size of the convolution kernel
    step_size: int
        step size for the convolution
    sound_level: int
        level of the sound (0-100)
    sample_rate: int
        sample rate of the audio
    sound_duration: int
        duration of the sound in seconds
    modulation_intensity: float
        intensity of the modulation (0-1)
    modulation_envelope_intensity: float
        intensity of the modulation envelope (0-1)
    lfo_scalar_freq: float
        scalar frequency for the LFO
    lfo_scalar_amplitude: float
        scalar amplitude for the LFO
    lfo_intensity: float
        intensity of the LFO (0-1)

    Returns
    -------
    None
    """
    if os.path.isdir(img_path):
        image_files = glob.glob(os.path.join(img_path, "*.jpg")) + glob.glob(os.path.join(img_path, "*.png"))
        
        images = []
        file_names = []

        print("Processing image batch...")
        for file in image_files:
            file_name = os.path.splitext(os.path.basename(file))[0]
            print("Processing image: "+file_name)
            file_names.append(file_name)
            img = Image.open(file)
            img = img.convert('RGB')
            img_array = np.asarray(img)
            
            #check if image dimensions are greater than 3840*2160
            if img_array.shape[0] > 3840 or img_array.shape[1] > 2160:
                print("Error: Image dimensions are greater than 3840*2160, resizing to 3840*2160")
                img = img.resize((3840, 2160))
                img_array = np.asarray(img)

            images.append(img_array)

        print("Convolving image batch...")
        for n in range(len(images)):
            print("Convolving image: "+file_names[n])

            rgb_dict = image_convolution(images[n], kernel_size, step_size)

            generate_sound(rgb_dict = rgb_dict, 
                out_path = out_path,
                sound_level = sound_level,
                sample_rate = sample_rate,
                sound_duration = sound_duration,
                modulation_intensity = modulation_intensity,
                modulation_envelope_intensity = modulation_envelope_intensity,
                modulation_duration = modulation_duration,
                lfo_scalar_freq = lfo_scalar_freq,
                lfo_scalar_amplitude = lfo_scalar_amplitude,
                lfo_intensity = lfo_intensity,
                overtone_num_scalar = overtone_num_scalar,
                lfo_amount_scalar = lfo_amount_scalar,
                file_name=file_names[n]

            )
            print("Audio generated for: "+file_names[n])

    else:
        image = Image.open(img_path)
        image = image.convert('RGB')
        image_array = np.asarray(image)

        #check if image dimensions are greater than 3840*2160
        if image_array.shape[0] > 3840 or image_array.shape[1] > 2160:
            print("Error: Image dimensions are greater than 3840*2160, resizing to 3840*2160")
            file_name = os.path.splitext(os.path.basename(img_path))[0]
            image = image.resize((3840, 2160))
            image_array = np.asarray(image)

        print("Convolving image: "+file_name)
        rgb_dict = image_convolution(image_array, kernel_size, step_size)

        generate_sound(rgb_dict = rgb_dict, 
            out_path = out_path,
            sound_level = sound_level,
            sample_rate = sample_rate,
            sound_duration = sound_duration,
            modulation_intensity = modulation_intensity,
            modulation_envelope_intensity = modulation_envelope_intensity,
            modulation_duration = modulation_duration,
            lfo_scalar_freq = lfo_scalar_freq,
            lfo_scalar_amplitude = lfo_scalar_amplitude,
            lfo_intensity = lfo_intensity,
            overtone_num_scalar = overtone_num_scalar,
            lfo_amount_scalar = lfo_amount_scalar,
            file_name=file_name
        )
        print("Audio generated for: "+file_name)

    pass






if __name__ == "__main__":




    debug = True

    if not debug:
        img_path = sys.argv[1]
        out_path = sys.argv[2] if len(sys.argv) > 2 else None
        kernel_size = int(sys.argv[3]) if len(sys.argv) > 3 else 10
        step_size = int(sys.argv[4]) if len(sys.argv) > 4 else 2
        sound_level = int(sys.argv[5]) if len(sys.argv) > 5 else 1
        sample_rate = int(sys.argv[6]) if len(sys.argv) > 6 else 44800
        sound_duration = int(sys.argv[7]) if len(sys.argv) > 7 else 6
        modulation_intensity = float(sys.argv[8]) if len(sys.argv) > 8 else 0.4
        modulation_envelope_intensity = float(sys.argv[9]) if len(sys.argv) > 9 else 0.8
        modulation_duration = int(sys.argv[10]) if len(sys.argv) > 10 else 6
        lfo_scalar_freq = float(sys.argv[11]) if len(sys.argv) > 11 else 1
        lfo_scalar_amplitude = float(sys.argv[12]) if len(sys.argv) > 12 else 1
        lfo_intensity = float(sys.argv[13]) if len(sys.argv) > 13 else 1 
        overtone_num_scalar = float(sys.argv[14]) if len(sys.argv) > 14 else 1
        lfo_amount_scalar = float(sys.argv[15]) if len(sys.argv) > 15 else 1

        main_generation_handler(
            img_path= img_path,
            out_path = out_path,
            kernel_size = kernel_size,
            step_size = step_size,
            sound_level = sound_level,
            sample_rate = sample_rate,
            sound_duration = sound_duration,
            modulation_intensity =  modulation_intensity,
            modulation_envelope_intensity = modulation_envelope_intensity,
            modulation_duration = modulation_duration,
            lfo_scalar_freq = lfo_scalar_freq,
            lfo_scalar_amplitude = lfo_scalar_amplitude,
            lfo_intensity = lfo_intensity,
            overtone_num_scalar = overtone_num_scalar,
            lfo_amount_scalar = lfo_amount_scalar

        )

    else:
        img_path = os.getcwd() + "/imgs/"
        out_path = os.getcwd() + "/output/"
        kernel_size = 20
        step_size = 10
        sound_level = 1
        sample_rate = 44800
        sound_duration = 6
        modulation_intensity = .8
        modulation_envelope_intensity = 0.2
        modulation_duration = 6
        lfo_scalar_freq = .6
        lfo_scalar_amplitude = 1
        lfo_intensity = 1
        overtone_num_scalar = 3
        lfo_amount_scalar = 1



        main_generation_handler(
            img_path= img_path,
            out_path = out_path,
            kernel_size = kernel_size,
            step_size = step_size,
            sound_level = sound_level,
            sample_rate = sample_rate,
            sound_duration = sound_duration,
            modulation_intensity =  modulation_intensity,
            modulation_envelope_intensity = modulation_envelope_intensity,
            modulation_duration = modulation_duration,
            lfo_scalar_freq = lfo_scalar_freq,
            lfo_scalar_amplitude = lfo_scalar_amplitude,
            lfo_intensity = lfo_intensity,
            lfo_amount_scalar = lfo_amount_scalar,
            overtone_num_scalar = overtone_num_scalar
        )

    
