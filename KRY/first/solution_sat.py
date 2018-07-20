from satispy import Variable
from satispy.solver import Minisat
import string

N_B = 32
N = 8 * N_B
vs = [Variable("i" + str(i)) for i in range(N)]


def sat_to_number(res, var):
    r = 0
    for i in range(1, N):
        if (res[var[i]]):
            r = r | 1 << (i - 1)
    if (res[var[0]]):
        r = r | 1 << (N - 1)
    return r


def satify_bit(bit, v, v1, v2):
    if not bit:
        v = -v
    return (v & -v1 & -v2) | (-v & (v1 | v2))


def rev_step(x):
    exp = satify_bit(x & 1, vs[0], vs[1], vs[2])
    for i in range(1, N):
        exp = exp & satify_bit((x >> i) & 1, vs[i], vs[(i + 1) % N], vs[(i + 2) % N])

    return sat_to_number(Minisat().solve(exp), vs)


int1 = int.from_bytes(open("in/bis.txt", 'rb').read(N_B), 'little')
int2 = int.from_bytes(open("in/bis.txt.enc", 'rb').read(N_B), 'little')

keystr = int1 ^ int2

for i in range(N // 2):
    keystr = rev_step(keystr)

key = keystr.to_bytes(N_B, 'little').decode('ascii')
print("".join(filter(lambda x: x in string.printable, key)), end='')
