import os


input_folder = "input_txt"     
output_folder = "output_txt"   


os.makedirs(output_folder, exist_ok=True)


for filename in os.listdir(input_folder):
    if filename.endswith(".txt"):
        input_path = os.path.join(input_folder, filename)
        output_path = os.path.join(output_folder, filename)

        with open(input_path, "r", encoding="utf-8") as infile, \
             open(output_path, "w", encoding="utf-8") as outfile:
            for line in infile:
                # Tab to |
                converted_line = line.replace('\t', '|')
                outfile.write(converted_line)

print("Output Path", os.path.abspath(output_folder))
