# BSQ

Fast enough B(iggest)SQ(uare).

Ever wandered how fast it could get using Dynamic programming and min neighbours approach ?

Well, if not, you're served anyway.

```txt
time ./build/linux/x86_64/release/bsq mouli_maps/intermediate_map_10000_10000 > /dev/null

real	0m0.168s
user	0m0.155s
sys	0m0.011s
```

## Explanation and caveats

This program assumes maps will be bellow UINT16_MAX x UINT16_MAX, mainly because provided maps does not exceed 10000 x 10000, but also because planning for such sizes imply a slow down for smaller ones, which are much more representative in benchmarks. Stay assured, the program will warn you if you try such big maps anyway.

Most of the optimization made was by adding `-O3` and `-march=native` to let the compiler by as agressive and platform specific as possible; fairly obvious to say it represented more than 50% of optimization work but hey, there you go still.

About the real code-wise optimization, well it mainly boil down to the fact i use only two small 1D buffers and proceed "step by step" without a single allocation, memset or memcpy mid-solving process. Things are kept straightforward, and both `perf` and `gprofng` used to profile this program indicated that more than 70% of total time taken by the program is spent between line 57 and 70; which is expected behavior and tends to show decent optimization.

## Compilation

Using [xmake](https://xmake.io/guide/quick-start.html), simply run at this repo's root:

```bash
xmake
```

build files can therefore be found under `build/`.

## Tests

A bash script is available to test all provided maps, use it like so:

```bash
./tests/tester.sh build/linux/x86_64/release/bsq
```

## Epitech disclaimer

My advice for any Epitech/42 students looking for "inspiration", this code being pretty compact, chances of you getting flagged are fairly high, I wouldn't try it.
