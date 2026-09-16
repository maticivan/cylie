# Insertion and expansion of cylinders around unit ball

Ivan Matic, Rados Radoicic, and Andreas Spomer

This repository contains the verifier for the computer-assisted proofs in the
paper above. For the most current version of the paper, please visit
<https://imomath.com/maticivan>.

## Building

    make

On macOS with a Homebrew-installed GCC, use `make forMac` instead of `make`.

## Running

Form 1:

    ./cylie -i certificate.txt

Form 2:

    ./cylie -i constructions

## Third-party code

The directory `src/boost` contains an unmodified vendored subset of Boost
1.91.0 (Multiprecision and Config), used for fixed-width exact integer
arithmetic.

## License

MIT for the authors' code; the vendored Boost subset in `src/boost` is under
the Boost Software License 1.0. See `LICENSE`.
