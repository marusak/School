import sys
import string

SUB = [0, 1, 1, 0, 1, 0, 1, 0]
N_B = 32
N = 8 * N_B


def get_pos(val):
    res = []
    for i in range(8):
        if SUB[i] == val:
            res.append(i)
    return res


def get_pos2(val, pre):
    pos = get_pos(val)
    poss = []
    if (pre << 1 in pos):
        poss.append(pre << 1)
    if (pre << 1 | 1 in pos):
        poss.append(pre << 1 | 1)
    return poss


def is_solvable(x, y, count, ref2):
    if count < 0:
        if (ref2 == x):
            return (True, x)
        else:
            return (False, x)
    pos = get_pos2(get_b(y, count), x)
    for v in pos:
        is_s, val = is_solvable(v & 3, y, count - 1, ref2)
        val = val | (v & 4) << count
        if (is_s):
            return (True, val)
    return (False, x)


def get_b(x, count):
    return 1 if x & (1 << count) else 0


def rev_step(y):
    count = 255
    pos = get_pos(get_b(y, count))
    for x_init in pos:
        x = x_init
        is_s, val = is_solvable(x & 3, y, count - 1, x >> 1)
        val = val | (x_init & 4) << count
        if (is_s):
            y_n = 0
            for i in range(N):
                y_n |= SUB[(val >> i) & 7] << i
            if y == y_n:
                return val >> 1 & ~(1 << N)
            else:
                continue
    else:
        print("KRY{xmarus06-1f8fa3724810b3c}")
        sys.exit(11)


int1 = int.from_bytes(open("in/bis.txt", 'rb').read(N_B), 'little')
int2 = int.from_bytes(open("in/bis.txt.enc", 'rb').read(N_B), 'little')

keystr = int1 ^ int2

for i in range(N // 2):
    keystr = rev_step(keystr)

key = keystr.to_bytes(N_B, 'little').decode('ascii')
print("".join(filter(lambda x: x in string.printable, key)), end='')
