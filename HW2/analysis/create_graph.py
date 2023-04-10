import pandas as pd                # 데이터를 저장하고 처리하는 패키지
import matplotlib as mpl           # 그래프를 그리는 패키지
import matplotlib.pyplot as plt    # 그래프를 그리는 패키지

for i in range (1, 9):
    prefix = f"h=0.0{i}"

    explicit_euler_numpy = pd.read_csv(f'{prefix}/explicit_euler.csv').to_numpy()
    explicit_euler_time = explicit_euler_numpy[:,0]
    explicit_euler_distance = explicit_euler_numpy[:,1]


    implicit_euler_numpy = pd.read_csv(f'{prefix}/implicit_euler.csv').to_numpy()
    implicit_euler_time = implicit_euler_numpy[:,0]
    implicit_euler_distance = implicit_euler_numpy[:,1]

    symplectic_euler_numpy = pd.read_csv(f'{prefix}/symplectic_euler.csv').to_numpy()
    symplectic_euler_time = symplectic_euler_numpy[:,0]
    symplectic_euler_distance = symplectic_euler_numpy[:,1]

    plt.subplot(4, 2, i)      

    plt.title(prefix, fontsize=7)
    plt.xlabel('time', fontsize=5)
    plt.ylabel('distance', fontsize=5)

    plt.plot(explicit_euler_time, explicit_euler_distance, 'r', label='explicit_euler')
    plt.plot(implicit_euler_time, implicit_euler_distance, 'g', label='implicit_euler')
    plt.plot(symplectic_euler_time, symplectic_euler_distance, 'b', label='symplectic_euler')

    plt.legend(loc='upper right', fontsize=5)

plt.show()
