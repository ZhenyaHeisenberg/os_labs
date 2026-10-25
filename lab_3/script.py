import secrets

COUNT = 1_000_000
BITS = 512
HEX_LEN = BITS // 4  # 128

with open("data.txt", "w") as f:
    for _ in range(COUNT):
        n = secrets.randbits(BITS)
        f.write(f"{n:0{HEX_LEN}x}\n")

print("Done")