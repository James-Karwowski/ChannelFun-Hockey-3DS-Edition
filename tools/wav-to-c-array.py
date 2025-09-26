import wave
import struct

def wav_to_c_array(wav_file_path, output_c_file_path, array_name="audio_data"):
    with wave.open(wav_file_path, 'rb') as wf:
        n_channels = wf.getnchannels()
        sampwidth = wf.getsampwidth()
        framerate = wf.getframerate()
        n_frames = wf.getnframes()

        raw_data = wf.readframes(n_frames)

        # Determine C data type based on sample width
        if sampwidth == 1:
            c_type = "int8_t"
            fmt = "<b" * n_frames * n_channels # 8-bit signed
        elif sampwidth == 2:
            c_type = "int16_t"
            fmt = "<h" * n_frames * n_channels # 16-bit signed
        else:
            raise ValueError("Unsupported sample width")

        samples = struct.unpack(fmt, raw_data)

    with open(output_c_file_path, 'w') as f:
        f.write(f"#include <stdint.h>\n\n")
        f.write(f"const {c_type} {array_name}[] = {{\n")
        
        # Format for C array, e.g., 10 samples per line
        for i, sample in enumerate(samples):
            f.write(f"    {sample},")
            if (i + 1) % 10 == 0: # Newline after 10 samples
                f.write("\n")
        f.write(f"\n}};\n")
        f.write(f"const size_t {array_name}_len = {len(samples)};\n")

wav_to_c_array("../romfs/sfx/collision.wav", "audio_data_collision.h")
wav_to_c_array("../romfs/sfx/goal.wav", "audio_data_goal.h")