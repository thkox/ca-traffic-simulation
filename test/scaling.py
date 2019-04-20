import numpy as np
import subprocess
import sys, os
import scipy.stats
import matplotlib.pyplot as plt
from tqdm import tqdm

ntrials = 1

def simulate(nthread):
    sp = subprocess.Popen(['./cats', str(nthread), '>', 'output.txt'],
                          stdout=subprocess.PIPE)
    lines = sp.stdout.readlines()
    try:
        data_line = lines[5].decode(sys.stdout.encoding)
        comp_time = float(data_line.split(':')[1].strip().split(' ')[0])
    except Exception as e:
        print(e)
        for line in lines:
            print(line)
        sys.exit()
    return comp_time

def write_input(ncells):
    with open('cats-input.txt', 'w') as input_file:
        input_file.write('2\n%d\n1\n6\n6\n5\n0.588\n1.0\n100\n3.904\n1' % ncells)

def main():
    subprocess.call(['cp', '../cmake-build-release/cats', '.'])

    plt.figure(figsize=(6,6))
    nthreads = [1,2,3,4]
    ncells_dict = {1000:'-rD', 10000:'-g^', 100000:'-bo'}
    for ncells in ncells_dict.keys():
        comp_times = []
        write_input(ncells)
        for nthread in nthreads:
            comp_time = 0.0
            for _ in tqdm(range(ntrials)):
                comp_time += simulate(nthread)
            comp_time /= ntrials
            comp_times.append(comp_time)
            print(nthread, comp_time)

        comp_times = np.array(comp_times)

        plt.loglog(nthreads, comp_times[0] * comp_times**-1, ncells_dict[ncells],
                   markersize=6, label=r'$N_{\rm{cell}} = %d$' % ncells)

    plt.loglog([1,nthreads[-1]], [1,nthreads[-1]], '--k', label='Reference')

    plt.xlim(1,nthreads[-1])
    plt.ylim(1,nthreads[-1])

    plt.xlabel(r'Number of Threads')
    plt.ylabel(r'Speedup')
    plt.legend(frameon=False)
    plt.gca().set_aspect('equal', 'box')
    plt.tight_layout()

    plt.savefig('speedup.pdf')

    plt.show()

if __name__ == '__main__':
    main()
