import subprocess

default_k = 2
default_c = 2
default_epsilon = 0.8
default_r_tuples = 1e5
default_s_tuples = 2e5
default_M = 64
default_B = 32
index_nested_loop_runner = './index_nested_loop_runner'
covering_index_nested_loop_runner = './covering_index_nested_loop_runner'
sort_merge_runner = './external_sort_merge_runner'
index_sort_merge_runner = './index_sort_merge_runner'
covering_index_sort_merge_runner = './covering_index_sort_merge_runner'
hash_join_runner = './external_hash_join_runner'

runners = [
    sort_merge_runner,
    hash_join_runner,
    index_nested_loop_runner,
    index_sort_merge_runner,
    covering_index_nested_loop_runner,
    covering_index_sort_merge_runner,
]


def execution(
    runner,
    index,
    k=default_k,
    c=default_c,
    epsilon=default_epsilon,
    r_tuples=default_r_tuples,
    s_tuples=default_s_tuples,
    M=default_M,
    B=default_B,
    ingestion=False,
):
    assert r_tuples * k * (1 - epsilon) < s_tuples
    if ingestion:
        cmd = [
            runner,
            f'--index={index}',
            f'--k={k}',
            f'--c={c}',
            f'--epsilon={epsilon}',
            f'--r_tuples={r_tuples}',
            f'--s_tuples={s_tuples}',
            f'--M={M}',
            f'--B={B}',
            f'--ingestion',
        ]
    else:
        cmd = [
            runner,
            f'--index={index}',
            f'--k={k}',
            f'--c={c}',
            f'--epsilon={epsilon}',
            f'--r_tuples={r_tuples}',
            f'--s_tuples={s_tuples}',
            f'--M={M}',
            f'--B={B}',
        ]
    cmd = ' '.join(cmd)
    print(cmd)
    proc = subprocess.Popen(
        cmd,
        stdout=subprocess.PIPE,
        universal_newlines=True,
        shell=True,
    )
    proc.wait()
    print(proc.stdout.read())


def k_experiment(list_k=[1, 2, 4, 8]):
    for k in list_k:
        for i, runner in enumerate(runners):
            if i == 0:
                execution(runner, index='', k=k, ingestion=True)
            elif 'index' in runner:
                for index in ['comp', 'lazy', 'eager']:
                    execution(runner, index=index, k=k)
            else:
                execution(runner, index='', k=k)


if __name__ == "__main__":
    subprocess.run(['make', 'all'])
    k_experiment([4])
