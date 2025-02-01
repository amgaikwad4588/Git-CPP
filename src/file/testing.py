import hashlib

def calculate_blob_sha1(fcontent):
    """
    Calculate the SHA-1 hash of a Git blob object based on its content.

    :param fcontent: The content of the file as a string.
    :return: The SHA-1 hash of the blob object as a hexadecimal string.
    """
    # Prepare the blob header
    fcontent += "\x0A"
    blob_header = f"blob {len(fcontent)}\x00"
    print(blob_header)

    # Combine the header and content
    blob = blob_header + fcontent
    # Print the last character of the blob
    last_char = blob[-1]
    print(f"Last character of the blob: '{last_char}'")
    print(f"Hexadecimal value of the last character: {hex(ord(last_char))}")

    # Calculate the SHA-1 hash
    sha1_hash = hashlib.sha1(blob.encode('utf-8')).hexdigest()

    return sha1_hash

def print_with_warning(fcontent):
    """
    Iterate through each character of the content and print it. If the character is '\x0A',
    print a warning in yellow color.

    :param fcontent: The content of the file as a string.
    """

    count = 0
    pink_c = 0
    for c in fcontent:
        count += 1
        if c == '\x0A':
            print("\033[33m\\x0A\033[0m", end="")
        if c == ' ':
            print(f"\033[95m{pink_c}\033[0m", end="")
            pink_c += 1

        print(c, end="")

    print()
    print("COUNT: ", count)

# Example usage
fcontent = """import random
import os
import shutil

random_words = [
    "humpty",
    "dumpty",
    "horsey",
    "donkey",
    "yikes",
    "monkey",
    "doo",
    "scooby",
    "dooby",
    "vanilla",
]

random.seed(1)


def words(count):
    def f():
        yielded = []
        while len(yielded) < count:
            chosen = random.choice(random_words)
            if chosen in yielded:
                next
            else:
                yielded.append(chosen)
                yield chosen

    return list(f())


def create():
    for word in random_words:
        shutil.rmtree(word, ignore_errors=True)

    folder_names = words(5)
    for folder_name in folder_names:
        print(f"- Creating {folder_name}")
        os.mkdir(folder_name)
        sub_folder_names = words(random.randint(0, 10))
        for sub_folder_name in sub_folder_names:
            print(f" - {sub_folder_name}")
            os.mkdir(f"{folder_name}/{sub_folder_name}")
            file_names = words(random.randint(0, 10))
            for file_name in file_names:
                print(f"   - {file_name}")
                f = open(f"{folder_name}/{sub_folder_name}/{file_name}", "w")
                contents = " ".join(words(random.randint(0, 10)))
                f.write(contents)
                f.close()


if __name__ == "__main__":
    create()
"""

sha1 = calculate_blob_sha1(fcontent)
print(f"SHA-1 of the blob: {sha1}")

print_with_warning(fcontent)
