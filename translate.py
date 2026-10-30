import os
import re
import telnetlib
import time

HOST = "192.168.43.1"
PORT = 9090

ROOT_DIR = os.path.dirname(os.path.abspath(__file__))


def natural_key(path):
    return [
        int(x) if x.isdigit() else x.lower()
        for x in re.split(r'(\d+)', path)
    ]

def normalize_quotes(s):
    """Ganti kutip miring Unicode dengan kutip ASCII biasa."""
    table = {
        "\u201c": '"',   # “
        "\u201d": '"',   # ”
        "\u201e": '"',   # „
        "\u2018": "'",   # ‘
        "\u2019": "'",   # ’
    }
    for old, new in table.items():
        s = s.replace(old, new)
    return s


def strip_outer_quotes(s):
    """Buang hanya 1 kutip ASCII di awal dan 1 di akhir."""
    s = s.strip()
    if s.startswith('"'):
        s = s[1:]
    if s.endswith('"'):
        s = s[:-1]
    return s


def read_block(path):
    with open(path, "r", encoding="utf-8", errors="ignore") as f:
        lines = f.readlines()

    original_lines = []
    translate_lines = []
    mode = None

    for line in lines:
        if line.startswith("text_original="):
            mode = "original"
            original_lines.append(line[len("text_original="):].rstrip("\n"))

        elif line.startswith("text_translate="):
            mode = "translate"
            translate_lines.append(line[len("text_translate="):].rstrip("\n"))

        elif line.startswith("posisi_overlay="):
            mode = None

        elif mode == "original":
            original_lines.append(line.rstrip("\n"))

        elif mode == "translate":
            translate_lines.append(line.rstrip("\n"))

    original = normalize_quotes(strip_outer_quotes("\n".join(original_lines)))
    translate = strip_outer_quotes("\n".join(translate_lines))

    return lines, original, translate


def write_translation(path, lines, translation):
    """
    Ganti bagian text_translate=, bagian lain dibiarkan apa adanya.
    """
    new_line = 'text_translate="' + translation.replace("\n", " ") + '"\n'

    output = []
    mode = None
    wrote_translate = False

    for line in lines:
        if line.startswith("text_translate="):
            if not wrote_translate:
                output.append(new_line)
                wrote_translate = True
            mode = "translate"
            continue

        # field lain menghentikan mode translate (mis. posisi_overlay)
        if line.startswith("posisi_overlay=") or line.startswith("text_original="):
            mode = None

        if mode == "translate":
            # buang sisa baris translate lama
            continue

        output.append(line)

    if not wrote_translate:
        if output and not output[-1].endswith("\n"):
            output.append("\n")
        output.append(new_line)

    with open(path, "w", encoding="utf-8") as f:
        f.writelines(output)


def translate_text(tn, text):
    if text == "":
        text = "zz"

    tn.write(text.encode("utf-8"))

    for i in range(4):
        tn.write(b"\n")

    result = tn.read_until(
        b"============= zzzzzzText Output ================",
        timeout=30
    )
    result = result.decode("utf-8", errors="ignore")

    try:
        result = result.split("============= Text Output ================", 1)[1]
        result = result.split("============= zzzzzzText Output ================", 1)[0]
        return result
    except Exception:
        print("WARNING: format response server tidak dikenal")
        return result


def find_blocks():
    blocks = []
    for folder, subfolders, files in os.walk(ROOT_DIR):
        for file in files:
            if file.startswith("block") and file.endswith(".txt"):
                blocks.append(os.path.join(folder, file))
    blocks.sort(key=natural_key)
    return blocks


def main():
    print("Root:")
    print(ROOT_DIR)
    print()

    blocks = find_blocks()
    print("Found %d block files" % len(blocks))
    print()

    if not blocks:
        print("Tidak ditemukan block*.txt")
        return

    print("Connecting translation server %s:%d ..." % (HOST, PORT))
    tn = telnetlib.Telnet(HOST, PORT)
    tn.read_until(b"Translate Server 1.0")
    print("Translation server connected.")
    print()

    total = len(blocks)
    translated = 0
    skipped = 0
    failed = 0

    for index, path in enumerate(blocks, 1):
        relative = os.path.relpath(path, ROOT_DIR)
        print("[%d/%d] %s" % (index, total, relative))

        try:
            lines, original, translate = read_block(path)

            if translate.strip() != "":
                print("  SKIP: translate sudah ada")
                skipped += 1
                continue

            if original.strip() == "":
                print("  SKIP: original kosong")
                skipped += 1
                continue

            print("  original: %d chars" % len(original))

            hasil = translate_text(tn, original)

            if hasil is None or hasil.strip() == "":
                print("  ERROR: hasil translation kosong")
                failed += 1
                continue

            hasil = hasil.strip()
            write_translation(path, lines, hasil)

            print("  translated: %d chars" % len(hasil))
            translated += 1
            time.sleep(1)

        except Exception as e:
            print("  ERROR:", e)
            failed += 1

    print()
    print("================================")
    print("Translation selesai")
    print("Total     :", total)
    print("Translated:", translated)
    print("Skipped   :", skipped)
    print("Failed    :", failed)
    print("================================")


main()
