# Base64 Encoder/Decoder

Simple command-line base64 encoder/decoder written in C.  
  
This project was created to familiarize myself with:

- Parsing command-line arguments
- File input/output
- Error handling
- Binary data manipulation and bitwise operations
- Understanding base64 in web development

## Demo

[![asciicast](https://asciinema.org/a/rdYjEroQStpuiQc9iiFbx5tI0.svg)](https://asciinema.org/a/rdYjEroQStpuiQc9iiFbx5tI0)

## base64enc.c

- Converts binary/text data to base64 encoded format
- Wraps output at 76 characters per line
- Handles padding with '=' characters as needed

## base64dec.c

- Decodes base64 encoded data back to original format
- Validates input format and padding

## Installation

```bash
make # Build both encoder and decoder
make clean # Remove generated files
```

## Usage

**Encoding**

```bash
./base64enc [FILE]         # Encode file to base64
./base64enc < input.txt    # Encode from stdin
```

**Decoding**

```bash
./base64dec [FILE]         # Decode base64 file
./base64dec < encoded.txt  # Decode from stdin
```

